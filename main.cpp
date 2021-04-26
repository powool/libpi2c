#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <unistd.h>
#include "checkTerminate.hpp"
#include "aht20.hpp"
#include "i2cbus.hpp"
#include "lsm6ds33.hpp"
#include "render.hpp"
#include "ssd1306.hpp"
#include "tmp117.hpp"
#include "vcnl4040.hpp"

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

void watch_humidity(std::shared_ptr<i2cBus> i2c, std::ofstream *screen)
{
	auto htSensor = std::make_shared<aht20>(i2c);
	while(true) {
		try {
			checkTerminate::check();
			htSensor->readSensor();
			lockedLambda(ioLock, [&] {
					*screen << "\x1b[1;1H";		// row 1, col 1
					*screen << "\x1b[31;1m";	// red
					*screen << "temperature=" << htSensor->getTemperature() << "\x1b[0K" << std::endl;
					*screen << "humidity=" << htSensor->getHumidity() << "\x1b[0K";
				});
			std::this_thread::sleep_for(std::chrono::seconds(1));
		} catch (i2cException e) {
			std::cerr << "humidity device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
		} catch (checkException e) {
			std::cerr << "humidity device shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

void watch_proximity(std::shared_ptr<i2cBus> i2c, std::ofstream *screen)
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
				checkTerminate::check();
#if 0
				if (vcnl->isClose()) {
					std::cout << "Oh so close!" << std::endl;
				}
				if (vcnl->isAway()) {
					std::cout << "Oh so far!" << std::endl;
				}
#endif
				lockedLambda(ioLock, [&] {
					*screen << "\x1b[7;1H";		// row 7, col 1
					*screen << "\x1b[34;1m";	// blue
					*screen << "getProximity()=" << vcnl->getProximity() << "\x1b[0K";
//					std::cout << "getAmbient()=" << vcnl->getAmbient() << std::endl;
//					std::cout << "getWhite()=" << vcnl->getWhite() << std::endl;
					});
				std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			}
		} catch (i2cException e) {
			std::cerr << "proximity device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			// configure again
			vcnl = std::make_shared<vcnl4040>(i2c);
		} catch (checkException e) {
			std::cerr << "proximity device shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

void watch_gyro(std::shared_ptr<i2cBus> i2c, std::ofstream *screen)
{
	auto gyro = std::make_shared<lsm6ds33>(i2c);
	while(true) {
		try {
			checkTerminate::check();
			gyro->read();
			lockedLambda(ioLock, [&] {
					*screen << "\x1b[3;1H";		// row 3, col 1
					*screen << "\x1b[32;1m";	// green
					*screen << "temperature=" << gyro->temperature << "\x1b[0K" << std::endl;
					*screen << "\x1b[0K";	// clear to end of line
					*screen << "accX=" << gyro->accX << "\x1b[0K" << std::endl;
					*screen << "accY=" << gyro->accY << "\x1b[0K" << std::endl;
					*screen << "accZ=" << gyro->accZ << "\x1b[0K";
				});
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} catch (i2cException e) {
			std::cerr << "gyro device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			// configure again
			gyro = std::make_shared<lsm6ds33>(i2c);
		} catch (checkException e) {
			std::cerr << "gyro device shutdown: " << e.what() << std::endl;
			return;
		}
	}
}

void watch_temp(std::shared_ptr<i2cBus> i2c, std::ofstream *screen)
{
	auto temp = std::make_shared<tmp117>(i2c);
	while(true) {
		try {
			checkTerminate::check();
			auto t = temp->readTemperature();
			lockedLambda(ioLock, [&] {
					*screen << "\x1b[8;1H";		// row 3, col 1
					*screen << "\x1b[32;1m";	// green
					*screen << "temperature=" << t << "\x1b[0K";
				});
			std::this_thread::sleep_for(std::chrono::seconds(1));
		} catch (i2cException e) {
			std::cerr << "tmp117 device disconnect: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			// configure again
			temp = std::make_shared<tmp117>(i2c);
		} catch (checkException e) {
			std::cerr << "tmp117 device shutdown: " << e.what() << std::endl;
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
#if 0
	try {
		auto display = std::make_shared<SSD1306>(i2c, (uint16_t) 128, (uint16_t) 32);

		display->initDisplay();

		display->powerOn();

		display->invert(true);

		std::this_thread::sleep_for(std::chrono::seconds(1));

		display->invert(false);

		std::this_thread::sleep_for(std::chrono::seconds(1));

		render r(display->getBuffer(), 128, 32, 1, display->getBufferOffset());
		r.setPixelColor(0xffff);
#if 0
		r.line(0,0, 10,10);
		r.pixel(2,5);
		r.pixel(3,6);
		r.pixel(4,7);
#endif
//		r.circle(30,16,5);
		r.line(0,0,127,31);
		std::cout << "done writing circle" << std::endl;
#if 0
		{
			for(auto x=0;x<32;x++) {
				r.pixel(x,x);
				r.pixel(x+32,x);
				r.pixel(x+64,x);
				r.pixel(x+96,x);
			}
		}
#endif
		display->show();

	} catch (i2cException e) {
		std::cerr << "got exception: " << e.what() << std::endl;
	} catch (checkException e) {
		std::cerr << "final shutdown: " << e.what() << std::endl;
	}
#endif
	i2cBus::list_devices();
	std::cout << std::endl;
	i2c->list_connected_devices();

	screen << "\x1b[0m";
	screen << "\x1b[2J";	// clear screen
	auto humidity_thread = std::make_shared<std::thread>(watch_humidity, i2c, &screen);
	auto proximity_thread = std::make_shared<std::thread>(watch_proximity, i2c, &screen);
	auto gyro_thread = std::make_shared<std::thread>(watch_gyro, i2c, &screen);
	auto temp_thread = std::make_shared<std::thread>(watch_temp, i2c, &screen);

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
	humidity_thread->join();
	gyro_thread->join();
	temp_thread->join();
	proximity_thread->join();
}
