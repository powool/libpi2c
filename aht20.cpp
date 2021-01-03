#include "aht20.hpp"

static auto dummy = aht20::register_device();

void aht20::delay(uint32_t milliSeconds) {
	usleep(milliSeconds * 1000);
}

uint8_t aht20::getStatus(void) {
	uint8_t ret;
	bus->read(AHTX0_I2CADDR_DEFAULT, &ret);
	return ret;
}

aht20::aht20(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
	uint8_t softReset[] = { AHTX0_CMD_SOFTRESET };
	uint8_t calibrate[] = { AHTX0_CMD_CALIBRATE, 0x08, 0x00 };

	bus->write(AHTX0_I2CADDR_DEFAULT, softReset, sizeof(softReset));

	delay(20);

	bus->write(AHTX0_I2CADDR_DEFAULT, calibrate, sizeof(calibrate));

	while (getStatus() & AHTX0_STATUS_BUSY) {
		delay(10);
	}
	if (!(getStatus() & AHTX0_STATUS_CALIBRATED)) {
		throw;
	}
}

void aht20::readSensor(void) {
	// read the data and store it!
	uint8_t cmd[] = {AHTX0_CMD_TRIGGER, 0x33, 0};

	bus->write(AHTX0_I2CADDR_DEFAULT, cmd, sizeof(cmd));

	while (getStatus() & AHTX0_STATUS_BUSY) {
		delay(10);
	}

	uint8_t data[6];
	bus->read(AHTX0_I2CADDR_DEFAULT, data, 6);

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

float aht20::getTemperature() {
	return temperature;
}

float aht20::getHumidity() {
	return humidity;
}
