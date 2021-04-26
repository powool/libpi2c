#include "tca9548a.hpp"

static auto dummy = tca9548a::register_device();

tca9548a::tca9548a(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
}
