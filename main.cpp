#include <iostream>
extern "C"{
#include "libtmt/tmt.h"
}

#include "termWindow.hpp"
#include "windowBackend.hpp"
#include "subprocess.hpp"
#include "onscreenKeyboard1.hpp"
#if defined(USE_SDL2) && defined(USE_SDL)
#error "Must chose one backend!"
#endif
#ifdef USE_SDL2
#include "SDL2Backend.hpp"
#endif
#ifdef USE_SDL
#include "SDLBackend.hpp"
#endif

void callback(tmt_msg_t m, TMT* vt, const void* a, void* p);

int main(int argc, char** argv){
	termWindow window;
#ifdef USE_SDL2
	window.backend = new SDL2Backend();
#endif
#ifdef USE_SDL
	window.backend = new SDLBackend();
#endif
	window.backend->resolution = {320,240};
	window.fontSize = 10;
	window.backend->fontSize = 10;
	window.inputMode = 1;
	window.osk = new OSK1();
	window.osk->backend = window.backend;

	if(!window.backend->open()){
		std::cout<<"Failed to open window"<<std::endl;
		return -1;
	}

	std::pair<unsigned int, unsigned int> termSize = window.getRowsCols();
	if(termSize.first == 0 || termSize.second == 0){
		termSize = {2, 10};
	}

	TMT* vt = tmt_open(termSize.first, termSize.second, callback, &window, NULL);
	if(!vt){
		std::cerr<<"Unable to open terminal!"<<std::endl;
		return 1;
	}

	std::string process = "/bin/sh";
	subprocess subproc;
	subproc.run({process, "-i"});

	//tmt_write(vt, "\033[1mhello, world (in bold!)\033[0m", 0);
	//tmt_write(vt, "Hello World!\n", 0);

	int lastTime = 0;
	while(window.backend->opened){
		if(!subproc.running)
			break;
		
		std::vector<std::string> keysPressed = window.getKeysPressed();
		for(unsigned int i=0;i<keysPressed.size();i++){
			subproc.write(keysPressed[i]);
		}

		std::string gotText = subproc.read();
		tmt_write(vt, gotText.c_str(), gotText.length());
		
		window.update();
	}

	subproc.kill();
	subproc.wait();

	tmt_close(vt);

	window.close();

	return 0;
}

void callback(tmt_msg_t m, TMT* vt, const void* a, void* p){

    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);
    termWindow* window = (termWindow*)p;

    switch (m){
        case TMT_MSG_BELL://terminal bell
            //nothing for now
            break;
        case TMT_MSG_UPDATE:
            /* the screen image changed; a is a pointer to the TMTSCREEN */	
	    window->updateScreen(vt);

            /* let tmt know we've redrawn the screen */
            tmt_clean(vt);
            break;

        case TMT_MSG_ANSWER:
            /* the terminal has a response to give to the program; a is a
             * pointer to a string */
            //printf("terminal answered %s\n", (const char *)a);//do nothing
            break;

        case TMT_MSG_MOVED:
            /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
            //do nothing printf("cursor is now at %zd,%zd\n", c->r, c->c);
            break;
    }
}

