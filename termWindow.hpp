#pragma  once
#include <iostream>
#include "windowBackend.hpp"

struct TMT;
class OSK;

class termWindow{
public:
	termWindow();
	~termWindow();
	windowBackend* backend;
	bool open();
	bool close();
	bool updateScreen(TMT* vt);
	void update();
	std::pair<unsigned int, unsigned int> getRowsCols();
	virtual std::vector<std::string> getKeysPressed();
	unsigned fontSize = 15;
	uint8_t inputMode = 0;//normal input, onscreen keyboard 1, 2
	OSK* osk;
};
