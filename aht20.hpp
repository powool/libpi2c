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

	void delay(uint32_t milliSeconds) {
		usleep(milliSeconds * 1000);
	}

	// must have called bus->setTarget first
	uint8_t getStatus(void) {
		uint8_t ret;
		bus->read(&ret);
		return ret;
	}

	public:

	aht20(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
		uint8_t softReset[] = { AHTX0_CMD_SOFTRESET };
		uint8_t calibrate[] = { AHTX0_CMD_CALIBRATE, 0x08, 0x00 };

		bus->setTarget(AHTX0_I2CADDR_DEFAULT);
		bus->write(softReset, sizeof(softReset));

		delay(20);

		bus->write(calibrate, sizeof(calibrate));

		while (getStatus() & AHTX0_STATUS_BUSY) {
			delay(10);
		}
		if (!(getStatus() & AHTX0_STATUS_CALIBRATED)) {
			throw;
		}
	}

	void readSensor(void) {
		// read the data and store it!
		uint8_t cmd[] = {AHTX0_CMD_TRIGGER, 0x33, 0};

		bus->setTarget(AHTX0_I2CADDR_DEFAULT);
		bus->write(cmd, sizeof(cmd));

		while (getStatus() & AHTX0_STATUS_BUSY) {
			delay(10);
		}

		uint8_t data[6];
		bus->read(data, 6);

		uint32_t h = data[1];
		h <<= 8;
		h |= data[2];
		h <<= 4;
		h |= data[3] >> 4;
		humidity = ((float)h * 100) / 0x100000;

		uint32_t tdata = data[3] & 0x0F;
		tdata <<= 8;
		tdata |= data[4];
		tdata <<= 8;
		tdata |= data[5];
		temperature = ((float)tdata * 200 / 0x100000) - 50;
	}

	float getTemperature() {
		return temperature;
	}

	float getHumidity() {
		return humidity;
	}
};

#endif
