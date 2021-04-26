#include "ds3502.hpp"

static auto dummy = ds3502::register_device();

ds3502::ds3502(std::shared_ptr<i2cBus> bus_, int deviceIndex_) : bus(bus_), deviceIndex(deviceIndex_) {
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, CONTROL, MODE1);
}

void ds3502::setWiper(uint8_t wiper) {
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, WIPER, wiper);
}

void ds3502::setWiperDefault(uint8_t wiper) {
	auto currentWiper = getWiper();
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, CONTROL, MODE0);
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, WIPER, wiper);
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, CONTROL, MODE1);
	bus->writeRegister8(I2CADDR_DEFAULT + deviceIndex, WIPER, currentWiper);
}

uint8_t ds3502::getWiper() {
	auto wiper = bus->readRegister8(I2CADDR_DEFAULT + deviceIndex, WIPER);
	return wiper;
}
