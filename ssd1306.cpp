#include "ssd1306.hpp"

static auto dummy = SSD1306::register_device();

SSD1306::SSD1306(std::shared_ptr<i2cBus> bus_, uint16_t width_, uint16_t height_) :
	bus(bus_), width(width_), height(height_), pages(height_)
{

	if(!bus->connected(SSD1306_DEFAULT_ADDRESS))
		throw i2cException("SSD1306 not responding - check i2c connections");

	externalVcc = false;
	displayPower = false;
	powerOn();
	initDisplay();
}

void SSD1306::initDisplay(void)
{
	// The various screen sizes available with the ssd1306 OLED driver
	// chip require differing configuration values for the display clock
	// div and com pin, which are listed below for reference and future
	// compatibility:
	//    w,  h: DISP_CLK_DIV  COM_PIN_CFG
	//  128, 64:         0x80         0x12
	//  128, 32:         0x80         0x02
	//   96, 16:         0x60         0x02
	//   64, 48:         0x80         0x12
	//   64, 32:         0x80         0x12
	powerOff();
	write_cmd(SET_MEM_ADDR);
	write_cmd(0x00);
	write_cmd(SET_DISP_START_LINE | 0x00);
	write_cmd(SET_SEG_REMAP | 0x01);
	write_cmd(SET_MUX_RATIO);
	write_cmd(height - 1);
	write_cmd(SET_COM_OUT_DIR | 0x08);
	write_cmd(SET_DISP_OFFSET);
	write_cmd(0x00);
	write_cmd(SET_COM_PIN_CFG);
	write_cmd((height == 32 || height == 16) && (width != 64) ? 0x02 : 0x12);
	write_cmd(SET_DISP_CLK_DIV);
	write_cmd(0x80);
	write_cmd(SET_PRECHARGE);
	write_cmd(externalVcc ? 0x22 : 0xf1);
	write_cmd(SET_VCOM_DESEL);
	write_cmd(0x30);
	write_cmd(SET_CONTRAST);
	write_cmd(0xff);
	write_cmd(SET_ENTIRE_ON);
	write_cmd(SET_NORM_INV);
	write_cmd(SET_CHARGE_PUMP);
	write_cmd(externalVcc ? 0x10 : 0x14);
	powerOn();


	if(width == 72) {
		write_cmd(0xad);
		write_cmd(0x30);
	}

	// Add an extra byte to the data buffer to hold an I2C data/command byte
	// to use hardware-compatible I2C transactions.  A memoryview of the
	// buffer is used to mask this byte from the framebuffer operations
	// (without a major memory hit as memoryview doesn't copy to a separate
	// buffer).

	buffer.resize(((width / 8) * height) + 1);
	buffer[0] = 0x40;	// Set first byte of data buffer to Co=0, D/C=1

//	fill(0);
	show();
}

void SSD1306::powerOff(void) {
	write_cmd(SET_DISP | 0x00);
	displayPower = false;
}

void SSD1306::contrast(uint8_t contrast) {
	// Adjust the contrast
	write_cmd(SET_CONTRAST);
	write_cmd(contrast);
}

void SSD1306::invert(bool invert) {
	// Invert all pixels on the display
	write_cmd(SET_NORM_INV | (invert & 1));
}

void SSD1306::powerOn(void) {
	// Reset device and turn on the display.
	write_cmd(SET_DISP | 0x01);
	displayPower = true;
}

void SSD1306::show() {
	// Update the display
	uint8_t xpos0 = 0;
	uint8_t xpos1 = width - 1;
	if(width == 64) {
		// displays with width of 64 pixels are shifted by 32
		xpos0 += 32;
		xpos1 += 32;
	}
	if(width == 72) {
		// displays with width of 72 pixels are shifted by 28
		xpos0 += 28;
		xpos1 += 28;
	}
	write_cmd(SET_COL_ADDR);
	write_cmd(xpos0);
	write_cmd(xpos1);
	write_cmd(SET_PAGE_ADDR);
	write_cmd(0);
	write_cmd(pages - 1);
	write_framebuf();
}

void SSD1306::write_cmd(uint8_t cmd) {
	uint8_t buffer[2];
	// Send a command to the sdd1306
	buffer[0] = 0x80;	// Co=1, D/C#=0
	buffer[1] = cmd;
	bus->write(SSD1306_DEFAULT_ADDRESS, buffer, 2);
}

void SSD1306::write_framebuf() {
	// Blast out the frame buffer using a single I2C transaction to support
	// hardware I2C interfaces.
	bus->write(SSD1306_DEFAULT_ADDRESS, &buffer[0], buffer.size());
}

std::vector<uint8_t> &SSD1306::getBuffer()
{
	return buffer;
}

uint16_t SSD1306::getBufferOffset()
{
	return 1;
}

