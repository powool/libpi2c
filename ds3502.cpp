#include "ds3502.hpp"

static auto dummy = ds3502::register_device();

ds3502::ds3502(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
	bus->writeRegister8(I2CADDR_DEFAULT, CONTROL, MODE1);
}

void ds3502::setWiper(uint8_t wiper) {
	bus->writeRegister8(I2CADDR_DEFAULT, WIPER, wiper);
}

void ds3502::setWiperDefault(uint8_t wiper) {
	auto currentWiper = getWiper();
	bus->writeRegister8(I2CADDR_DEFAULT, CONTROL, MODE0);
	bus->writeRegister8(I2CADDR_DEFAULT, WIPER, wiper);
	bus->writeRegister8(I2CADDR_DEFAULT, CONTROL, MODE1);
	bus->writeRegister8(I2CADDR_DEFAULT, WIPER, currentWiper);
}

uint8_t ds3502::getWiper() {
	auto wiper = bus->readRegister8(I2CADDR_DEFAULT, WIPER);
	return wiper;
}
