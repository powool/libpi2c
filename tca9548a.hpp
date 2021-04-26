#ifndef _TCA9548A_
#define _TCA9548A_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

class tca9548a {
	static const uint8_t I2CADDR_DEFAULT = 0x70;

	std::shared_ptr<i2cBus> bus;

	public:

	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "tca9548a", "i2c mux");
	}

	tca9548a(std::shared_ptr<i2cBus> bus_);
};

#endif
