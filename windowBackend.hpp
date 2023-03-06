#pragma  once
#include <iostream>
#include <vector>
extern "C"{
#include "libtmt/tmt.h"
}

struct color{
	uint8_t r, g, b, a;
};

class textModifiers{
public:
	bool bold = false;
	bool underline = false;
	color foreground = {255, 255, 255, 0};
	color background = {0, 0, 0, 0};
	bool operator==(const textModifiers& other){
		return(bold == other.bold && underline == other.underline && foreground.r == other.foreground.r && foreground.g == other.foreground.g && foreground.b == other.foreground.b && foreground.a == other.foreground.a && background.r == other.background.r && background.g == other.background.g && background.b == other.background.b && background.a == other.background.a);
	}
	bool operator!=(const textModifiers& other){
		return !(*this == other);
	}
	textModifiers(){}
	textModifiers(const TMTATTRS& attrs){
		bold = attrs.bold;
		underline = attrs.underline;
		switch(attrs.fg){
			case TMT_COLOR_BLACK:
				foreground = {0, 0, 0, 0};
				break;
			case TMT_COLOR_RED:
				foreground = {255, 0, 0, 0};
				break;
			case TMT_COLOR_GREEN:
				foreground = {0, 255, 0, 0};
				break;
			case TMT_COLOR_YELLOW:
				foreground = {255, 255, 0, 0};
				break;
			case TMT_COLOR_BLUE:
				foreground = {0, 0, 255, 0};
				break;
			case TMT_COLOR_MAGENTA:
				foreground = {255, 0, 255, 0};
				break;
			case TMT_COLOR_CYAN:
				foreground = {0, 255, 255, 0};
				break;
			case TMT_COLOR_WHITE:
			case TMT_COLOR_DEFAULT:
			default:
				foreground = {255, 255, 255, 0};
				break;
		}
		switch(attrs.bg){
			case TMT_COLOR_BLACK:
				background = {0, 0, 0, 0};
				break;
			case TMT_COLOR_RED:
				background = {255, 0, 0, 0};
				break;
			case TMT_COLOR_GREEN:
				background = {0, 255, 0, 0};
				break;
			case TMT_COLOR_YELLOW:
				background = {255, 255, 0, 0};
				break;
			case TMT_COLOR_BLUE:
				background = {0, 0, 255, 0};
				break;
			case TMT_COLOR_MAGENTA:
				background = {255, 0, 255, 0};
				break;
			case TMT_COLOR_CYAN:
				background = {0, 255, 255, 0};
				break;
			case TMT_COLOR_WHITE:
				background = {255, 255, 255, 0};
				break;
			case TMT_COLOR_DEFAULT:
			default:
				background = {0, 0, 0, 0};
				break;
		}
	}
};

class windowBackend{
public:
	windowBackend(){}
	virtual ~windowBackend(){}
	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual void update() = 0;
	virtual void drawText(const std::string& text, const std::pair<unsigned int, unsigned int>& pos, const textModifiers& mods) = 0;
	virtual void drawRect(const std::pair<unsigned int, unsigned int>& pos, const std::pair<unsigned int, unsigned int>& size, const color& c) = 0;
	virtual void updateWindow() = 0;
	virtual std::pair<unsigned int, unsigned int> getFontCharSize(){return {0,0};}
	virtual std::vector<std::string> getKeysPressed(){return {};}
	bool opened = false;
	std::pair<unsigned int, unsigned int> resolution;
	unsigned fontSize = 15;
};
