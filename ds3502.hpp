#ifndef _DS3502_H
#define _DS3502_H

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"


class ds3502 {
	static const uint8_t I2CADDR_DEFAULT = 0x28;
	static const uint8_t WIPER  = 0x00;
	static const uint8_t CONTROL = 0x02;
	static const uint8_t MODE0 = 0x00;
	static const uint8_t MODE1 = 0x80;

	std::shared_ptr<i2cBus> bus;
public:
	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "ds3502", "digital potentiometer");
	}
	ds3502(std::shared_ptr<i2cBus> bus_);

	uint8_t getWiper(void);
	void setWiper(uint8_t new_wiper_value);
	void setWiperDefault(uint8_t new_wiper_default);
};

#endif
