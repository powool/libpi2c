#ifndef _RENDER_
#define _RENDER_

#include <cstdint>
#include <vector>

class render {

	uint16_t	width;
	uint16_t	height;
	uint16_t	depth;
	uint16_t	bufferOffset;
	uint16_t	color;
	std::vector<uint8_t> &buffer;

	uint16_t	getPixelOffset(uint16_t column, uint16_t row);
public:
	render(std::vector<uint8_t> &buffer_, uint16_t width_, uint16_t height_, uint16_t depth_, uint16_t bufferOffset_);

	void setPixelColor(uint16_t color);

	void setPixel(uint16_t column, uint16_t row);

	void setAllPixels();

	void pixel(uint16_t x, uint16_t y);
	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void circle(uint16_t x, uint16_t y, uint16_t radius);
};

#endif
