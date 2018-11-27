#include <iostream>
#include "util.h"
#include "sdl_trigger.h"
#include <SDL2/SDL_ttf.h>

const size_t WIDTH = 640;
const size_t HEIGHT = 480;

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fatal(SDL_GetError());
    }

    if (TTF_Init() != 0) {
        fatal(TTF_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("SDL_Trigger Visual Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fatal(SDL_GetError());
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        fatal(SDL_GetError());
    }

    Surface::setFormat(surface->format);

    TTF_Font* font = TTF_OpenFont("./cft.ttf", 16);
    if (font == NULL) {
        fatal(TTF_GetError());
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
    SDL_RaiseWindow(window);

    SDL_Event e;
    bool running = true;

    while (running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            Trigger::processEvent(e);
            switch (e.type)
            {
                case SDL_QUIT: {
                    running = false;
                } break;

                default: break;
            }
        }

        SDL_FillRect(surface, NULL, Surface::colorFor(0, 0, 0));
        SDL_UpdateWindowSurface(window);

        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
