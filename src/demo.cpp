#include <iostream>
#include "util.h"
#include "sdl_trigger.h"
#include "graphics.h"

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

    Surface::setFont(font);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
    SDL_RaiseWindow(window);

    SDL_Event e;
    bool running = true;

    Trigger::on({SDLK_q}, [&running]() {
        running = false;
    });

    Trigger::on({SDLK_ESCAPE}, [&running]() {
        running = false;
    });

    Trigger::on({SDLK_RCTRL, SDLK_a}, [](){
    });

    Trigger::on({SDLK_UP}, [](){
    });

    Trigger::on({SDLK_RIGHT}, []() {
    });

    Trigger::on({SDLK_DOWN}, []() {
    });

    Trigger::on({SDLK_LEFT}, []() {
    });

    combinations.push_back(Combination("Select All", {SDLK_RCTRL, SDLK_a}));
    combinations.push_back(Combination("Move Up", {SDLK_UP}));
    combinations.push_back(Combination("Move Right", {SDLK_RIGHT}));
    combinations.push_back(Combination("Move Down", {SDLK_DOWN}));
    combinations.push_back(Combination("Move Left", {SDLK_LEFT}));

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

                case SDL_KEYDOWN: {
                    if (e.key.repeat == 0) {
                        KeyPressLog::insert(currentTime() + " [DOWN] " + SDL_GetKeyName(e.key.keysym.sym));
                    }
                } break;

                case SDL_KEYUP: {
                    KeyPressLog::insert(currentTime() + " [ UP ] " + SDL_GetKeyName(e.key.keysym.sym));
                }

                default: break;
            }
        }

        SDL_FillRect(surface, NULL, Surface::colorFor(0, 0, 0));

        int recordY = HEIGHT;
        int i = 0;
        for (auto& record : KeyPressLog::records) {

            recordY -= 20;

            Uint8 color = 150 / KeyPressLog::maxRecords * (KeyPressLog::maxRecords - i);
            SDL_Surface* recordSurface = Surface::ofText(record.c_str(), {color, color, color});

            SDL_Rect recordRect;
            recordRect.x = 10;
            recordRect.y = recordY;

            SDL_BlitSurface(recordSurface, NULL, surface, &recordRect);
            SDL_FreeSurface(recordSurface);

            i++;
        }

        SDL_Surface *clockSurface = Surface::ofText(currentTime().c_str());
        SDL_Rect clockRect;
        clockRect.x = WIDTH - clockSurface->w - 10;
        clockRect.y = 10;
        SDL_BlitSurface(clockSurface, NULL, surface, &clockRect);
        SDL_FreeSurface(clockSurface);

        SDL_Rect combinationRect;
        combinationRect.x = 10;
        combinationRect.y = 10;
        for (auto &combination : combinations) {
            SDL_BlitSurface(combination.render(), NULL, surface, &combinationRect);

            combinationRect.y += combination.surface->h + 30;
        }

        SDL_UpdateWindowSurface(window);

        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
