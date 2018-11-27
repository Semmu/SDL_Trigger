#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "sdl_trigger.h"

struct Button {
    Trigger::KeyState* keyState;
    SDL_Surface* surface;

    const int BUTTON_PADDING = 10;
    const int BUTTON_HEIGHT = 10;
    const int BUTTON_DEPTH = 7;

    Button() : keyState{NULL}, surface{NULL} {
        //
    }

    void setKeyStatePtr(Trigger::KeyState* newKeyState);
    SDL_Surface* render();
};

#endif /* GRAPHICS_H */