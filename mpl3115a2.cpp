#include "mpl3115a2.hpp"

static auto dummy = mpl3115a2::register_device();

mpl3115a2::mpl3115a2(std::shared_ptr<i2cBus> bus_) : bus(bus_) {
}
