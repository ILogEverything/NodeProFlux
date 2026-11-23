/*
   Copyright 2019 Leon Kiefer

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once

#include "Arduino.h"
#include "NodeProFluxFirmware.h"

#define toBigEndian(a) highByte(a), lowByte(a)

#if NPF_DEBUG

#define NPF_STRCAT(a, b) a##b
#define NPF_STRCAT3(a, b, c) a##b##c

#define NPF_XSTRCAT(a, b) NPF_STRCAT(a, b)
#define NPF_XSTRCAT3(a, b, c) NPF_STRCAT3(a, b, c)

#if defined(NPF_DEBUG_PORT)
namespace NPF {
extern int printf(const char* format, ...);
extern int printf(const __FlashStringHelper* format, ...);
}  // namespace NPF
#define npfPrintf NPF::printf
#else
#define npfPrintf printf
#endif

namespace NPF {
static inline void printVar(uint8_t const* buf, uint32_t bufsize) {
	for (uint32_t i = 0; i < bufsize; i++) npfPrintf("%02X ", buf[i]);
}
extern void printData(uint8_t const* buf, uint32_t bufsize, bool address_table);
}  // namespace NPF

/* clang-format off */
#define NPF_LOG(n, ...)               NPF_XSTRCAT(NPF_LOG, n)(__VA_ARGS__)
#define NPF_LOG_VAR(n, ...)           NPF_XSTRCAT3(NPF_LOG, n, _VAR)(__VA_ARGS__)
#define NPF_LOG_HEX(n, ...)           NPF_XSTRCAT3(NPF_LOG, n, _HEX)(__VA_ARGS__)
#define NPF_LOG_DAT(n, buf, size, at) NPF_XSTRCAT3(NPF_LOG, n, _DAT)(buf, size, at)
#define NPF_LOG_LOCATION()            npfPrintf("%s: %d:\r\n", __PRETTY_FUNCTION__, __LINE__)
#define NPF_LOG_FAILED()              npfPrintf("%s: %d: Failed\r\n", __PRETTY_FUNCTION__, __LINE__)
#define NPF_LOG_FUNC(func)            func

// Log Level 1: Error
#define NPF_LOG1                      npfPrintf
#define NPF_LOG1_VAR(_x)              NPF::printVar((uint8_t const*)(_x), sizeof(*(_x)))
#define NPF_LOG1_HEX(_x)              npfPrintf(#_x " = %lX\r\n", (unsigned long) (_x) )
#define NPF_LOG1_DAT(_x, _y, _z)      NPF::printData((uint8_t const*)(_x), _y, _z)

// Log Level 2: Warn
#if NPF_DEBUG >= 2
#define NPF_LOG2                      NPF_LOG1
#define NPF_LOG2_VAR                  NPF_LOG1_VAR
#define NPF_LOG2_HEX                  NPF_LOG1_HEX
#define NPF_LOG2_DAT                  NPF_LOG1_DAT
#endif

// Log Level 3: Info
#if NPF_DEBUG >= 3
#define NPF_LOG3                      NPF_LOG1
#define NPF_LOG3_VAR                  NPF_LOG1_VAR
#define NPF_LOG3_HEX                  NPF_LOG1_HEX
#define NPF_LOG3_DAT                  NPF_LOG1_DAT
#endif

// Log Level 4: Data
#if NPF_DEBUG >= 4
#define NPF_LOG4                      NPF_LOG1
#define NPF_LOG4_VAR                  NPF_LOG1_VAR
#define NPF_LOG4_HEX                  NPF_LOG1_HEX
#define NPF_LOG4_DAT                  NPF_LOG1_DAT
#endif
/* clang-format on */

#endif  // NPF_DEBUG

#ifndef NPF_LOG
#define NPF_LOG(n, ...)
#define NPF_LOG_VAR(n, ...)
#define NPF_LOG_HEX(n, ...)
#define NPF_LOG_DAT(n, buf, size, at)
#define NPF_LOG_FUNC(n)
#endif

#define NPF_LOG0(...)
#define NPF_LOG0_VAR(...)
#define NPF_LOG0_HEX(...)
#define NPF_LOG0_DAT(...)

#ifndef NPF_LOG1
#define NPF_LOG1(...)
#define NPF_LOG1_VAR(...)
#define NPF_LOG1_HEX(...)
#define NPF_LOG1_DAT(...)
#endif

#ifndef NPF_LOG2
#define NPF_LOG2(...)
#define NPF_LOG2_VAR(...)
#define NPF_LOG2_HEX(...)
#define NPF_LOG2_DAT(...)
#endif

#ifndef NPF_LOG3
#define NPF_LOG3(...)
#define NPF_LOG3_VAR(...)
#define NPF_LOG3_HEX(...)
#define NPF_LOG3_DAT(...)
#endif

#ifndef NPF_LOG4
#define NPF_LOG4(...)
#define NPF_LOG4_VAR(...)
#define NPF_LOG4_HEX(...)
#define NPF_LOG4_DAT(...)
#endif

namespace NPF {

template <typename T>
void swap(T a, T b) {
	auto temp = *a;
	*a = *b;
	*b = temp;
}

template <typename T>
void reverse(T first, T last) {
	while ((first != last) && (first != --last)) {
		NPF::swap(first++, last);
	}
}

template <typename T>
void rotate(T first, T n_first, T last) {
	if (first == n_first) return;
	if (n_first == last) return;

	T read = n_first;
	T write = first;
	T next_read = first;

	while (read != last) {
		if (write == next_read) next_read = read;
		NPF::swap(read++, write++);
	}

	(rotate)(write, next_read, last);
	return;
}

uint16_t fromBigEndian(const byte& byte1, const byte& byte2);

/**
 * Convert value from range 0-100 to 0-255
 *
 * @param value the value which should be converted
 */
inline uint8_t convert100To255(uint8_t value) { return value * 2.5546875f; }

/**
 * Convert value from range 0-255 to 0-100.
 *
 * @param value the value which should be converted
 */
inline uint8_t convert255To100(uint8_t value) { return value / 2.5546875f; }

/**
 * Check if a device id is the special null id (00 00 00 00).
 *
 * @param deviceId the device id to check
 */
bool isNullID(const DeviceID& deviceId);

/**
 * Check if a device id is the special reset id (FF FF FF FF).
 *
 * @param deviceId the device id to check
 */
bool isResetID(const DeviceID& deviceId);

/**
 * This will disable the RX and TX built in LEDs on Arduino Leonardo, Micro and Pro Micro.
 */
void disableBuildInLEDs();

/**
 * Print the given DeviceID to Serial
 *
 * @param deviceId the device id to print
 */
void printDeviceID(const DeviceID& deviceId);

/*
 * Measure and print the framerate at the given interval in milliseconds. The higher this value the more precise the
 * result will be. This function should be called after FastLED.show() to count the FPS.
 *
 * @param interval the measurement interval in milliseconds
 */
void printFps(const int interval);
}  // namespace NPF
