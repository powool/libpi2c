#include "i2cbus.hpp"
#include <iostream>

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

bool i2cBus::register_device(uint8_t address, const char *name)
{
	registered_devices.push_back(device_entry(address, name));
	return true;
}

void i2cBus::list_devices()
{
	for(auto const &dv: registered_devices) {
		std::cout << "device " << dv.name << " at address " << int(dv.address) << std::endl;
	}
}

void i2cBus::list_connected_devices()
{
	for(auto const &dv: registered_devices) {
		if(connected(dv.address))
			std::cout << "device " << dv.name << " is connected at address " << int(dv.address) << std::endl;
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
	if(bcm2835_i2c_write(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_write failed");
}

void i2cBus::read(uint8_t targetAddress, uint8_t *buf) {
	read(targetAddress, buf, 1);
}

void i2cBus::read(uint8_t targetAddress, uint8_t *buf, uint32_t len) {
	const std::lock_guard<std::mutex> lock(mutex);
	bcm2835_i2c_setSlaveAddress(targetAddress);
	if(bcm2835_i2c_read(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_write failed");
}

void i2cBus::readRegisterWithRestart(uint8_t targetAddress, uint8_t register_, uint8_t *buf, uint32_t len) {
	const std::lock_guard<std::mutex> lock(mutex);
	bcm2835_i2c_setSlaveAddress(targetAddress);
	auto rc = bcm2835_i2c_read_register_rs(reinterpret_cast<char *>(&register_), reinterpret_cast<char *>(buf), len);
	if(rc != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_read_register_rs failed");
}

bool i2cBus::connected(uint8_t targetAddress) {
	try {
		uint8_t ch;    // unused
		write(targetAddress, &ch, 0);
	} catch(std::exception e) {
		return false;
	}
	return true;
}
