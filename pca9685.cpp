#include "pca9685.hpp"

static auto dummy = pca9685::register_device();

pca9685::pca9685(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
}

void pca9685::setPWM(uint8_t num, uint16_t on, uint16_t off)
{
}

void pca9685::setPWMFreq(float freq)
{
}

void pca9685::setOscillatorFrequency(uint32_t freq)
{
}

