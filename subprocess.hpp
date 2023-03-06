#pragma once
#include <iostream>
#include <unistd.h>
#include <vector>

class subprocess{
public:
	void run(const std::vector<std::string>& args);
	pid_t pid = 0;
	int inpipefd[2];
	int outpipefd[2];
	int status;
	bool running = false;
	void write(const std::string& msg);
	std::string read();
	void kill();
	void wait();
	int retcode;
};
