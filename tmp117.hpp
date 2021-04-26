#ifndef _TMP_117_
#define _TMP_117_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

class tmp117 {
	static const uint8_t I2CADDR_DEFAULT = 0x48;
	static const uint8_t TEMP_RESULT = 0x00;
	static const uint8_t CONFIGURATION = 0x01;
	static const uint8_t THIGH_LIMIT = 0x02;
	static const uint8_t TLOW_LIMIT = 0x03;
	static const uint8_t EEPROM_UL = 0x04;
	static const uint8_t EEPROM1 = 0x05;
	static const uint8_t EEPROM2 = 0x06;
	static const uint8_t TEMP_OFFSET = 0x07;
	static const uint8_t EEPROM3 = 0x08;
	static const uint8_t DEVICE_ID = 0x0f;
	static const uint16_t DEVICE_ID_VALUE = 0x117;

	std::shared_ptr<i2cBus> bus;

	public:

	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "tmp117", "temperature sensor");
	}

	tmp117(std::shared_ptr<i2cBus> bus_);
	float readTemperature();
};

#endif
