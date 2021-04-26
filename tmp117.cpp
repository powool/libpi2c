#include "tmp117.hpp"

static auto dummy = tmp117::register_device();

tmp117::tmp117(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
	if(bus->readRegister16(I2CADDR_DEFAULT, DEVICE_ID) != DEVICE_ID_VALUE)
		throw i2cException("tmp117 failed to get expected device ID value of 0x117");
}

float tmp117::readTemperature() {
	// get a 15-bit fixed point fraction
	uint16_t temp = bus->readRegister16(I2CADDR_DEFAULT, TEMP_RESULT);

	return static_cast<int16_t>(temp) / 32768.0;
}
