#include "i2cbus.hpp"
#include <iostream>
#include <sstream>

std::vector<device_entry> i2cBus::registered_devices;

i2cException::i2cException(const std::string &e) {
	msg = e;
}
const char* i2cException::what() {
	return msg.c_str();
}

bcm2835::bcm2835() {
	if(!bcm2835_init())
		throw i2cException("bcm2835_init failed");
	bcm2835_i2c_begin();
	bcm2835_i2c_set_baudrate(100000);
}

bcm2835::~bcm2835() {
	bcm2835_i2c_end();
}

i2cBus::i2cBus(std::shared_ptr<bcm2835> bcm_) : bcm(bcm_) {
}

bool i2cBus::register_device(uint8_t address, const char *name, const char *description)
{
	registered_devices.push_back(device_entry(address, name, description));
	return true;
}

void i2cBus::list_devices()
{
	for(auto const &dv: registered_devices) {
		std::cout << "device driver " << dv.name << " at address " << std::hex << std::showbase << int(dv.address) << std::endl << std::dec;
	}
}

void i2cBus::list_connected_devices()
{
	for(auto const &dv: registered_devices) {
		if(connected(dv.address))
			std::cout << "device " << dv.name << " is connected at address " << std::hex << std::showbase << int(dv.address) << std::endl << std::dec;
	}
}

std::vector<device_entry *> i2cBus::probe()
{
	std::vector<device_entry *> rv;

	for(auto &entry: registered_devices) {
	}

	return std::move(rv);
}

void i2cBus::write(uint8_t targetAddress, const uint8_t ch) {
	write(targetAddress, &ch, 1);
}

void i2cBus::write(uint8_t targetAddress, const uint8_t *buf, uint32_t len) {
	const std::lock_guard<std::mutex> lock(mutex);
	bcm2835_i2c_setSlaveAddress(targetAddress);
	if(bcm2835_i2c_write(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK) {
		std::stringstream s;
		s << "i2cBus::write failed on target " << std::hex << std::showbase << (uint16_t) targetAddress << std::dec;
		throw i2cException(s.str());
	}
}

void i2cBus::read(uint8_t targetAddress, uint8_t *buf) {
	read(targetAddress, buf, 1);
}

void i2cBus::read(uint8_t targetAddress, uint8_t *buf, uint32_t len) {
	const std::lock_guard<std::mutex> lock(mutex);
	bcm2835_i2c_setSlaveAddress(targetAddress);
	if(bcm2835_i2c_read(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK) {
		std::stringstream s;
		s << "i2cBus::read failed on target " << std::hex << std::showbase << (uint16_t) targetAddress << std::dec;
		throw i2cException(s.str());
	}
}

void i2cBus::writeRegister8(uint8_t targetAddress, uint8_t register_, uint8_t data) {
	uint8_t buf[2];
	buf[0] = register_;
	buf[1] = data;
	write(targetAddress, buf, 2);
}

void i2cBus::writeRegister16(uint8_t targetAddress, uint8_t register_, uint16_t data) {
	uint8_t buf[3];
	buf[0] = register_;
	buf[1] = data >> 8;
	buf[2] = data;
	write(targetAddress, buf, 3);
}

uint8_t i2cBus::readRegister8(uint8_t targetAddress, uint8_t register_) {
	uint8_t buffer;
	readRegisterWithRestart(targetAddress, register_, &buffer, 1);
	return buffer;
}

uint16_t i2cBus::readRegister16(uint8_t targetAddress, uint8_t register_) {
	uint8_t buffer[2];
	readRegisterWithRestart(targetAddress, register_, &buffer[0], 2);
	return buffer[0] << 8 | buffer[1];
}

void i2cBus::readRegisterWithRestart(uint8_t targetAddress, uint8_t register_, uint8_t *buf, uint32_t len) {
	const std::lock_guard<std::mutex> lock(mutex);
	bcm2835_i2c_setSlaveAddress(targetAddress);
	auto rc = bcm2835_i2c_read_register_rs(reinterpret_cast<char *>(&register_), reinterpret_cast<char *>(buf), len);
	if(rc != BCM2835_I2C_REASON_OK) {
		std::stringstream s;
		s << "i2cBus::readRegisterWithRestart failed on target " << std::hex << std::showbase << (uint16_t) targetAddress << std::dec;
		throw i2cException(s.str());
	}
}

bool i2cBus::connected(uint8_t targetAddress) {
	try {
		uint8_t ch;    // unused
		write(targetAddress, &ch, 0);
	} catch(i2cException e) {
		return false;
	}
	return true;
}
