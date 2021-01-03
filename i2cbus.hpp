#ifndef _LIBI2C_
#define _LIBI2C_

#include <bcm2835.h>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

struct device_entry {
	uint8_t		address;
	const char	*name;
	device_entry(uint8_t address_, const char *name_) : address(address_), name(name_) {;}
};


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
	static	std::vector<device_entry> registered_devices;

	static	std::shared_ptr<bcm2835>	p;
	std::shared_ptr<bcm2835>	bcm;
	std::mutex	mutex;
public:
	i2cBus(std::shared_ptr<bcm2835> bcm_);

	static bool register_device(uint8_t address, const char *name);
	static void list_devices();
	void list_connected_devices();

	std::vector<device_entry *> probe();

	void write(uint8_t targetAddress, const uint8_t ch);

	void write(uint8_t targetAddress, const uint8_t *buf, uint32_t len);

	void read(uint8_t targetAddress, uint8_t *buf);

	void read(uint8_t targetAddress, uint8_t *buf, uint32_t len);

	void readRegisterWithRestart(uint8_t targetAddress, uint8_t register_, uint8_t *buf, uint32_t len);

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
