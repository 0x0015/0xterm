#pragma once
#include <iostream>

#include "OSK.hpp"

class OSK1 : public OSK{
public:
	OSK1();
	std::vector<std::string> getInput(std::vector<std::string> inputs);
	void draw();
	int selectedChar = 0;
	std::vector<std::pair<std::string, std::string>> lettersSelections = {
		{" backspace ", "\b"},
		{"enter ", "\n"},
		{"tab ", "\t"}
	};
};
