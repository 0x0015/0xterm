#include "SDL2Backend.hpp"
extern "C"{
#include "libtmt/tmt.h"
}
#include <unistd.h>

SDL2Backend::SDL2Backend(){

}

SDL2Backend::~SDL2Backend(){
	if(opened)
		close();
}

bool SDL2Backend::open(){
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0){
		std::cerr<<"Failed to initialize SDL2"<<std::endl;
		return false;
	}

	if(TTF_Init() < 0){
		std::cerr<<"Failed to initialize SDL2_ttf"<<std::endl;
		return false;
	}


	window = SDL_CreateWindow("Terminal Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.first, resolution.second, 0);
	if(!window){
		std::cerr<<"Failed to initialize SDL2 window"<<std::endl;
		return false;
	}

	window_surface = SDL_GetWindowSurface(window);
	if(!window_surface){
		std::cerr<<"Failed to get SDL2 window surface"<<std::endl;
		return false;
	}


	std::string fontFilename = "/home/illiam/Desktop/terminalEmu/font.ttf";
	font = TTF_OpenFont(fontFilename.c_str(), fontSize);

	if(!font){
		std::cerr<<"Failed to load font: "<<fontFilename<<std::endl;
		std::cerr<<TTF_GetError();
		return false;
	}

	SDL_StartTextInput();

	SDL_UpdateWindowSurface(window);

	opened = true;
	return true;
}

bool SDL2Backend::close(){
	if(!opened)
		return false;
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return true;
}


std::pair<unsigned int, unsigned int> SDL2Backend::getFontCharSize(){
	if(!opened)
		return {};
	int w, h;
	TTF_SizeUTF8(font, " ", &w, &h);
	return {w, h};
}

void SDL2Backend::update(){
	while(SDL_PollEvent(&e) > 0){
		bool gotInputKey = false;
		switch(e.type){
			case SDL_QUIT:
				opened = false;
				break;
			case SDL_TEXTINPUT:
				keysDown.push_back(e.text.text);
				gotInputKey = true;
				break;
			case SDL_KEYDOWN:
				if(!gotInputKey){
					switch(e.key.keysym.sym){
						case SDLK_BACKSPACE:
							keysDown.push_back("\b");
							break;
						case SDLK_RETURN:
							keysDown.push_back("\n");
							break;
						case SDLK_TAB:
							keysDown.push_back("\t");
							break;
						case SDLK_ESCAPE:
							keysDown.push_back("\033");
							break;
						case SDLK_UP:
							keysDown.push_back(TMT_KEY_UP);
							break;
						case SDLK_DOWN:
							keysDown.push_back(TMT_KEY_DOWN);
							break;
						case SDLK_LEFT:
							keysDown.push_back(TMT_KEY_LEFT);
							break;
						case SDLK_RIGHT:
							keysDown.push_back(TMT_KEY_RIGHT);
							break;
						default:
							break;
					}
				}
				break;
		}
	}
	while(lastTime-SDL_GetTicks() < (1000/60)){
		SDL_Delay(1);
	}
	lastTime = SDL_GetTicks();
}

std::vector<std::string> SDL2Backend::getKeysPressed(){
	std::vector<std::string> output = keysDown;
	keysDown.clear();
	return output;
}

void SDL2Backend::drawText(const std::string& text, const std::pair<unsigned int, unsigned int>& pos, const textModifiers& mods){
	int textFlags = TTF_STYLE_NORMAL;
	if(mods.bold)
		textFlags &= TTF_STYLE_BOLD;
	if(mods.underline)
		textFlags &= TTF_STYLE_UNDERLINE;
	TTF_SetFontStyle(font, textFlags);
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, text.c_str(), {mods.foreground.r, mods.foreground.g, mods.foreground.b, mods.foreground.a}, {mods.background.r, mods.background.g, mods.background.b, mods.background.a});
	SDL_Rect destRect = {(int)pos.first, (int)pos.second, textSurface->w, textSurface->h};
	SDL_BlitSurface(textSurface, NULL, window_surface, &destRect);
	SDL_FreeSurface(textSurface);
}

void SDL2Backend::drawRect(const std::pair<unsigned int, unsigned int>& pos, const std::pair<unsigned int, unsigned int>& size, const color& c){
	SDL_Rect r = {(int)pos.first, (int)pos.second, (int)size.first, (int)size.second};
	uint32_t hexcolor = SDL_MapRGBA(window_surface->format, c.r, c.g, c.b, c.a);
	SDL_FillRect(window_surface, &r, hexcolor);
}

void SDL2Backend::updateWindow(){
	SDL_UpdateWindowSurface(window);
}
