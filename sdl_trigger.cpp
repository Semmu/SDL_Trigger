#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

const size_t WIDTH = 500;
const size_t HEIGHT = 400;

void DIE(const char *reason)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             "FATAL ERROR",
                             reason,
                             NULL);

    exit(1);
}

namespace Trigger {

    using Action = std::function<void(void)>;

    struct KeyState
    {
        SDL_Keycode key;
        bool isDown;
    };

    struct KeyCombination {
        std::vector<KeyState> keys;

        bool hasKey(SDL_Keycode key) {
            for(size_t i = 0; i < keys.size(); i++) {
                if (keys[i].key == key) {
                    return true;
                }
            }

            return false;
        }

        void markKeyDown(SDL_Keycode key) {
            for(size_t i = 0; i < keys.size(); i++) {
                if (keys[i].key == key) {
                    keys[i].isDown = true;
                }
            }
        }

        void markKeyUp(SDL_Keycode key) {
            for(size_t i = 0; i < keys.size(); i++) {
                if (keys[i].key == key) {
                    keys[i].isDown = false;
                }
            }
        }

        void reset() {
            for(size_t i = 0; i < keys.size(); i++) {
                keys[i].isDown = false;
            }
        }

        bool isFulfilled() {
            for(size_t i = 0; i < keys.size(); i++) {
                if (!keys[i].isDown) {
                    return false;
                }
            }

            return true;
        }
    };

    struct Trigger {
        KeyCombination combination;
        Action action;
    };

    std::vector<Trigger> triggers;

    void on(std::vector<SDL_Keycode> keys, Action action) {

        KeyCombination keyCombination;
        for(const auto key : keys) {
            keyCombination.keys.push_back({key, false});
        }

        Trigger trigger;
        trigger.combination = keyCombination;
        trigger.action = action;

        triggers.push_back(trigger);
    }

    KeyCombination& referenceOf(std::vector<SDL_Keycode>) {

    }

    void processEvent(SDL_Event& e) {
        SDL_Keycode key;

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            std::cout << std::endl;
            key = e.key.keysym.sym;

            for (auto& trigger : triggers) {
                if (trigger.combination.hasKey(key)) {
                    trigger.combination.markKeyDown(key);
                } else {
                    trigger.combination.reset();
                }
            }

            for (auto& trigger : triggers) {
                if (trigger.combination.isFulfilled()) {
                    trigger.action();
                }
            }
        } else if (e.type == SDL_KEYUP) {
            key = e.key.keysym.sym;

            for (auto& trigger : triggers) {
                if (trigger.combination.hasKey(key)) {
                    trigger.combination.markKeyUp(key);
                }
            }
        }
    }
}

int main(int argc, char *args[])
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1)
        DIE(TTF_GetError());
    atexit(SDL_Quit);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;

    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                WIDTH, HEIGHT);

    surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF,
                                   0xFF000000);
    // SDL_ShowCursor(SDL_DISABLE);
    // SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
    SDL_RaiseWindow(window);

    TTF_Font *font = TTF_OpenFont("./cft.ttf", 16);
    if (font == NULL)
        DIE(TTF_GetError());

    Trigger::on({SDLK_RSHIFT}, []() {
        std::cout << " --------------------------- RIGHT SHIFT" << std::endl;
    });

    Trigger::on({SDLK_RCTRL, SDLK_a}, []() {
        std::cout << " --------------------------- COMBO " << std::endl;
    });




    bool running = true;

    Trigger::on({SDLK_q}, [&running]() {
        running = false;
    });

    Trigger::on({SDLK_ESCAPE}, [&running]() {
        running = false;
    });

    SDL_Event e;
    std::stringstream textOnScreen;
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
                        textOnScreen.str("");
                        textOnScreen.clear();
                        textOnScreen << "down...: " << SDL_GetKeyName(e.key.keysym.sym);
                    }
                } break;

                case SDL_KEYUP: {
                    if (e.key.repeat == 0)
                    {
                        textOnScreen.str("");
                        textOnScreen.clear();
                        textOnScreen << "  ...up: " << SDL_GetKeyName(e.key.keysym.sym);
                    }
                }

                default: break;
            }
        }

        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, textOnScreen.str().c_str(), {150, 150, 150});
        SDL_Rect r;
        r.x = 10;
        r.y = 10;
        SDL_BlitSurface(textSurface, NULL, surface, &r);
        SDL_FreeSurface(textSurface);

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    return 0;
}
