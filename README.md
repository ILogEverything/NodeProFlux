# NodeProFlux

> **Fork of [Legion2/CorsairLightingProtocol](https://github.com/Legion2/CorsairLightingProtocol)**, maintained independently.
> It keeps the original library API and adds compatibility with current FastLED releases (tested with **FastLED 3.10.10**).
> _This is not an official Corsair project._

## What this is

An Arduino library that makes a Pro Micro (ATmega32U4) show up as a **Corsair Lighting Node PRO**, so addressable LEDs can be driven from the PC. This fork is used with **SignalRGB** (iCUE and OpenRGB compatibility is inherited from the original library).

## Changes from upstream

- **FastLED 3.10.x compatibility.** Recent FastLED releases moved `dim8_video` into the `fl::` namespace, which broke the build of `FastLEDControllerUtils.cpp` (`'dim8_video' was not declared in this scope`). `CLP::gammaCorrection` now uses a local helper with the same result (`scale8_video(x, x)`), so it builds on both old and new FastLED versions.
- Example sketch for a multi-pin Node PRO with a "no data, lights off" timeout (see below).

Everything else is unchanged from upstream.

## Requirements

- A board based on the **ATmega32U4**: SparkFun Pro Micro, Arduino Leonardo, Arduino Micro.
- [FastLED](https://github.com/FastLED/FastLED) (tested with 3.10.10).
- The [CLP Boards](https://github.com/Legion2/CorsairLightingProtocolBoards) definitions (and the SparkFun board definitions if you use a Pro Micro), so the board presents Corsair USB IDs.
- An **external 5 V supply** for the LEDs, with its ground connected to the board's ground. Never power the strips from the USB port.

## Installation

1. Download this repository as a ZIP, or clone it into your Arduino `libraries` folder.
2. If the original "Corsair Lighting Protocol" library is installed, remove it first so the two don't conflict.
3. Install FastLED from the Library Manager.
4. Select the CLP board, open the example below and upload it.
5. Check that a **Lighting Node PRO** appears in the Windows device list, then open SignalRGB (or iCUE).

## Example: two-channel Node PRO with auto-off

This sketch drives 78 WS2812B LEDs on eight data pins, exposed as the two channels of a Node PRO:

| Channel | Pin | LEDs |
|---|---|---|
| 1 | 3, 4, 6, 7, 14, 15 | 8 each (48 total) |
| 2 | 16 | 8 |
| 2 | 10 | 22 (30 total on the channel) |

Each pin is a separate FastLED controller writing into a segment of the shared channel buffer, so the host sees one continuous strip per channel.

**Auto-off:** if no update arrives for 1 second (`NO_UPDATE_TIMEOUT_MS`), all LEDs are turned off. This is what switches the lights off when SignalRGB stops sending data or the PC is shut down while USB stays powered.

```cpp
#include <CorsairLightingProtocol.h>
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

// Change both IDs if you use more than one device on the same PC
const char mySerialNumber[] PROGMEM = "<YOUR_HEX_SERIAL>";

DeviceID deviceId = { 0x00, 0x00, 0x00, 0x00 };  // <YOUR_DEVICE_ID>
CorsairLightingFirmwareStorageStatic firmwareStorage(deviceId);
CorsairLightingFirmware firmware(CORSAIR_LIGHTING_NODE_PRO, &firmwareStorage);
FastLEDControllerStorageEEPROM storage;
FastLEDController ledController(&storage);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP, mySerialNumber);

uint32_t lastUpdateMs = 0;

void allLedsOff() {
  memset(ledsChannel1, 0, sizeof(ledsChannel1));
  memset(ledsChannel2, 0, sizeof(ledsChannel2));
  FastLED.show();
}

void setup() {
  CLP::disableBuildInLEDs();

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
  cHID.update();
  if (ledController.updateLEDs()) {
    FastLED.show();
    lastUpdateMs = millis();
  } else if (millis() - lastUpdateMs > NO_UPDATE_TIMEOUT_MS) {
    allLedsOff();
    lastUpdateMs = millis();
  }
}
```

### Adapting it to your setup

- **Pins and LED counts:** change the `DATA_PIN*` and `LEDS_PIN*` defines and add or remove `FastLED.addLeds` lines. Each pin must be a compile-time constant and a valid FastLED pin for the 32U4.
- **Colour order:** `GRB` suits most WS2812B strips. Change it if the colours are swapped.
- **Timeout:** raise or lower `NO_UPDATE_TIMEOUT_MS`. Test any static colour you plan to use in hardware lighting mode: with this logic the LEDs go dark if the controller reports no new frame for longer than the timeout.
- **Host setup:** in SignalRGB (or iCUE) set the LED count of each channel to match the sketch (48 and 30 in this example). In iCUE the amount is entered in groups of ten.
- **Several devices:** each one needs its own `Serial Number` (hex characters only) and `DeviceID`.

## Notes

- Interrupts are disabled while FastLED writes each strip. With long segments this can disturb USB communication, so keep segments short, as in this example.
- LED power draw can reach about 60 mA per LED at full white. Size the power supply and wiring for it.

## Inherited features

- Corsair DIY device protocol on ATmega32U4 over USB HID.
- FastLED chipsets (WS2812B, WS2801 and others).
- Hardware lighting mode.
- Multiple devices at the same time.
- Repeat or scale LED channels (`CLP::repeat`, `CLP::scale`), reverse direction (`CLP::reverse`) and brightness fix (`CLP::fixIcueBrightness`, only for LS100/LT100 emulation).

See the upstream [README](https://github.com/Legion2/CorsairLightingProtocol) and [API documentation](https://legion2.github.io/CorsairLightingProtocol/) for details on these.

## License

Licensed under the Apache License 2.0, like the original project. See [LICENSE](LICENSE).

## Credits

- [Legion2/CorsairLightingProtocol](https://github.com/Legion2/CorsairLightingProtocol), the original library
- [HoodLoader2](https://github.com/NicoHood/HoodLoader2)
- [Arduino HID Project](https://github.com/NicoHood/HID)
- [Protocol information](https://github.com/audiohacked/OpenCorsairLink/issues/70)
- [CorsairLightingProtocolBoards](https://github.com/Legion2/CorsairLightingProtocolBoards)

## Disclaimer

This is a do-it-yourself project, use it at your own risk. Not affiliated with or endorsed by Corsair.
