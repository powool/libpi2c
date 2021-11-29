# libpi2c - Powool's I2C library for the Rapsberry Pi

## What is libpi2c

libpi2c is a small, C++ thread and exception safe set of core files and drivers for
a number of I2C devices from Adafruit.

There is a main() in mustang.cpp, which demonstrates my goals by implementing a thread safe,
exception safe, self-healing program to monitor engine temperature and control
both a fan and a water pump via external PWM controllers.

## Why libpi2c

Mostly, this is a chance for me to refresh my C++ skills. I've done a lot of concurrent
C++ programming, and decided to combine this into a project to run my cooling system on
a 1965 Mustang that I'm using cheap PWM controllers to control the electric fan and water pump with.

Most previous work I've done was using older boost libraries, but as newer versions
of C++ have come along, they've included more and better primitives for concurrency,
which I wanted to explore here.

I've also wanted to try adapting "my view" of what a microcontroller might look like in
modern C++, using threads and exceptions to improve readability, separation of concerns,
testing, and self-healing in flaky I2C circuits.


## What is included

The core file is i2cbus.cpp, which provides thread safe access to the i2c bus.
If a function returns, it worked.
If an exception was thrown, it did not.

All devices are designed to accept a shared pointer to a single global i2cbus. All
communications occur through the i2cbus object. Eventually, I will add support for
other i2c busses, hopefully including "bit bang" to digital I/O pins.

The list of devices is not long, but includes I2C devices from adafruit and sparkfun.

* ads1015 - 12 bit precision analog to digitial converter [Adafruit](https://www.adafruit.com/product/1083)
* aht20 - temperature and humidity sensor [Adafruit](https://www.adafruit.com/product/4566)
* ds3502 - digital 10K potentiometer [Adafruit](https://www.adafruit.com/product/4286)
* lsm6ds33 - 9 degrees of freedom IMU with Accel/Gyro/ Mag [Adafruit](https://www.adafruit.com/product/4485)
* mpl3115a2 - barometric pressure/temperature [Adafruit](https://www.adafruit.com/product/1893)
* pca9685 - 16 channel PWM/servo interface [Adafruit](https://www.adafruit.com/product/815)
* ssd1306 - 128x32 OLED [Adafruit](https://www.adafruit.com/product/4650)
* tca9548a - I2c Multiplexer [Adafruit](https://www.adafruit.com/product/2717)
* tmp117 - high accuracy temperature sensor [Adafruit](https://www.adafruit.com/product/4821)
* vcnl4040 - proximity and Lux sensor [Adafruit](https://www.adafruit.com/product/4161)

## Rationale

This repository is an experiment in demonstrating thoughts I've had about 
microcontroller type devices - how they are implemented goes a long way towards
determining how reusable the engineering is, how robust they are, and how much
fun they are to work on.

### C++ I2C subgoal: proper memory use

Use RAII contruction, smart pointers to manage lifetime, and proper destruction, such
that the sytem can be either "uncleanly" terminated via signal, or cleanly shut down,
and yet maintain memory coherency.

### C++ I2C subgoal: signals

External, asychronous events are important to the life of a microcontroller. Though
done in Debian/Ubuntu user space, I want this to grow to be able to properly handle
asychronous events.

In the included code, the signal is crude - simply handle a SIGTERM signal, but do
so in such a way that all threads exit cleanly with no memory leaks or hangs.

### C++ I2C subgoal: exceptions

In my breadboarding experience, it is common for I2C devices to drop offline for various
reasons - noise on the power lines, jostling connectors, and so on.

A thread managing a single device should not die - there must be a sane, easy to read
retry/reset/reconnect mechanism, short of a power cycle.

Ideally, any I2C device should be allowed to be removed from the system, and all devices
that don't depend on it will continue functioning as expected.

This is a goal, and mostly one to experiment with this combination of C++ exceptions
with self-healing.

### readability subgoal: 

I'm quite frustrated reading other code libraries that mix concerns between different
layers of drivers, and end up with a mess of different ways of detecting errors and
acting on them. As things get more complicated, it gets harder and harder to ensure that
exceptional conditions are being dealt with in a correct fashion, thus making it ever more
important to make core business logic logic of an application aware of, and very clearly
able to deal with, exceptional conditions.

Doing this well is hard, but proper abstraction, consistent idiomatic forms of error
detection and handling should work to make a more robust, safer to use, device.

### design and implementation subgoal: 

In general, I feel code that is isolated to simple areas of concern is easier to maintain,
easier to test, and easier to "prove" that it is correct. This is obviously impossible to truly
achieve, but every reasonable effor should be made to do so, and is especially fitting
for a concurrent implementation like this.

### reuse subgoal:

Avoid copy/paste where possible. Any instance of common code is getting pushed down a layer
so that it can be shared by other drivers if possible.
