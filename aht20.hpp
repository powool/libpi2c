#ifndef _AHT20_
#define _AHT20_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

class aht20 {
	const uint8_t AHTX0_I2CADDR_DEFAULT = 0x38;		// AHT default i2c address
	const uint8_t AHTX0_CMD_CALIBRATE = 0xE1;		// Calibration command
	const uint8_t AHTX0_CMD_TRIGGER = 0xAC;			// Trigger reading command
	const uint8_t AHTX0_CMD_SOFTRESET = 0xBA;		// Soft reset command
	const uint8_t AHTX0_STATUS_BUSY = 0x80;			// Status bit for busy
	const uint8_t AHTX0_STATUS_CALIBRATED = 0x08;	// Status bit for calibrated
	const uint16_t sensorID = 0x1020;

	std::shared_ptr<i2cBus> bus;
	float temperature;
	float humidity;

	void delay(uint32_t milliSeconds);

	// must have called bus->setTarget first
	uint8_t getStatus(void);

	public:

	aht20(std::shared_ptr<i2cBus> bus_);

	void readSensor(void);

	float getTemperature();

	float getHumidity();
};

#endif
