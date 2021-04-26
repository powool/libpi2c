#ifndef _MPL3115A2_
#define _MPL3115A2_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

class mpl3115a2 {
	static const uint8_t I2CADDR_DEFAULT = 0x60;

	std::shared_ptr<i2cBus> bus;

	public:

	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "mpl3115a2");
	}

	mpl3115a2(std::shared_ptr<i2cBus> bus_);
};

#endif
