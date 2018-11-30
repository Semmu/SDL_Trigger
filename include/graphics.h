#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "sdl_trigger.h"

struct Button {
    std::vector<SDL_Keycode> keys;
    size_t index;
    bool isEnabled;

    Trigger::KeyState* keyState = NULL;
    SDL_Surface* surface = NULL;

    const int BUTTON_PADDING = 7;
    const int BUTTON_HEIGHT = 7;
    const int BUTTON_DEPTH = 4;

    Button(std::vector<SDL_Keycode> keys, size_t index);

    static Button forCombinationKey(std::vector<SDL_Keycode> keys, size_t index);

    void findKeyState();

    SDL_Surface* render();
};

struct Combination {
    std::string description;
    std::vector<Button> buttons;
    SDL_Surface *surface;

    const int DESCRIPTION_WIDTH = 150;
    const int BUTTON_DISTANCE = 7;

    Combination(std::string description, std::vector<SDL_Keycode> keys);
    SDL_Surface* render();
};

extern std::vector<Combination> combinations;

#endif /* GRAPHICS_H */