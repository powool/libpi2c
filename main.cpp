#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include <iostream>
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

int main(int argc, const char **argv)
{
//	signal(SIGSEGV, handler);   // install our handler

	try {
		auto bcm = std::make_shared<bcm2835>();
		auto i2c = std::make_shared<i2cBus>(bcm);
		auto vcnl = std::make_shared<vcnl4040>(i2c);

		auto htSensor = std::make_shared<aht20>(i2c);
		htSensor->readSensor();
		std::cout << "temperature=" << htSensor->getTemperature() << std::endl;
		std::cout << "humidity=" << htSensor->getHumidity() << std::endl;

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
			sleep(2);
			std::cout << "getProximity()=" << vcnl->getProximity() << std::endl;
			std::cout << "getAmbient()=" << vcnl->getAmbient() << std::endl;
			std::cout << "getWhite()=" << vcnl->getWhite() << std::endl;
		}
	} catch (i2cException e) {
		std::cerr << "got exception: " << e.what() << std::endl;
	}
}
