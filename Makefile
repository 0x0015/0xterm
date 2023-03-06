OBJS	= main.cpp.o\
	  termWindow.cpp.o\
	  libtmt/tmt.c.o\
	  subprocess.cpp.o\
	  onscreenKeyboard1.cpp.o

OUT	= main
CXX	= g++
CC      = gcc
BUILD_CXX_FLAGS	 = -std=c++11 -g
BULID_CC_FLAGS   =
LINK_OPTS	 = -lutil

sdl2: LINK_OPTS += -lSDL2 -lSDL2_ttf
sdl2: BUILD_CXX_FLAGS += -DUSE_SDL2
sdl2: OBJS += SDL2Backend.cpp.o
sdl2: $(OBJS) SDL2Backend.cpp.o
	$(CXX) $(OBJS) -o $(OUT) $(LINK_OPTS)

sdl: LINK_OPTS += -lSDL -lSDL_ttf
sdl: BUILD_CXX_FLAGS += -DUSE_SDL
sdl: OBJS += SDLBackend.cpp.o
sdl: $(OBJS) SDLBackend.cpp.o
	$(CXX) $(OBJS) -o $(OUT) $(LINK_OPTS)


all: $(OBJS)
	$(CXX) $(OBJS) -o $(OUT) $(LINK_OPTS)

%.cpp.o: %.cpp
	$(CXX) $< $(BUILD_CXX_FLAGS) -g -c -o $@

%.c.o: %.c
	$(CC) $< $(BUILD_CC_FLAGS) -g -c -o $@

clean:
	rm -f $(OBJS) SDL2Backend.cpp.o SDLBackend.cpp.o $(OUT)
