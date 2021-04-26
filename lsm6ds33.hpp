#ifndef _LSM6DS33_
#define _LSM6DS33_

#include <exception>
#include <memory>
#include <string>
#include <unistd.h>
#include "i2cbus.hpp"

// datasheet: https://www.st.com/resource/en/datasheet/lsm6ds33.pdf

class lsm6ds33 {
	static const uint8_t I2CADDR_DEFAULT = 0x6a;		//
	static const uint8_t LSM6DS33_CHIP_ID = 0x69;		//
	static const uint8_t LSM6DS_FUNC_CFG_ACCESS = 0x1; ///< Enable embedded functions register
	static const uint8_t LSM6DS_INT1_CTRL = 0x0D;      ///< Interrupt control for INT 1
	static const uint8_t LSM6DS_INT2_CTRL = 0x0E;      ///< Interrupt control for INT 2
	static const uint8_t LSM6DS_WHOAMI = 0xF;          ///< Chip ID register
	static const uint8_t LSM6DS_CTRL1_XL = 0x10;       ///< Main accelerometer config register
	static const uint8_t LSM6DS_CTRL2_G = 0x11;        ///< Main gyro config register
	static const uint8_t LSM6DS_CTRL3_C = 0x12;        ///< Main configuration register
	static const uint8_t LSM6DS_CTRL8_XL = 0x17;       ///< High and low pass for accel
	static const uint8_t LSM6DS_CTRL10_C = 0x19;       ///< Main configuration register
	static const uint8_t LSM6DS_WAKEUP_SRC = 0x1B;     ///< Why we woke up
	static const uint8_t LSM6DS_OUT_TEMP_L = 0x20;     ///< First data register (temperature low)
	static const uint8_t LSM6DS_OUTX_L_G = 0x22;       ///< First gyro data register
	static const uint8_t LSM6DS_OUTX_L_A = 0x28;       ///< First accel data register
	static const uint8_t LSM6DS_STEPCOUNTER = 0x4B;    ///< 16-bit step counter
	static const uint8_t LSM6DS_TAP_CFG = 0x58;        ///< Tap/pedometer configuration
	static const uint8_t LSM6DS_WAKEUP_THS = 0x5B;     ///< Single and double-tap function threshold register
	static const uint8_t LSM6DS_WAKEUP_DUR = 0x5C; ///< Free-fall, wakeup, timestamp and sleep mode duration
	static const uint8_t LSM6DS_MD1_CFG = 0x5E; ///< Functions routing on INT1 register

	std::shared_ptr<i2cBus> bus;

	int16_t rawAccX, ///< Last reading's raw accelerometer X axis
		rawAccY,     ///< Last reading's raw accelerometer Y axis
		rawAccZ,     ///< Last reading's raw accelerometer Z axis
		rawTemp,     ///< Last reading's raw temperature reading
		rawGyroX,    ///< Last reading's raw gyro X axis
		rawGyroY,    ///< Last reading's raw gyro Y axis
		rawGyroZ;    ///< Last reading's raw gyro Z axis

	void reset();
public:
	float temperature, ///< Last reading's temperature (C)
		accX,          ///< Last reading's accelerometer X axis m/s^2
		accY,          ///< Last reading's accelerometer Y axis m/s^2
		accZ,          ///< Last reading's accelerometer Z axis m/s^2
		gyroX,         ///< Last reading's gyro X axis in rad/s
		gyroY,         ///< Last reading's gyro Y axis in rad/s
		gyroZ;         ///< Last reading's gyro Z axis in rad/s


	static bool register_device() {
		return i2cBus::register_device(I2CADDR_DEFAULT, "lsm6ds33", "accelerometer");
	}

	lsm6ds33(std::shared_ptr<i2cBus> bus_);

	/** The accelerometer data rate */
	typedef enum data_rate {
		LSM6DS_RATE_SHUTDOWN,
		LSM6DS_RATE_12_5_HZ,
		LSM6DS_RATE_26_HZ,
		LSM6DS_RATE_52_HZ,
		LSM6DS_RATE_104_HZ,
		LSM6DS_RATE_208_HZ,
		LSM6DS_RATE_416_HZ,
		LSM6DS_RATE_833_HZ,
		LSM6DS_RATE_1_66K_HZ,
		LSM6DS_RATE_3_33K_HZ,
		LSM6DS_RATE_6_66K_HZ,
	} lsm6ds_data_rate_t;

	/** The accelerometer data range */
	typedef enum accel_range {
		LSM6DS_ACCEL_RANGE_2_G,
		LSM6DS_ACCEL_RANGE_16_G,
		LSM6DS_ACCEL_RANGE_4_G,
		LSM6DS_ACCEL_RANGE_8_G
	} lsm6ds_accel_range_t;

	/** The gyro data range */
	typedef enum gyro_range {
		LSM6DS_GYRO_RANGE_125_DPS = 0b0010,
		LSM6DS_GYRO_RANGE_250_DPS = 0b0000,
		LSM6DS_GYRO_RANGE_500_DPS = 0b0100,
		LSM6DS_GYRO_RANGE_1000_DPS = 0b1000,
		LSM6DS_GYRO_RANGE_2000_DPS = 0b1100,
		ISM330DHCX_GYRO_RANGE_4000_DPS = 0b0001
	} lsm6ds_gyro_range_t;

	/** The high pass filter bandwidth */
	typedef enum hpf_range {
		LSM6DS_HPF_ODR_DIV_50 = 0,
		LSM6DS_HPF_ODR_DIV_100 = 1,
		LSM6DS_HPF_ODR_DIV_9 = 2,
		LSM6DS_HPF_ODR_DIV_400 = 3,
	} lsm6ds_hp_filter_t;

	lsm6ds_data_rate_t getAccelDataRate(void);
	void setAccelDataRate(lsm6ds_data_rate_t data_rate);

	lsm6ds_data_rate_t getGyroDataRate(void);
	void setGyroDataRate(lsm6ds_data_rate_t data_rate);

	void read();

	lsm6ds_gyro_range_t getGyroRange(void);
	void setGyroRange(lsm6ds_gyro_range_t new_range);

	lsm6ds_accel_range_t getAccelRange(void);
	void setAccelRange(lsm6ds_accel_range_t new_range);

#if 0
public:
  bool getEvent(sensors_event_t *accel, sensors_event_t *gyro,
                sensors_event_t *temp);


  void reset(void);
  void configIntOutputs(bool active_low, bool open_drain);
  void configInt1(bool drdy_temp, bool drdy_g, bool drdy_xl,
                  bool step_detect = false, bool wakeup = false);
  void configInt2(bool drdy_temp, bool drdy_g, bool drdy_xl);
  void highPassFilter(bool enabled, lsm6ds_hp_filter_t filter);

  void enableWakeup(bool enable, uint8_t duration, uint8_t thresh);
  bool awake(void);

  void enablePedometer(bool enable);
  void resetPedometer(void);
  uint16_t readPedometer(void);
protected:
  uint8_t chipID();
  virtual void _read(void);
  virtual bool _init(int32_t sensor_id);

  uint16_t _sensorid_accel, ///< ID number for accelerometer
      _sensorid_gyro,       ///< ID number for gyro
      _sensorid_temp;       ///< ID number for temperature

private:

  void fillTempEvent(sensors_event_t *temp, uint32_t timestamp);
  void fillAccelEvent(sensors_event_t *accel, uint32_t timestamp);
  void fillGyroEvent(sensors_event_t *gyro, uint32_t timestamp);
#endif

};

#endif
