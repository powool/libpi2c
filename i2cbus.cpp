#include "i2cbus.hpp"

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

void i2cBus::setTarget(uint8_t targetAddress) {
	bcm2835_i2c_setSlaveAddress(targetAddress);
}

void i2cBus::write(const uint8_t ch) {
	write(&ch, 1);
}

void i2cBus::write(const uint8_t *buf, uint32_t len) {
	if(bcm2835_i2c_write(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_write failed");
}

void i2cBus::read(uint8_t *buf) {
	read(buf, 1);
}

void i2cBus::read(uint8_t *buf, uint32_t len) {
	if(bcm2835_i2c_read(const_cast<char *>(reinterpret_cast<const char *>(buf)), len) != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_write failed");
}

void i2cBus::readRegisterWithRestart(uint8_t register_, uint8_t *buf, uint32_t len) {
	auto rc = bcm2835_i2c_read_register_rs(reinterpret_cast<char *>(&register_), reinterpret_cast<char *>(buf), len);
	if(rc != BCM2835_I2C_REASON_OK)
		throw i2cException("bcm2835_i2c_read_register_rs failed");
}

bool i2cBus::connected(uint8_t targetAddress) {
	try {
		uint8_t ch;    // unused
		setTarget(targetAddress);
		write(&ch, 0);
	} catch(std::exception e) {
		return false;
	}
	return true;
}
