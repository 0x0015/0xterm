#pragma once
#include "windowBackend.hpp"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

class SDLBackend : public windowBackend{
public:	
	SDLBackend();
	~SDLBackend();
	bool open();
	bool close();
	void update();
	void drawText(const std::string& text, const std::pair<unsigned int, unsigned int>& pos, const textModifiers& mods);
	void drawRect(const std::pair<unsigned int, unsigned int>& pos, const std::pair<unsigned int, unsigned int>& size, const color& c);
	void updateWindow();
	std::pair<unsigned int, unsigned int> getFontCharSize();
	std::vector<std::string> getKeysPressed();
	std::vector<std::string> keysDown;
	//SDL_Window* window;
	SDL_Surface* window_surface;
	SDL_Event e;
	TTF_Font* font;
	int lastTime = 0;
};
