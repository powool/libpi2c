#ifndef _DS3502_H
#define _DS3502_H

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"


class ds3502 {
	static const uint8_t I2CADDR_DEFAULT = 0x48;
	static const uint8_t CONVERSION  = 0x00;
	static const uint8_t CONFIG  = 0x01;
	static const uint8_t LO_THRESH  = 0x02;
	static const uint8_t HI_THRESH  = 0x03;

	static const uint16_t OS  = 0x8000;

	// see other mux options in https://cdn-shop.adafruit.com/datasheets/ads1015.pdf
	static const uint16_t AIN0_TO_GND  = 0b100 << 9;
	static const uint16_t AIN1_TO_GND  = 0b101 << 9;
	static const uint16_t AIN2_TO_GND  = 0b110 << 9;
	static const uint16_t AIN3_TO_GND  = 0b111 << 9;

	static const uint16_t FS_6144V  = 0b000 << 9;
	static const uint16_t FS_4096V  = 0b001 << 9;
	static const uint16_t FS_2048V  = 0b010 << 9;
	static const uint16_t FS_1024V  = 0b011 << 9;
	static const uint16_t FS_512V  = 0b100 << 9;
	static const uint16_t FS_256V  = 0b101 << 9;

	static const uint16_t CONTINUOUS  = 0 << 8;
	static const uint16_t POWER_DOWN  = 1 << 8;

	static const uint16_t DATA_RATE_128SPS  = 0b000 << 5;
	static const uint16_t DATA_RATE_250SPS  = 0b001 << 5;
	static const uint16_t DATA_RATE_490SPS  = 0b010 << 5;
	static const uint16_t DATA_RATE_920SPS  = 0b011 << 5;
	static const uint16_t DATA_RATE_1600SPS  = 0b100 << 5;
	static const uint16_t DATA_RATE_2400SPS  = 0b101 << 5;
	static const uint16_t DATA_RATE_3300SPS  = 0b110 << 5;

	static const uint16_t COMPARATOR_TRADITIONAL  = 0b0 << 4;
	static const uint16_t COMPARATOR_WINDOW  = 0b1 << 4;

	static const uint16_t COMPARATOR_POLATIRY_ACTIVE_LOW  = 0b0 << 3;
	static const uint16_t COMPARATOR_POLATIRY_ACTIVE_HIGH  = 0b1 << 3;

	static const uint16_t COMPARATOR_NON_LATCHING  = 0b0 << 3;
	static const uint16_t COMPARATOR_LATCHING  = 0b1 << 3;

	static const uint16_t COMPARATOR_ASSERT_ONE  = 0b00 << 0;
	static const uint16_t COMPARATOR_ASSERT_TWO  = 0b01 << 0;
	static const uint16_t COMPARATOR_ASSERT_FOUR  = 0b10 << 0;
	static const uint16_t COMPARATOR_DISABLE  = 0b11 << 0;

	std::shared_ptr<i2cBus> bus;
public:
	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "ads1015", "4 channel ADC");
	}
	ads1015(std::shared_ptr<i2cBus> bus_);

	float get(void);
	void setMux(int channel);
};

#endif
