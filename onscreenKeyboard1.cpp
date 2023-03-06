#include "onscreenKeyboard1.hpp"
#include <ctype.h>
#include <algorithm>

OSK1::OSK1(){
	std::vector<std::pair<std::string, std::string>> endLetters = lettersSelections;
	lettersSelections.clear();
	std::string letters = "abcdefghijklmnopqrstuvwxyz";
	for(unsigned int i=0;i<letters.size();i++){
		std::string justLetter;
		justLetter += letters[i];
		lettersSelections.push_back({justLetter, justLetter});
	}
	std::transform(letters.begin(), letters.end(), letters.begin(), ::toupper);
	for(unsigned int i=0;i<letters.size();i++){
		std::string justLetter;
		justLetter += letters[i];
		lettersSelections.push_back({justLetter, justLetter});
	}
	letters = "0123456789\"\'!@#$%^&*(){}[]/=?+s-_,.<>\\";
	for(unsigned int i=0;i<letters.size();i++){
		std::string justLetter;
		justLetter += letters[i];
		lettersSelections.push_back({justLetter, justLetter});
	}
	for(const auto& o : endLetters){
		lettersSelections.push_back(o);
	}
}

std::vector<std::string> OSK1::getInput(std::vector<std::string> inputs){
	std::vector<std::string> output;
	for(const auto& o : inputs){
		if(o == TMT_KEY_LEFT){
			selectedChar--;
			if(selectedChar < 0){
				selectedChar += lettersSelections.size();
			}
		}else if(o == TMT_KEY_RIGHT){
			selectedChar++;
			if(selectedChar >= lettersSelections.size()){
				selectedChar -= lettersSelections.size();
			}
		}else if(o == TMT_KEY_DOWN){
			selectedChar-=10;
			if(selectedChar < 0){
				selectedChar += lettersSelections.size();
			}
		}else if(o == TMT_KEY_UP){
			selectedChar+=10;
			if(selectedChar >= lettersSelections.size()){
				selectedChar -= lettersSelections.size();
			}
		}else if(o == "\b"){
			output.push_back("\b");
		}else{
			output.push_back(lettersSelections[selectedChar].second);
		}
	}
	draw();
	return output;
}

void OSK1::draw(){
	unsigned int usedWidth = 0;
	auto letterWH = backend->getFontCharSize();
	unsigned int ydrawPos = backend->resolution.second-letterWH.second;
	textModifiers mods;
	unsigned int charsInScreen = backend->resolution.first/letterWH.first;
	int currentlyDrawing = selectedChar - (charsInScreen/2);
	if(currentlyDrawing < 0)
		currentlyDrawing += lettersSelections.size();
	while(usedWidth < backend->resolution.first){
		std::string& text = lettersSelections[currentlyDrawing].first;
		if(currentlyDrawing == selectedChar){
			mods.bold = true;
			mods.foreground = {0,0,0,0};
			mods.background = {255,255,255,0};
		}else{
			mods.bold = true;
			mods.background = {0,0,0,0};
			mods.foreground = {255,255,255,0};
		}
		if(usedWidth + (text.length() * letterWH.first) > backend->resolution.first){
			break;
		}
		backend->drawText(text, {usedWidth, ydrawPos}, mods);
		usedWidth += text.length() * letterWH.first;
		currentlyDrawing++;
		if(currentlyDrawing >= lettersSelections.size()){
			currentlyDrawing = 0;
		}
	}
	backend->updateWindow();
}
