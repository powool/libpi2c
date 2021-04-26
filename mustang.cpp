#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <unistd.h>
#include "checkTerminate.hpp"
#include "i2cbus.hpp"
#include "ads1015.hpp"
#include "ds3502.hpp"

#include <fstream>
#include <type_traits>

template<class T> void lockedLambda(std::mutex &m, T f) {
	std::lock_guard<std::mutex> lock(m);
	f();
}

template<typename T> void swap(T &a, T &b)
{
	static_assert(std::is_integral<T>::value, "swap requires integral types");
	a ^= b;
	b ^= a;
	a ^= b;
}

#if 0
template<typename T> void swap(T &a, T &b)
{
	auto tmp = b;
	b = a;
	a = tmp;
}
#endif

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

std::mutex ioLock;

float getTemp(float volts)
{
	// values from https://www.broncocorral.com/tech_library/ford-eec-iv-and-tfi-diagnostics-manual/
	const float volts2temp[] = {
		0.12, 302,
		0.20, 267,
		0.30, 250,
		0.36, 230,
		0.47, 212,
		0.61, 194,
		0.80, 176,
		1.04, 158,
		1.35, 140,
		1.72, 122,
		2.16, 104,
		2.62, 86,
		3.06, 68,
		3.70, 50,
		3.97, 32,
		4.42, 14,
		4.87, -4,
		4.89, -22,
		4.91, -40
	};

	const int rows = sizeof(volts2temp) / 2;

	if(volts < .12) volts = .12;
	if(volts > 4.89) volts=4.89;

	int i;
	for(i=0; i<rows*2; i++) {
		if(volts <= volts2temp[i + 2]) break;
	}

	auto deltaVolts = volts2temp[i+2] - volts2temp[i+0];
	auto deltaTemp = volts2temp[i+1] - volts2temp[i+3];

	auto temp = volts2temp[i+1] + (volts2temp[i] - volts)/deltaVolts * deltaTemp;

	return temp;
}

float engineCoolantTemperature = 0.0;	// in degrees F

void readTemp(std::shared_ptr<i2cBus> i2c, std::ofstream *screen)
{
	auto tempSensor = std::make_shared<ads1015>(i2c);
	while(true) {
		try {
			checkTerminate::check();
//			tempSensor->readSensor();
			lockedLambda(ioLock, [&] {
					*screen << "\x1b[1;1H";		// row 1, col 1
					*screen << "\x1b[31;1m";	// red
//					*screen << "temperature=" << tempSensor->getTemperature() << "\x1b[0K" << std::endl;
				});
			std::this_thread::sleep_for(std::chrono::seconds(1));
		} catch (i2cException e) {
			std::cerr << "ADC device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
		} catch (checkException e) {
			std::cerr << "ADC device shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

void fanController(std::shared_ptr<i2cBus> i2c)
{
	// XXX need two:
	auto digitalPotentiometer = std::make_shared<ds3502>(i2c, 0);
	int fanSpeed = 0;	// 0-127
	bool fanShouldRun = false;
	while(true) {
		try {
			checkTerminate::check();

			if (engineCoolantTemperature < 175) {
				fanShouldRun = false;
			} else if (engineCoolantTemperature > 185) {
				fanShouldRun = true;
			}
				
			if (fanShouldRun) {
				fanSpeed += 1;
			} else {
				fanSpeed -= 1;
			}

			fanSpeed = std::min(fanSpeed, 127);
			fanSpeed = std::max(fanSpeed, 0);

			// set fan speed here
			digitalPotentiometer->setWiper(fanSpeed);

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} catch (i2cException e) {
			std::cerr << "digital potentiometer device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(5));
		} catch (checkException e) {
			std::cerr << "Fan control thread shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

void waterPumpController(std::shared_ptr<i2cBus> i2c)
{
	// XXX need two:
	auto digitalPotentiometer = std::make_shared<ds3502>(i2c, 1);
	int waterPumpSpeed = 0;	// 0-127
	bool waterPumpShouldRun = false;
	while(true) {
		try {
			checkTerminate::check();

			if (engineCoolantTemperature < 175) {
				waterPumpShouldRun = false;
			} else if (engineCoolantTemperature > 185) {
				waterPumpShouldRun = true;
			}
				
			if (waterPumpShouldRun) {
				waterPumpSpeed += 1;
			} else {
				waterPumpSpeed -= 1;
			}

			waterPumpSpeed = std::min(waterPumpSpeed, 127);
			waterPumpSpeed = std::max(waterPumpSpeed, 20);

			// set fan speed here
			digitalPotentiometer->setWiper(waterPumpSpeed);

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} catch (i2cException e) {
			std::cerr << "digital potentiometer device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(5));
		} catch (checkException e) {
			std::cerr << "Water pump control thread shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

int main(int argc, const char **argv)
{
	// see https://en.wikipedia.org/wiki/ANSI_escape_code
	auto screen = std::ofstream("/dev/console", std::ofstream::out);
//	signal(SIGSEGV, handler);   // install our handler
	signal(SIGINT, checkTerminate::interrupt);

	auto bcm = std::make_shared<bcm2835>();
	auto i2c = std::make_shared<i2cBus>(bcm);

	i2cBus::list_devices();
	std::cout << std::endl;
	i2c->list_connected_devices();

	screen << "\x1b[0m";
	screen << "\x1b[2J";	// clear screen
	screen << "\x1b[1;1H";		// row 1, col 1
	screen << "\x1b[31;1m";	// red
	screen << "MUSTANG";

	auto readTempThread = std::make_shared<std::thread>(readTemp, i2c, &screen);
	auto fanThread = std::make_shared<std::thread>(fanController, i2c);
	auto waterPumpThread = std::make_shared<std::thread>(waterPumpController, i2c);


	try {
		while(true) {
			checkTerminate::check();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (i2cException e) {
		std::cerr << "main got exception: " << e.what() << std::endl;
	} catch (checkException e) {
		std::cerr << "main shutdown: " << e.what() << std::endl;
	}

	readTempThread->join();
	fanThread->join();
	waterPumpThread->join();
}
