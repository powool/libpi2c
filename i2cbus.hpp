#ifndef _LIBI2C_
#define _LIBI2C_

#include <bcm2835.h>
#include <exception>
#include <memory>
#include <string>

class i2cException : std::exception {
private:
	std::string	msg;
public:
	i2cException(const std::string &e);
	const char* what();
};

class bcm2835 {
public:
	bcm2835();
	~bcm2835();
};

class i2cBus {
	static std::shared_ptr<bcm2835>	p;
	std::shared_ptr<bcm2835>	bcm;
public:
	i2cBus(std::shared_ptr<bcm2835> bcm_);

	void setTarget(uint8_t targetAddress);

	void write(const uint8_t ch);

	void write(const uint8_t *buf, uint32_t len);

	void read(uint8_t *buf);

	void read(uint8_t *buf, uint32_t len);

	void readRegisterWithRestart(uint8_t register_, uint8_t *buf, uint32_t len);

	bool connected(uint8_t targetAddress);

	// XXX implement writeRestart
	// XXX implement readRestart
};

class i2cBusExtender {
	i2cBus &i2cbus;		// ???
	uint8_t	targetAddress;
public:
	i2cBusExtender(i2cBus &i2cbus_) : i2cbus(i2cbus_) {
	}
};

#endif
