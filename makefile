all: sdl_trigger.cpp
	g++ sdl_trigger.cpp `sdl2-config --cflags --libs` -std=c++11 -lSDL2_ttf -o sdl_trigger