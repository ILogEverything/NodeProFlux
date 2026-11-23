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
#include "NodeProFluxConstants.h"
#include "NodeProFluxResponse.h"
#include "IFanController.h"
#include "ILEDController.h"
#include "ITemperatureController.h"

/**
 * The central Controller which integrates all components. The main components of the NodeProFluxController
 * are the NodeProFluxFirmware and the LEDController. There can also be an optional TemperatureController and
 * FanController which are required if the device should be an Commander PRO.
 */
class NodeProFluxController {
public:
	/**
	 * The constructor used to create a Lighting only device.
	 *
	 * @param l The LEDController which should be used to control the LEDs of the created Lighting Node PRO
	 * @param c The NodeProFluxFirmware used to handle Firmware related commands
	 */
	NodeProFluxController(ILEDController* l, NodeProFluxFirmware* c);
	/**
	 * The constructor used to create a device with lighting, temperature and fan controller functionality (Commander
	 * PRO).
	 *
	 * @param l The LEDController which should be used to control the LEDs of the created Commander PRO
	 * @param t The TemperatureController which used to messure the temperature of the created Commander PRO
	 * @param f The FanController used to control the fans of the created Commander PRO
	 * @param c The NodeProFluxFirmware used to handle Firmware related commands
	 */
	NodeProFluxController(ILEDController* l, ITemperatureController* t, IFanController* f,
									  NodeProFluxFirmware* c);
	/**
	 * The only public function of the NodeProFluxController. It must be called to process a command which
	 * was received from iCUE. This function is normally called by NodeProFluxHID and
	 * NodeProFluxSerial adapters.
	 *
	 * @param command The command received from iCUE
	 * @param response The response callback which can be called to response to the command
	 */
	void handleCommand(const Command& command, NodeProFluxResponse* response);

	NodeProFluxFirmware* getFirmware(void);

private:
	NodeProFluxFirmware* const npfLightingFirmware;
	ILEDController* const ledController;
	ITemperatureController* const temperatureController;
	IFanController* const fanController;
};
