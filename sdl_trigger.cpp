#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <deque>
#include <ctime>
#include <iomanip>

const size_t WIDTH = 640;
const size_t HEIGHT = 480;

TTF_Font* font;

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
        trigger.action = std::move(action);

        triggers.push_back(trigger);
    }

    std::vector<KeyState>& referenceOf(std::vector<SDL_Keycode> keys) {
        for(auto& trigger : triggers) {
            bool matches = true;

            if (trigger.combination.keys.size() != keys.size()) {
                matches = false;
            }

            if (matches) {
                for (size_t i = 0; i < keys.size(); i++) {
                    if (trigger.combination.keys[i].key != keys[i]) {
                        matches = false;
                    }
                }

                if (matches) {
                    return trigger.combination.keys;
                }
            }
        }

        throw std::runtime_error("::referenceOf error");
    }

    void processEvent(SDL_Event& e) {
        SDL_Keycode key;

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
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

std::string currentTime() {
    std::stringstream stream;
    std::time_t currentTime = std::time(nullptr);
    std::tm calendarTime = *std::localtime(&currentTime);
    stream << std::put_time(&calendarTime, "%T");
    return stream.str();
}

struct Surface {
    static SDL_Surface* etalon;

    static void setEtalon(SDL_Surface* surface) {
        etalon = surface;
    }

    static SDL_Surface* create(int width, int height) {
        if (etalon == nullptr) {
            throw std::runtime_error("Surface::etalon not set!");
        }

        SDL_Surface* newSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        SDL_Surface* optimizedSurface = SDL_ConvertSurface(newSurface, etalon->format, 0);
        SDL_FreeSurface(newSurface);

        return optimizedSurface;
    }

    static int colorFor(int r, int g, int b) {
        if (etalon == nullptr) {
            throw std::runtime_error("Surface::etalon not set!");
        }

        return SDL_MapRGB(etalon->format, r, g, b);
    }
};
SDL_Surface* Surface::etalon = nullptr;

struct Button {
    Trigger::KeyState& keyState;
    SDL_Surface* surface;

    const int BUTTON_PADDING = 10;
    const int BUTTON_HEIGHT = 10;
    const int BUTTON_DEPTH = 8;

    Button(Trigger::KeyState& keyState) : keyState{keyState}, surface{NULL} {
        // nothing
    }

    SDL_Surface* render() {
        SDL_Surface *labelSurface = TTF_RenderText_Solid(font, SDL_GetKeyName(keyState.key), {90, 120, 50});

        const int surfaceWitdh = labelSurface->w + 2 * BUTTON_PADDING;
        const int surfaceHeight = labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT;

        SDL_FreeSurface(surface);
        surface = Surface::create(labelSurface->w + 2 * BUTTON_PADDING,
                                  labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT);

        SDL_Rect facingSideRect;
        facingSideRect.w = surfaceWitdh;
        facingSideRect.h = surfaceHeight;
        facingSideRect.x = 0;
        facingSideRect.y = (keyState.isDown ? BUTTON_DEPTH : 0);
        SDL_FillRect(surface, &facingSideRect, Surface::colorFor(120, 150, 70));

        SDL_Rect outlineRect;
        outlineRect.w = surfaceWitdh - 2;
        outlineRect.h = labelSurface->h + 2 * BUTTON_PADDING - 2;
        outlineRect.x = 1;
        outlineRect.y = (keyState.isDown ? BUTTON_DEPTH : 0) + 1;
        SDL_FillRect(surface, &outlineRect, Surface::colorFor(230, 250, 180));

        SDL_Rect topRect;
        topRect.w = surfaceWitdh - 4;
        topRect.h = surfaceHeight - BUTTON_HEIGHT - 4;
        topRect.x = 2;
        topRect.y = (keyState.isDown ? BUTTON_DEPTH : 0) + 2;
        SDL_FillRect(surface, &topRect, Surface::colorFor(170, 210, 100));

        SDL_Rect labelRect;
        labelRect.x = BUTTON_PADDING;
        labelRect.y = BUTTON_PADDING + (keyState.isDown ? BUTTON_DEPTH : 0);
        SDL_BlitSurface(labelSurface, NULL, surface, &labelRect);
        SDL_FreeSurface(labelSurface);

        return surface;/**/
        // SDL_FreeSurface(surface);
        // surface = TTF_RenderText_Solid(font, SDL_GetKeyName(SDLK_RETURN), {150, 150, 150});
        // return surface;
    }
};

int main(int argc, char *args[])
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1)
        DIE(TTF_GetError());
    atexit(SDL_Quit);

    SDL_Window *window;
    SDL_Surface *surface;

    window = SDL_CreateWindow("SDL_Trigger Visual Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    Surface::setEtalon(surface);

    // SDL_ShowCursor(SDL_DISABLE);
    // SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
    SDL_RaiseWindow(window);

    font = TTF_OpenFont("./cft.ttf", 16);
    if (font == NULL)
        DIE(TTF_GetError());

    Trigger::on({SDLK_RSHIFT}, []() {
        std::cout << " --------------------------- RIGHT SHIFT" << std::endl;
    });

    Trigger::on({SDLK_RCTRL, SDLK_a}, []() {
        std::cout << " --------------------------- COMBO " << std::endl;
    });

    Trigger::on({SDLK_MODE, SDLK_LSHIFT, SDLK_LCTRL}, []() {
        std::cout << " --------------------------- RECORDING " << std::endl;
    });


    const size_t KEYPRESS_HISTORY_SIZE = 20;
    std::deque<std::string> keyPressLog;


    bool running = true;

    Trigger::on({SDLK_q}, [&running]() {
        running = false;
    });

    Trigger::on({SDLK_ESCAPE}, [&running]() {
        running = false;
    });

    SDL_Event e;
    std::time_t t = std::time(nullptr);

    Button button1(Trigger::referenceOf({SDLK_RCTRL, SDLK_a})[0]);

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
                        keyPressLog.push_front(currentTime() + " [DOWN] " + SDL_GetKeyName(e.key.keysym.sym));
                        keyPressLog.resize(KEYPRESS_HISTORY_SIZE);
                    }
                } break;

                case SDL_KEYUP: {
                    keyPressLog.push_front(currentTime() + " [ UP ] " + SDL_GetKeyName(e.key.keysym.sym));
                    keyPressLog.resize(KEYPRESS_HISTORY_SIZE);
                }

                default: break;
            }
        }

        SDL_FillRect(surface, NULL, Surface::colorFor(0, 0, 0));

        int y = HEIGHT;
        for(size_t i = 0; i < keyPressLog.size(); i++)
        {
            Uint8 lightness = 150 / KEYPRESS_HISTORY_SIZE * (KEYPRESS_HISTORY_SIZE - i);
            y -= 20;

            SDL_Surface *textSurface = TTF_RenderText_Solid(font, keyPressLog[i].c_str(), {lightness, lightness, lightness});
            SDL_Rect r;
            r.x = 10;
            r.y = y;
            SDL_BlitSurface(textSurface, NULL, surface, &r);
            SDL_FreeSurface(textSurface);
        }

        SDL_Surface *clockSurface = TTF_RenderText_Solid(font, currentTime().c_str(), {150, 150, 150});
        SDL_Rect r;
        r.x = WIDTH - clockSurface->w - 10;
        r.y = 10;
        SDL_BlitSurface(clockSurface, NULL, surface, &r);
        SDL_FreeSurface(clockSurface);

        SDL_Rect r2;
        r2.x = 10;
        r2.y = 10;
        SDL_BlitSurface(button1.render(), NULL, surface, &r2);

        SDL_UpdateWindowSurface(window);

        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
