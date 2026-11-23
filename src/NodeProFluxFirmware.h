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
#include "NodeProFluxConstants.h"
#include "NodeProFluxResponse.h"

#define FIRMWARE_VERSION_SIZE 3

const uint8_t npfLightingNodePROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x0A, 0x04};

#define npfLightingNodeCOREFirmwareVersion npfLightingNodePROFirmwareVersion

#define npfLS100FirmwareVersion npfLightingNodePROFirmwareVersion

const uint8_t npfCommanderPROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x09, 0xD4};

const uint8_t npfLT100FirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x01, 0x01, 0x38};

const uint8_t npfCommanderCOREFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x02, 0x06, 0xC9};

const uint8_t* const firmwareVersions[] PROGMEM = {
	npfLightingNodePROFirmwareVersion, npfCommanderPROFirmwareVersion, npfLightingNodeCOREFirmwareVersion,
	npfLS100FirmwareVersion,           npfLT100FirmwareVersion,        npfCommanderCOREFirmwareVersion};

struct DeviceID {
	uint8_t data[4];
};

/**
 * Interface to store the device id
 */
class NodeProFluxFirmwareStorage {
public:
	virtual void loadDeviceID(DeviceID& deviceID) const = 0;
	virtual void saveDeviceID(const DeviceID& deviceID) = 0;
};

class NodeProFluxFirmware {
public:
	NodeProFluxFirmware(npf_product_enum_t product, NodeProFluxFirmwareStorage* storage);
	void handleFirmwareCommand(const Command& command, const NodeProFluxResponse* response);
	void getDeviceID(DeviceID& deviceID) const;
	void setDeviceID(const DeviceID& deviceID);
	uint8_t getStatus();
	void setStatus(uint8_t status);
	uint8_t getProduct();

protected:
	NodeProFluxFirmwareStorage* const storage;
	const npf_product_enum_t product;
	DeviceID deviceId;

private:
	uint8_t status = PROTOCOL_STATUS_OK;
};
