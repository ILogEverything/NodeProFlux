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
#include "NodeProFluxHID.h"

#if defined(SUPPORT_RAW_HID)

#if defined(DEBUG) && defined(VERBOSE)
bool printCommand = PRINT_COMMAND;
bool printResponse = PRINT_RESPONSE;
#endif

NodeProFluxHID::NodeProFluxHID(NodeProFluxController* controller)
	: controller(controller) {
	NPF::RawHID.begin(rawhidData, sizeof(rawhidData));
}

NodeProFluxHID::NodeProFluxHID(NodeProFluxController* controller,
													   const char* serialNumber)
	: NodeProFluxHID(controller) {
	NPF::RawHID.setSerialNumber(serialNumber);
}

void NodeProFluxHID::update() {
	if (available()) {
		Command command;
		getCommand(command);
		controller->handleCommand(command, this);
	}
}

bool NodeProFluxHID::available() const { return NPF::RawHID.available() > 0; }

void NodeProFluxHID::getCommand(Command& command) {
	auto bytesAvailable = NPF::RawHID.available();
	if (bytesAvailable) {
		NPF::RawHID.readBytes(command.raw, sizeof(command.raw));
#if defined(DEBUG) && defined(VERBOSE)
		if (printCommand) {
			Serial.print(F("Received Command: "));
			Serial.print(command.command, HEX);
			Serial.print(" ");
			Serial.println(command.data[0], HEX);
		}
#endif
	}
}

void NodeProFluxHID::sendX(const uint8_t* data, const size_t x) const {
#if defined(DEBUG) && defined(VERBOSE)
	if (printResponse) {
		Serial.print(F("Send Response: "));
		Serial.println(data[0], HEX);
	}
#endif
	NPF::RawHID.write(data, x);
}

#endif
