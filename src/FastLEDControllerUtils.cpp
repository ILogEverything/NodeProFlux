#include "FastLEDControllerUtils.h"

#include <FastLED.h>
#include <math.h>

#include "CLPUtils.h"

static inline uint8_t clpDim8Video(uint8_t x) {
	return (uint8_t)(((uint16_t)x * x) >> 8) + (x ? 1 : 0);
}

void CLP::transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex) {
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		auto count = controller->getLEDCount(channelIndex);
		for (uint8_t fanIndex = 0; fanIndex < count / LL_FAN_LEDS; fanIndex++) {
			CLP::reverse(&leds[fanIndex * LL_FAN_LEDS], &leds[fanIndex * LL_FAN_LEDS + LL_FAN_LEDS - 1] + 1);
		}
	}
}

void CLP::transformLC100ToStrip(FastLEDController* controller, uint8_t channelIndex) {
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		auto count = controller->getLEDCount(channelIndex);
		for (uint8_t lc100Index = 0; lc100Index < count / LC100_LEDS; lc100Index++) {
			CLP::rotate(&leds[lc100Index * LC100_LEDS], &leds[lc100Index * LC100_LEDS + LC100_FIRST_LED_OFFSET],
						&leds[lc100Index * LC100_LEDS + LC100_LEDS - 1] + 1);
		}
	}
}

inline int scaleIndex(const int index, const float scaleFactor) { return round(index * scaleFactor); }

float scaleFactorOf(const int numberLEDsBefore, const int numberLEDsAfter) {
	return (float)(numberLEDsBefore - 1) / (numberLEDsAfter - 1);
}

void CLP::scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize) {
	auto leds = controller->getLEDs(channelIndex);
	const float scaleFactor = scaleFactorOf(controller->getLEDCount(channelIndex), scaleToSize);
	if (scaleFactor < 1.0f) {
		for (int ledIndex = scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledIndex] = leds[scaleIndex(ledIndex, scaleFactor)];
		}
	} else {
		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledIndex] = leds[scaleIndex(ledIndex, scaleFactor)];
		}
	}
}

void CLP::repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	for (int i = 1; i < times; i++) {
		memcpy(leds + (count * i), leds, sizeof(CRGB) * count);
	}
}

void CLP::scaleSegments(FastLEDController* controller, uint8_t channelIndex, const SegmentScaling* const segments,
						int segmentsCount) {
	auto leds = controller->getLEDs(channelIndex);
	int ledStripIndexAfterScaling = 0;
	int ledStripIndexBeforeScaling = 0;
	int totalLengthAfterScaling = 0;
	SegmentScaling downScaledSegments[segmentsCount];
	for (int i = 0; i < segmentsCount; i++) {
		const int segmentLength = segments[i].segmentLength;
		const int scaleToSize = min(segments[i].scaleToSize, segmentLength);
		const float scaleFactor = scaleFactorOf(segmentLength, scaleToSize);

		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndex(ledIndex, scaleFactor)];
		}
		ledStripIndexAfterScaling += scaleToSize;
		ledStripIndexBeforeScaling += segmentLength;
		downScaledSegments[i].segmentLength = scaleToSize;
		downScaledSegments[i].scaleToSize = segments[i].scaleToSize;
		totalLengthAfterScaling += segments[i].scaleToSize;
	}

	ledStripIndexBeforeScaling = ledStripIndexAfterScaling;
	ledStripIndexAfterScaling = totalLengthAfterScaling;
	for (int i = segmentsCount - 1; i >= 0; i--) {
		const float scaleFactor = scaleFactorOf(downScaledSegments[i].segmentLength, downScaledSegments[i].scaleToSize);
		ledStripIndexAfterScaling -= downScaledSegments[i].scaleToSize;
		ledStripIndexBeforeScaling -= downScaledSegments[i].segmentLength;
		for (int ledIndex = downScaledSegments[i].scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndex(ledIndex, scaleFactor)];
		}
	}
}

void CLP::reverse(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto maxIndex = controller->getLEDCount(channelIndex) - 1;
	for (int ledIndex = 0; ledIndex < maxIndex - ledIndex; ledIndex++) {
		CRGB temp = leds[ledIndex];
		leds[ledIndex] = leds[maxIndex - ledIndex];
		leds[maxIndex - ledIndex] = temp;
	}
}

void CLP::gammaCorrection(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	for (int ledIndex = 0; ledIndex < count; ledIndex++) {
		leds[ledIndex].r = clpDim8Video(leds[ledIndex].r);
		leds[ledIndex].g = clpDim8Video(leds[ledIndex].g);
		leds[ledIndex].b = clpDim8Video(leds[ledIndex].b);
	}
}

void CLP::fixIcueBrightness(FastLEDController* controller, uint8_t channelIndex) {
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		auto count = controller->getLEDCount(channelIndex);
		for (int ledIndex = 0; ledIndex < count; ledIndex++) {
			leds[ledIndex].r = leds[ledIndex].r * 2;
			leds[ledIndex].g = leds[ledIndex].g * 2;
			leds[ledIndex].b = leds[ledIndex].b * 2;
		}
	}
}
