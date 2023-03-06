#include "SDLBackend.hpp"
extern "C"{
#include "libtmt/tmt.h"
}
#include <unistd.h>
#include <string>
#include <locale>

SDLBackend::SDLBackend(){

}

SDLBackend::~SDLBackend(){
	if(opened)
		close();
}

bool SDLBackend::open(){
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0){
		std::cerr<<"Failed to initialize SDL2"<<std::endl;
		return false;
	}

	if(TTF_Init() < 0){
		std::cerr<<"Failed to initialize SDL2_ttf"<<std::endl;
		return false;
	}


	//window = SDL_CreateWindow("Terminal Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.first, resolution.second, 0);
	//if(!window){
	//	std::cerr<<"Failed to initialize SDL2 window"<<std::endl;
	//	return false;
	//}
	SDL_SetVideoMode(resolution.first, resolution.second, 0, SDL_SWSURFACE);

	window_surface = SDL_GetVideoSurface();
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

	//SDL_StartTextInput();
	SDL_EnableUNICODE(1);

	SDL_Flip(window_surface);

	opened = true;
	return true;
}

bool SDLBackend::close(){
	if(!opened)
		return false;
	//SDL_FreeSurface(window_surface);
	//SDL_DestroyWindow(window);

	SDL_Quit();

	return true;
}


std::pair<unsigned int, unsigned int> SDLBackend::getFontCharSize(){
	if(!opened)
		return {};
	int w, h;
	TTF_SizeUTF8(font, " ", &w, &h);
	return {w, h};
}

std::string wchar_to_UTF8(const wchar_t * in)
{
    std::string out;
    unsigned int codepoint = 0;
    for (in;  *in != 0;  ++in)
    {
        if (*in >= 0xd800 && *in <= 0xdbff)
            codepoint = ((*in - 0xd800) << 10) + 0x10000;
        else
        {
            if (*in >= 0xdc00 && *in <= 0xdfff)
                codepoint |= *in - 0xdc00;
            else
                codepoint = *in;

            if (codepoint <= 0x7f)
                out.append(1, static_cast<char>(codepoint));
            else if (codepoint <= 0x7ff)
            {
                out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else if (codepoint <= 0xffff)
            {
                out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else
            {
                out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            codepoint = 0;
        }
    }
    return out;
}

void SDLBackend::update(){
	while(SDL_PollEvent(&e) > 0){
		bool gotInputKey = false;
		switch(e.type){
			case SDL_QUIT:
				opened = false;
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
						default:{
							std::wstring widestr;
							widestr += e.key.keysym.unicode;
							keysDown.push_back(wchar_to_UTF8(&widestr[0]));
							}
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				break;
		}
	}
	while(lastTime-SDL_GetTicks() < (1000/60)){
		SDL_Delay(1);
	}
	lastTime = SDL_GetTicks();
}

std::vector<std::string> SDLBackend::getKeysPressed(){
	std::vector<std::string> output = keysDown;
	keysDown.clear();
	return output;
}

void SDLBackend::drawText(const std::string& text, const std::pair<unsigned int, unsigned int>& pos, const textModifiers& mods){
	int textFlags = TTF_STYLE_NORMAL;
	if(mods.bold)
		textFlags &= TTF_STYLE_BOLD;
	if(mods.underline)
		textFlags &= TTF_STYLE_UNDERLINE;
	TTF_SetFontStyle(font, textFlags);
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, text.c_str(), {mods.foreground.r, mods.foreground.g, mods.foreground.b, mods.foreground.a}, {mods.background.r, mods.background.g, mods.background.b, mods.background.a});
	SDL_Rect destRect = {(int16_t)pos.first, (int16_t)pos.second, (uint16_t)textSurface->w, (uint16_t)textSurface->h};
	SDL_BlitSurface(textSurface, NULL, window_surface, &destRect);
	SDL_FreeSurface(textSurface);
}

void SDLBackend::drawRect(const std::pair<unsigned int, unsigned int>& pos, const std::pair<unsigned int, unsigned int>& size, const color& c){
	SDL_Rect r = {(int16_t)pos.first, (int16_t)pos.second, (uint16_t)size.first, (uint16_t)size.second};
	uint32_t hexcolor = SDL_MapRGBA(window_surface->format, c.r, c.g, c.b, c.a);
	SDL_FillRect(window_surface, &r, hexcolor);
}

void SDLBackend::updateWindow(){
	SDL_Flip(window_surface);
}
