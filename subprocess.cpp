#include "subprocess.hpp"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <pty.h>
#include <utmp.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <cstring>

void subprocess::run(const std::vector<std::string>& args){
	pipe(inpipefd);
	pipe(outpipefd);
	int master;
	int fd_master = posix_openpt(O_RDWR | O_NOCTTY);
	int ppid = getpid();
	const char* pts;
	assert(fd_master >= 0);
	std::cout<<"Opened fd_master: "<<fd_master<<std::endl;
	assert(unlockpt(fd_master) >= 0);
	std::cout<<"Unlocked fd_master"<<std::endl;
	assert((pts=ptsname(fd_master)) != NULL);
	std::cout<<"Name of child: "<<pts<<std::endl;

	pid = fork();
	if(pid == -1){
		std::cerr<<"Uncessessful forkpty!"<<std::endl;
		return;
	}
	if(pid == 0){
		//child
		int fd_child = -1;

		close(fd_master);
		fd_child = open(pts, O_RDWR);
		assert(fd_child >= 0 );
		std::cout<<"Opened child (stream "<<fd_child<<")"<<std::endl;

		auto login_tty_res = login_tty(fd_child);
		if(login_tty_res < 0){
			std::cerr<<"got login_tty result: "<<login_tty_res<<"errno: "<<errno<<"("<<std::strerror(errno)<<")"<<std::endl;
			assert(login_tty_res >= 0);
		}

		dup2(outpipefd[0], STDIN_FILENO);
		dup2(inpipefd[1], STDOUT_FILENO);
		dup2(inpipefd[1], STDERR_FILENO);
		//prctl(PR_SET_PDEATHSIG, SIGTERM);

		//assert(::write(STDOUT_FILENO, "|", 1) == 1);

		char* cargs[args.size() + 1];
		for(unsigned int i=0;i<args.size();i++){
			cargs[i] = (char*)malloc(args[i].length());
			strcpy(cargs[i], args[i].c_str());
		}

		cargs[args.size()] = NULL;

		int environc = 0;
		for(char** current = environ; *current; current++){
			environc++;
		}
		
		char* cenvs[environc + 2];
		for(unsigned int i=0;i<environc;i++){	
			cenvs[i] = (char*)malloc(strlen(environ[i])+1);
			strncpy(cenvs[i], environ[i], strlen(environ[i]));
			cenvs[i][strlen(environ[i])] = 0;
		}

		cenvs[environc] = "TERM=xterm\0";
		cenvs[environc+1] = NULL;

		execve(cargs[0], cargs, cenvs);

		for(unsigned int i=0;i<args.size();i++){
			free(cargs[i]);
		}

		running = false;

		exit(1);
	}
	running = true;

	close(outpipefd[0]);
	close(inpipefd[1]);
}

void subprocess::write(const std::string& msg){
	if(!running)
		return;
	::write(outpipefd[1], msg.c_str(), msg.length());
}

std::string subprocess::read(){
	std::string output;
	fcntl(inpipefd[0], F_SETFL, (fcntl(inpipefd[0], F_GETFL)|O_NONBLOCK));
	ssize_t amountRead;
	char buf[1];
	bool lastReadWasNL = false;
	amountRead = ::read(inpipefd[0], buf, 1);
	while(amountRead > 0){
		if(buf[0] == '\n' && !lastReadWasNL){
			lastReadWasNL = true;
			continue;
		}
		if (lastReadWasNL && buf[0] != '\r'){
			output += '\r';
			lastReadWasNL = false;
		}
		output += buf[0];
		amountRead = ::read(inpipefd[0], buf, 1);
	}
	fcntl(inpipefd[0], F_SETFL, (fcntl(inpipefd[0], F_GETFL)&O_NONBLOCK));
	
	return output;
}

void subprocess::kill(){
	if(!running)
		return;
	::kill(pid, SIGKILL);
}

void subprocess::wait(){
	if(!running)
		return;
	waitpid(pid, &status, 0);
}

