#include "lsm6ds33.hpp"

static auto dummy = lsm6ds33::register_device();

lsm6ds33::lsm6ds33(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
	// make sure we're talking to the right chip
	if (bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_WHOAMI) != LSM6DS33_CHIP_ID) {
		throw i2cException("incorrect chip ID for lsm6ds33");
	}
	reset();
	if (bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_WHOAMI) != LSM6DS33_CHIP_ID) {
		throw i2cException("incorrect chip ID for lsm6ds33");
	}

	// enable accelerometer and gyro by setting the data rate to non-zero
	setAccelDataRate(LSM6DS_RATE_104_HZ);
	setGyroDataRate(LSM6DS_RATE_104_HZ);

	usleep(10000);
}

void lsm6ds33::reset() {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL3_C);
	reg |= 0x80;
	bus->writeRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL3_C, reg);

	while(bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL3_C) & 0x80) {
		usleep(1000);
	}
}

void lsm6ds33::read() {
	uint8_t	buffer[14];

	bus->readRegisterWithRestart(I2CADDR_DEFAULT, LSM6DS_OUT_TEMP_L, buffer, 14);

	rawTemp = buffer[1] << 8 | buffer[0];

	temperature = (rawTemp / 256.0) + 25.0;

	rawGyroX = buffer[3] << 8 | buffer[2];
	rawGyroY = buffer[5] << 8 | buffer[4];
	rawGyroZ = buffer[7] << 8 | buffer[6];

	rawAccX = buffer[9] << 8 | buffer[8];
	rawAccY = buffer[11] << 8 | buffer[10];
	rawAccZ = buffer[13] << 8 | buffer[12];

	lsm6ds_gyro_range_t gyro_range = getGyroRange();
	float gyro_scale = 1; // range is in milli-dps per bit!
	if (gyro_range == ISM330DHCX_GYRO_RANGE_4000_DPS)
		gyro_scale = 140.0;
	if (gyro_range == LSM6DS_GYRO_RANGE_2000_DPS)
		gyro_scale = 70.0;
	if (gyro_range == LSM6DS_GYRO_RANGE_1000_DPS)
		gyro_scale = 35.0;
	if (gyro_range == LSM6DS_GYRO_RANGE_500_DPS)
		gyro_scale = 17.50;
	if (gyro_range == LSM6DS_GYRO_RANGE_250_DPS)
		gyro_scale = 8.75;
	if (gyro_range == LSM6DS_GYRO_RANGE_125_DPS)
		gyro_scale = 4.375;

#define SENSORS_GRAVITY_STANDARD (9.80665F)
#define SENSORS_DPS_TO_RADS (0.017453293F)

	gyroX = rawGyroX * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0;
	gyroY = rawGyroY * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0;
	gyroZ = rawGyroZ * gyro_scale * SENSORS_DPS_TO_RADS / 1000.0;

	lsm6ds_accel_range_t accel_range = getAccelRange();
	float accel_scale = 1.0; // range is in milli-g per bit!
	if (accel_range == LSM6DS_ACCEL_RANGE_16_G)
		accel_scale = 0.488;
	if (accel_range == LSM6DS_ACCEL_RANGE_8_G)
		accel_scale = 0.244;
	if (accel_range == LSM6DS_ACCEL_RANGE_4_G)
		accel_scale = 0.122;
	if (accel_range == LSM6DS_ACCEL_RANGE_2_G)
		accel_scale = 0.061;

	accX = rawAccX * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
	accY = rawAccY * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
	accZ = rawAccZ * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
}

void lsm6ds33::setAccelDataRate(lsm6ds_data_rate_t data_rate) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL1_XL) & 0x0f;

	reg |= static_cast<uint8_t>(data_rate) << 4;

	bus->writeRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL1_XL, reg);
}

lsm6ds33::lsm6ds_data_rate_t lsm6ds33::getAccelDataRate(void) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL1_XL);

	return static_cast<lsm6ds_data_rate_t>(reg >> 4);
}

void lsm6ds33::setGyroDataRate(lsm6ds_data_rate_t data_rate) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL2_G) & 0x0f;

	reg |= static_cast<uint8_t>(data_rate) << 4;

	bus->writeRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL2_G, reg);
}

lsm6ds33::lsm6ds_data_rate_t lsm6ds33::getGyroDataRate(void) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL2_G);

	return static_cast<lsm6ds_data_rate_t>(reg >> 4);
}

lsm6ds33::lsm6ds_gyro_range_t lsm6ds33::getGyroRange(void) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL2_G) & 0xf0;;

	return static_cast<lsm6ds_gyro_range_t>(reg >> 2);
}

lsm6ds33::lsm6ds_accel_range_t lsm6ds33::getAccelRange(void) {
	auto reg = bus->readRegister8(I2CADDR_DEFAULT, LSM6DS_CTRL1_XL) & 0xf0;;

	return static_cast<lsm6ds_accel_range_t>(reg >> 2);
}

#if 0

lsm6ds_data_rate_t Adafruit_LSM6DS::getAccelDataRate(void) {

  Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LSM6DS_CTRL1_XL);

  Adafruit_BusIO_RegisterBits accel_data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl1, 4, 4);

  return (lsm6ds_data_rate_t)accel_data_rate.read();
}

lsm6ds_data_rate_t lsm6ds33::getGyroDataRate(void) {

  Adafruit_BusIO_Register ctrl2 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LSM6DS_CTRL2_G);

  Adafruit_BusIO_RegisterBits gyro_data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl2, 4, 4);

  return (lsm6ds_data_rate_t)gyro_data_rate.read();
}

void lsm6ds33::setGyroDataRate(lsm6ds_data_rate_t data_rate) {

  Adafruit_BusIO_Register ctrl2 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LSM6DS_CTRL2_G);

  Adafruit_BusIO_RegisterBits gyro_data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl2, 4, 4);

  gyro_data_rate.write(data_rate);
}


void lsm6ds33::setAccelRange(lsm6ds_accel_range_t new_range) {

   Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
       i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LSM6DS_CTRL1_XL);

  Adafruit_BusIO_RegisterBits accel_range =
      Adafruit_BusIO_RegisterBits(&ctrl1, 2, 2);

  accel_range.write(new_range);
  delay(20);
}

#endif
