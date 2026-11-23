#include "NodeProFluxTinyUSBHID.h"

#if defined(USE_TINYUSB)

const char* npfProducts[] = {NPF_LNP_PRODUCT, NPF_CP_PRODUCT,  NPF_LNC_PRODUCT,
								 NPF_SLC_PRODUCT, NPF_SLT_PRODUCT, NPF_CC_PRODUCT};
const int npfPIDs[] = {NPF_LNP_PID, NPF_CP_PID,  NPF_LNC_PID,
						   NPF_SLC_PID, NPF_SLT_PID, NPF_CC_PID};

Command NodeProFluxTinyUSBHID::command;
int NodeProFluxTinyUSBHID::newData;

/* clang-format off */
uint8_t const hid_report[] = {HID_USAGE_PAGE_N (HID_USAGE_PAGE_VENDOR | 0xC0, 2),
							  HID_USAGE_N      (0x0C00,                       2),
							  HID_COLLECTION   (HID_COLLECTION_APPLICATION     ),
							  HID_REPORT_SIZE  (8                              ),
							  HID_LOGICAL_MIN  (0x00                           ),
							  HID_LOGICAL_MAX_N(0xff,                         2),
							  HID_REPORT_COUNT (RESPONSE_SIZE                  ),
							  HID_USAGE        (0x01                           ),
							  HID_INPUT        (HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
							  HID_REPORT_COUNT (COMMAND_SIZE                   ),
							  HID_USAGE        (0x02                           ),
							  HID_OUTPUT       (HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
							  HID_COLLECTION_END};
/* clang-format on */

Adafruit_USBD_HID tudHid(hid_report, sizeof(hid_report), HID_ITF_PROTOCOL_NONE, 1, true);

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;

	NPF_LOG(2, F("Get report callback!\r\n"));

	return 0;
}

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
	(void)report_id;
	(void)report_type;

	if (bufsize <= sizeof(Command)) {
		memcpy(&NodeProFluxTinyUSBHID::command.raw, buffer, bufsize);
		NodeProFluxTinyUSBHID::newData = 1;
		NPF_LOG(4, F("Data received:\r\n"));
		NPF_LOG_DAT(4, &NodeProFluxTinyUSBHID::command.raw,
					sizeof(NodeProFluxTinyUSBHID::command), true);
	} else {
		NPF_LOG(2, F("Command too large\r\n"));
	}
}

NodeProFluxTinyUSBHID::NodeProFluxTinyUSBHID(NodeProFluxController* controller)
	: controller(controller), serialNumber(SERIAL_NUMBER) {}

NodeProFluxTinyUSBHID::NodeProFluxTinyUSBHID(NodeProFluxController* controller,
																	 const char* serialNumber)
	: controller(controller), serialNumber(serialNumber) {}

void NodeProFluxTinyUSBHID::setup(void) {
	NPF_LOG_FUNC(NPF_DEBUG_PORT.begin(NPF_DEBUG_BAUD));

	TinyUSBDevice.setManufacturerDescriptor(NPF_MANUFACTURER);
	TinyUSBDevice.setProductDescriptor(npfProducts[controller->getFirmware()->getProduct()]);
	TinyUSBDevice.setID(NPF_VID, npfPIDs[controller->getFirmware()->getProduct()]);
	TinyUSBDevice.setSerialDescriptor(serialNumber);

	tudHid.setReportCallback(get_report_callback, set_report_callback);
	tudHid.begin();

	while (!TinyUSBDevice.mounted()) delay(1);
}

void NodeProFluxTinyUSBHID::update(void) {
	if (newData) {
		controller->handleCommand(command, this);
		newData = 0;
	}
}

void NodeProFluxTinyUSBHID::sendX(const uint8_t* data, const size_t x) const {
	tudHid.sendReport(0, data, x);
	NPF_LOG(4, F("Data sent:\r\n"));
	NPF_LOG_DAT(4, data, x, true);
}

#endif