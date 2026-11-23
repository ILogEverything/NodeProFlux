/*
   Node PRO with 8 data pins, two channels and automatic switch-off.

   Channel 1: pins 3, 4, 6, 7, 14, 15 with 8 LEDs each (48 LEDs)
   Channel 2: pin 16 with 8 LEDs + pin 10 with 22 LEDs (30 LEDs)

   If no update arrives for NO_UPDATE_TIMEOUT_MS (SignalRGB closed, PC off
   with USB still powered) all LEDs are switched off.
   The LEDs need their own 5V supply, with ground shared with the board.

   Licensed under the Apache License, Version 2.0, like the original library.
*/
#include <NodeProFlux.h>
#include <FastLED.h>

#define DATA_PIN2 3
#define DATA_PIN3 4
#define DATA_PIN4 6
#define DATA_PIN5 7
#define DATA_PIN6 14
#define DATA_PIN7 15
#define DATA_PIN8 16
#define DATA_PIN9 10

#define LEDS_PIN2 8
#define LEDS_PIN3 8
#define LEDS_PIN4 8
#define LEDS_PIN5 8
#define LEDS_PIN6 8
#define LEDS_PIN7 8
#define LEDS_PIN8 8
#define LEDS_PIN9 22

#define CH1_TOTAL (LEDS_PIN2 + LEDS_PIN3 + LEDS_PIN4 + LEDS_PIN5 + LEDS_PIN6 + LEDS_PIN7)
#define CH2_TOTAL (LEDS_PIN8 + LEDS_PIN9)

#define NO_UPDATE_TIMEOUT_MS 1000UL

CRGB ledsChannel1[CH1_TOTAL];
CRGB ledsChannel2[CH2_TOTAL];

// Serial number (hex characters only) and DeviceID must be unique for each device on the same PC
const char mySerialNumber[] PROGMEM = "5E0A1D3C7B92";

DeviceID deviceId = { 0x5E, 0x0A, 0x1D, 0x3C };
NodeProFluxFirmwareStorageStatic firmwareStorage(deviceId);
NodeProFluxFirmware firmware(NPF_LIGHTING_NODE_PRO, &firmwareStorage);
FastLEDControllerStorageEEPROM storage;
FastLEDController ledController(&storage);
NodeProFluxController npfController(&ledController, &firmware);
NodeProFluxHID npfHID(&npfController, mySerialNumber);

uint32_t lastUpdateMs = 0;

void allLedsOff() {
	memset(ledsChannel1, 0, sizeof(ledsChannel1));
	memset(ledsChannel2, 0, sizeof(ledsChannel2));
	FastLED.show();
}

void setup() {
	NPF::disableBuildInLEDs();

	uint16_t offset = 0;
	FastLED.addLeds<WS2812B, DATA_PIN2, GRB>(ledsChannel1, offset, LEDS_PIN2);
	offset += LEDS_PIN2;
	FastLED.addLeds<WS2812B, DATA_PIN3, GRB>(ledsChannel1, offset, LEDS_PIN3);
	offset += LEDS_PIN3;
	FastLED.addLeds<WS2812B, DATA_PIN4, GRB>(ledsChannel1, offset, LEDS_PIN4);
	offset += LEDS_PIN4;
	FastLED.addLeds<WS2812B, DATA_PIN5, GRB>(ledsChannel1, offset, LEDS_PIN5);
	offset += LEDS_PIN5;
	FastLED.addLeds<WS2812B, DATA_PIN6, GRB>(ledsChannel1, offset, LEDS_PIN6);
	offset += LEDS_PIN6;
	FastLED.addLeds<WS2812B, DATA_PIN7, GRB>(ledsChannel1, offset, LEDS_PIN7);

	offset = 0;
	FastLED.addLeds<WS2812B, DATA_PIN8, GRB>(ledsChannel2, offset, LEDS_PIN8);
	offset += LEDS_PIN8;
	FastLED.addLeds<WS2812B, DATA_PIN9, GRB>(ledsChannel2, offset, LEDS_PIN9);

	ledController.addLEDs(0, ledsChannel1, CH1_TOTAL);
	ledController.addLEDs(1, ledsChannel2, CH2_TOTAL);

	allLedsOff();
	lastUpdateMs = millis();
}

void loop() {
	npfHID.update();
	if (ledController.updateLEDs()) {
		FastLED.show();
		lastUpdateMs = millis();
	} else if (millis() - lastUpdateMs > NO_UPDATE_TIMEOUT_MS) {
		allLedsOff();
		lastUpdateMs = millis();
	}
}
