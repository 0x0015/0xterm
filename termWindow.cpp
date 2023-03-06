#include "termWindow.hpp"
#include <unistd.h>
#include "OSK.hpp"

termWindow::termWindow(){

}

termWindow::~termWindow(){
	if(backend){
		if(backend->opened)
			backend->close();
		delete backend;
	}
	if(osk)
		delete osk;
}

bool termWindow::open(){
	if(!backend)
		return false;
	return backend->open();
}

bool termWindow::close(){
	if(!backend)
		return false;
	return backend->close();
}



bool termWindow::updateScreen(TMT* vt){
	if(!backend || !backend->opened)
		return false;
	
        const TMTSCREEN *s = tmt_screen(vt);
	textModifiers currentModifiers;
        for (size_t r = 0; r < s->nline; r++){
        	if (s->lines[r]->dirty){
			unsigned int currentChar = 0;
			bool lineDone = false;
			while(!lineDone){
			std::string text;
			text.reserve(s->ncol+1);
			textModifiers newMods = currentModifiers;
			unsigned int charBefore = currentChar;
                	for (;currentChar < s->ncol; currentChar++){
				text+=s->lines[r]->chars[currentChar].c;
				textModifiers tmpMods(s->lines[r]->chars[currentChar].a);
				if(tmpMods != currentModifiers){
					//std::cout<<"Found new style"<<std::endl;
					newMods = tmpMods;
					break;
				}
                	}
			if(currentChar >= s->ncol)
				lineDone = true;
			
			std::pair<unsigned int, unsigned int> fontSize = backend->getFontCharSize();
			int w = fontSize.first;
			int h = fontSize.second;
			backend->drawRect({w * (int)charBefore, h * (int)r},{ w * text.length(), h}, currentModifiers.background);
			backend->drawText(text, {(int)w * (int)charBefore, (int)h * (int)r}, currentModifiers);

			currentModifiers = newMods;
			}
		}
        }
	if(inputMode > 0 && osk)
		osk->draw();
	backend->updateWindow();

	return true;
}

std::pair<unsigned int, unsigned int> termWindow::getRowsCols(){
	if(!backend || !backend->opened)
		return {0, 0};
	
	std::pair<unsigned int, unsigned int> wh = backend->getFontCharSize();
	std::pair<unsigned int, unsigned int> output = {backend->resolution.second/wh.second, backend->resolution.first/wh.first};
	switch (inputMode){
		case 0:
			return output;
		case 1:
			return {output.first-1, output.second};
		case 2:{
			std::pair<int, int> newOutput = {output.first-3, (int)output.second};
			if(newOutput.first <= 0){
				inputMode = 1;
				return getRowsCols();
		       }else{
				return {output.first-3, output.second};
		       }
		       }
	}
}

void termWindow::update(){
	if(backend && backend->opened)
		backend->update();
}

std::vector<std::string> termWindow::getKeysPressed(){
	if(!backend || !backend->opened){
		return {};
	}
	switch (inputMode){
		case 0:
			return backend->getKeysPressed();
		case 1:
		case 2:
		default:
			if(osk)
				return osk->getInput(backend->getKeysPressed());
			else
				return backend->getKeysPressed();
	}
}
