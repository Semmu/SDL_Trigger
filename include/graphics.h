#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "sdl_trigger.h"

struct Button {
    std::vector<SDL_Keycode> keys;
    size_t index;

    Trigger::KeyState* keyState = NULL;
    SDL_Surface* surface = NULL;

    const int BUTTON_PADDING = 10;
    const int BUTTON_HEIGHT = 10;
    const int BUTTON_DEPTH = 7;

    Button(std::vector<SDL_Keycode> keys, size_t index);

    static Button forCombinationKey(std::vector<SDL_Keycode> keys, size_t index);

    void findKeyState();

    SDL_Surface* render();
};

struct Combination {
    std::string description;
    std::vector<Button> buttons;
    SDL_Surface *surface;

    const int BUTTON_DISTANCE = 10;

    Combination(std::string description, std::vector<SDL_Keycode> keys);
    SDL_Surface* render();
};

extern std::vector<Combination> combinations;

#endif /* GRAPHICS_H */