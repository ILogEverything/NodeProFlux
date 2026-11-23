#pragma once

#include "NPFUtils.h"
#include "NodeProFluxConstants.h"
#include "NodeProFluxController.h"
#include "NodeProFluxResponse.h"

#if defined(USE_TINYUSB)

#include "Adafruit_TinyUSB.h"

class NodeProFluxTinyUSBHID : NodeProFluxResponse {
public:
	/**
	 * Create a new adapter for NodeProFluxController using the default Serial Number.
	 *
	 * @param controller the NodeProFluxController
	 */
	NodeProFluxTinyUSBHID(NodeProFluxController* controller);
	/**
	 * Create a new adapter for NodeProFluxController using the given Serial Number for the usb interface.
	 *
	 * @param controller the NodeProFluxController
	 * @param serialNumber the Serial Number used for the USB interface
	 */
	NodeProFluxTinyUSBHID(NodeProFluxController* controller, const char* serialNumber);
	/**
	 * Setup the TinyUSB HID connection.
	 * This function must be called in setup.
	 */
	void setup(void);
	/**
	 * Read commands form HID interface and pass them to the contoller. This function must be called in loop.
	 */
	void update(void);

	static Command command;

	static int newData;

protected:
	NodeProFluxController* const controller;
	const char* serialNumber;
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif