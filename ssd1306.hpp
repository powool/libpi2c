#ifndef _SSD1306_
#define _SSD1306_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "i2cbus.hpp"

class SSD1306 {

// register definitions
	static const uint8_t SET_CONTRAST = 0x81;
	static const uint8_t SET_ENTIRE_ON = 0xA4;
	static const uint8_t SET_NORM_INV = 0xA6;
	static const uint8_t SET_DISP = 0xAE;
	static const uint8_t SET_MEM_ADDR = 0x20;
	static const uint8_t SET_COL_ADDR = 0x21;
	static const uint8_t SET_PAGE_ADDR = 0x22;
	static const uint8_t SET_DISP_START_LINE = 0x40;
	static const uint8_t SET_SEG_REMAP = 0xA0;
	static const uint8_t SET_MUX_RATIO = 0xA8;
	static const uint8_t SET_COM_OUT_DIR = 0xC0;
	static const uint8_t SET_DISP_OFFSET = 0xD3;
	static const uint8_t SET_COM_PIN_CFG = 0xDA;
	static const uint8_t SET_DISP_CLK_DIV = 0xD5;
	static const uint8_t SET_PRECHARGE = 0xD9;
	static const uint8_t SET_VCOM_DESEL = 0xDB;
	static const uint8_t SET_CHARGE_PUMP = 0x8D;
	static const uint8_t SSD1306_DEFAULT_ADDRESS = 0x3C;

	std::shared_ptr<i2cBus> bus;
	uint8_t	width;
	uint8_t	height;
	uint8_t	pages;
	bool externalVcc;
	bool displayPower;
	std::vector<uint8_t> buffer;
public:
	static bool register_device() {
		return i2cBus::register_device(SSD1306_DEFAULT_ADDRESS, "ssd1306");
	}

	SSD1306(std::shared_ptr<i2cBus> bus_, uint16_t width_, uint16_t height_);

	void initDisplay(void);

	void powerOff(void);

	void contrast(uint8_t contrast);

	void invert(bool invert);

	void powerOn(void);

	void show();

	void write_cmd(uint8_t cmd);

	void write_framebuf();

	std::vector<uint8_t> &getBuffer();
	uint16_t	getBufferOffset();
};

#endif
