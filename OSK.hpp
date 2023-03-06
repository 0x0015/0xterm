#pragma  once
#include <iostream>

#include "windowBackend.hpp"

class OSK{
public:
	virtual ~OSK(){}
	windowBackend* backend;
	virtual std::vector<std::string> getInput(std::vector<std::string> inputs) = 0;
	virtual void draw() = 0;
};
