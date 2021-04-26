
#include "render.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

uint16_t	render::getPixelOffset(uint16_t column, uint16_t row)
{
	return 0;
}

render::render(std::vector<uint8_t> &buffer_, uint16_t width_, uint16_t height_, uint16_t depth_, uint16_t bufferOffset_) :
	buffer(buffer_), width(width_), height(height_), depth(depth_), bufferOffset(bufferOffset_)
{
}

void render::setPixelColor(uint16_t color_)
{
	color = color_;
}

void render::setPixel(uint16_t column, uint16_t row)
{
}

void render::setAllPixels()
{
	auto bits = (color != 0) ? 0xff : 0x00;
	for(auto byteIndex = bufferOffset; byteIndex < buffer.size(); byteIndex++) { buffer[byteIndex] = bits; }
}

void render::pixel(uint16_t x, uint16_t y)
{
//	index = (y >> 3) * framebuf.stride + x
//        offset = y & 0x07
	auto byteIndex = (y >> 3) * (width >> 3) + (x >> 3) + bufferOffset;
	auto bitIndex = x & 0x7;
	buffer[byteIndex] = (buffer[byteIndex] & ~(0x01 << bitIndex)) |
		(color != 0) << bitIndex;
	std::cout << "x = " << x << " y = " << y << " index = " << byteIndex << " size = " << buffer.size() << " height = " << height << " width = " << width << std::endl;
}

void render::line(uint16_t x_, uint16_t y_, uint16_t x2_, uint16_t y2_)
{
	int x = x_;
	int y = y_;
	int x2 = x2_;
	int y2 = y2_;
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 8) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x80+(x<<8);y<=longLen;++y) {
				pixel(j >> 8,y);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x80+(x<<8);y>=longLen;--y) {
			pixel(j >> 8,y);
			j-=decInc;
		}
		return;
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x80+(y<<8);x<=longLen;++x) {
			pixel(x,j >> 8);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x80+(y<<8);x>=longLen;--x) {
		pixel(x,j >> 8);
		j-=decInc;
	}

}
#if 0
void render::line(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
	int16_t d_x = std::abs(int16_t(x_1) - int16_t(x_0));
	int16_t d_y = std::abs(int16_t(y_1) - int16_t(y_0));

	auto x = x_0;
	auto y = y_0;
	int16_t s_x = (x_0 > x_1) ? -1 : 1;
	int16_t s_y = (y_0 > y_1) ? -1 : 1;

	if(d_x > d_y) {
		auto err = d_x / 2.0;
		while(x != x_1) {
			pixel(x, y);
			err -= d_y;
			if(err < 0) {
				y += s_y;
				err += d_x;
			}
			x += s_x;
		}
	} else {
		auto err = d_y / 2.0;
		while(y != y_1) {
			pixel(x, y);
			err -= d_x;
			if(err < 0) {
				x += s_x;
				err += d_y;
			}
			y += s_y;
		}
	}
	pixel(x, y);
}
#endif

// https://github.com/adafruit/Adafruit_CircuitPython_framebuf/blob/master/adafruit_framebuf.py
void render::circle(uint16_t center_x, uint16_t center_y, uint16_t radius)
{
	// Draw a circle at the given midpoint location, radius and color.
	// The ```circle``` method draws only a 1 pixel outline.
	int16_t x = radius - 1;
	int16_t y = 0;
	int16_t d_x = 1;
	int16_t d_y = 1;
	int16_t err = d_x - (radius << 1);
	while(x >= y) {
		pixel(center_x + x, center_y + y);
		pixel(center_x + y, center_y + x);
		pixel(center_x - y, center_y + x);
		pixel(center_x - x, center_y + y);
		pixel(center_x - x, center_y - y);
		pixel(center_x - y, center_y - x);
		pixel(center_x + y, center_y - x);
		pixel(center_x + x, center_y - y);
		if(err <= 0) {
			y += 1;
			err += d_y;
			d_y += 2;
		}
		if(err > 0) {
			x -= 1;
			d_x += 2;
			err += d_x - (radius << 1);
		}
	}
}
