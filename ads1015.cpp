#include "ads1015.hpp"

static auto dummy = ads1015::register_device();

ads1015::ads1015(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
	auto config = | AIN0_TO_GND | FS_6144V | CONTINUOUS
	bus->writeRegister16(I2CADDR_DEFAULT, CONFIG, MODE1);
}

float ads1015::get(void) {
	bus->readRegister16(I2CADDR_DEFAULT, WIPER, wiper);
}
