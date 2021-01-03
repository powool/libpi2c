#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <unistd.h>
#include "i2cbus.hpp"
#include "vcnl4040.hpp"
#include "aht20.hpp"

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void watch_humidity(std::shared_ptr<i2cBus> i2c)
{
	auto htSensor = std::make_shared<aht20>(i2c);
	while(true) {
		try {
			htSensor->readSensor();
			std::cout << "temperature=" << htSensor->getTemperature() << std::endl;
			std::cout << "humidity=" << htSensor->getHumidity() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		} catch (i2cException e) {
			std::cerr << "device disconnect: " << e.what() << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

void watch_proximity(std::shared_ptr<i2cBus> i2c)
{
	auto vcnl = std::make_shared<vcnl4040>(i2c);
	while(true) {
		try {
			vcnl->powerOnAmbient();
			vcnl->powerOnProximity();
			vcnl->setProxHighThreshold(10);
			vcnl->setProxLowThreshold(5);
			vcnl->enableProxLogicMode();
			while(true) {
				if (vcnl->isClose()) {
					std::cout << "Oh so close!" << std::endl;
				}
				if (vcnl->isAway()) {
					std::cout << "Oh so far!" << std::endl;
				}
				std::cout << "getProximity()=" << vcnl->getProximity() << std::endl;
				std::cout << "getAmbient()=" << vcnl->getAmbient() << std::endl;
				std::cout << "getWhite()=" << vcnl->getWhite() << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			}
		} catch (i2cException e) {
			std::cerr << "device disconnect: " << e.what() << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

int main(int argc, const char **argv)
{
	i2cBus::list_devices();
//	signal(SIGSEGV, handler);   // install our handler

	try {
		auto bcm = std::make_shared<bcm2835>();
		auto i2c = std::make_shared<i2cBus>(bcm);

		i2c->list_connected_devices();

		auto humidity_thread = std::make_shared<std::thread>(watch_humidity, i2c);
		auto proximity_thread = std::make_shared<std::thread>(watch_proximity, i2c);
		std::this_thread::sleep_for(std::chrono::seconds(60));
	} catch (i2cException e) {
		std::cerr << "got exception: " << e.what() << std::endl;
	}
}
