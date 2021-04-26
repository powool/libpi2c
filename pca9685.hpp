#ifndef _PCA9685_
#define _PCA9685_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

class pca9685 {
	static const uint8_t I2CADDR_DEFAULT = 0x40;

// REGISTER ADDRESSES
	static const uint8_t PCA9685_MODE1 = 0x00;      /**< Mode Register 1 */
	static const uint8_t PCA9685_MODE2 = 0x01;      /**< Mode Register 2 */
	static const uint8_t PCA9685_SUBADR1 = 0x02;    /**< I2C-bus subaddress 1 */
	static const uint8_t PCA9685_SUBADR2 = 0x03;    /**< I2C-bus subaddress 2 */
	static const uint8_t PCA9685_SUBADR3 = 0x04;    /**< I2C-bus subaddress 3 */
	static const uint8_t PCA9685_ALLCALLADR = 0x05; /**< LED All Call I2C-bus address */
	static const uint8_t PCA9685_LED0_ON_L = 0x06;  /**< LED0 on tick, low byte*/
	static const uint8_t PCA9685_LED0_ON_H = 0x07;  /**< LED0 on tick, high byte*/
	static const uint8_t PCA9685_LED0_OFF_L = 0x08; /**< LED0 off tick, low byte */
	static const uint8_t PCA9685_LED0_OFF_H = 0x09; /**< LED0 off tick, high byte */
// etc all 16:  LED15_OFF_H 0x45
	static const uint8_t PCA9685_ALLLED_ON_L = 0xFA;  /**< load all the LEDn_ON registers, low */
	static const uint8_t PCA9685_ALLLED_ON_H = 0xFB;  /**< load all the LEDn_ON registers, high */
	static const uint8_t PCA9685_ALLLED_OFF_L = 0xFC; /**< load all the LEDn_OFF registers, low */
	static const uint8_t PCA9685_ALLLED_OFF_H = 0xFD; /**< load all the LEDn_OFF registers,high */
	static const uint8_t PCA9685_PRESCALE = 0xFE;     /**< Prescaler for PWM output frequency */
	static const uint8_t PCA9685_TESTMODE = 0xFF;     /**< defines the test mode to be entered */

// MODE1 bits
	static const uint8_t MODE1_ALLCAL = 0x01;  /**< respond to LED All Call I2C-bus address */
	static const uint8_t MODE1_SUB3 = 0x02;    /**< respond to I2C-bus subaddress 3 */
	static const uint8_t MODE1_SUB2 = 0x04;    /**< respond to I2C-bus subaddress 2 */
	static const uint8_t MODE1_SUB1 = 0x08;    /**< respond to I2C-bus subaddress 1 */
	static const uint8_t MODE1_SLEEP = 0x10;   /**< Low power mode. Oscillator off */
	static const uint8_t MODE1_AI = 0x20;      /**< Auto-Increment enabled */
	static const uint8_t MODE1_EXTCLK = 0x40;  /**< Use EXTCLK pin clock */
	static const uint8_t MODE1_RESTART = 0x80; /**< Restart enabled */
// MODE2 bits
	static const uint8_t MODE2_OUTNE_0 = 0x01; /**< Active LOW output enable input */
	static const uint8_t MODE2_OUTNE_1 = 0x02; /**< Active LOW output enable input - high impedience */
	static const uint8_t MODE2_OUTDRV = 0x04; /**< totem pole structure vs open-drain */
	static const uint8_t MODE2_OCH = 0x08;    /**< Outputs change on ACK vs STOP */
	static const uint8_t MODE2_INVRT = 0x10;  /**< Output logic state inverted */

	static const uint8_t PCA9685_I2C_ADDRESS = 0x40;      /**< Default PCA9685 I2C Slave Address */
	static const uint32_t FREQUENCY_OSCILLATOR = 25000000; /**< Int. osc. frequency in datasheet */

	static const uint8_t PCA9685_PRESCALE_MIN = 3;   /**< minimum prescale value */
	static const uint8_t PCA9685_PRESCALE_MAX = 255; /**< maximum prescale value */

	std::shared_ptr<i2cBus> bus;

	uint32_t _oscillator_freq;

	public:

	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "pca9685");
	}

	pca9685(std::shared_ptr<i2cBus> bus_);

	void setPWM(uint8_t num, uint16_t on, uint16_t off);
	void setPWMFreq(float freq);
	void setOscillatorFrequency(uint32_t freq);

};

#endif
