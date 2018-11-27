#include <iostream>
#include "util.h"
#include "sdl_trigger.h"
#include "graphics.h"
#include "maze.h"

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

    Trigger::on(SDLK_q, [&running]() {
        running = false;
    });

    Trigger::on(SDLK_ESCAPE, [&running]() {
        running = false;
    });

    Trigger::on({SDLK_LCTRL, SDLK_LSHIFT, SDLK_r}, []() {
        Maze.generate();
    });

    Trigger::on({SDLK_LCTRL, SDLK_SPACE}, []() {
        Maze.drop(Maze_t::Tile::COIN);
    });

    Trigger::on(SDLK_UP, []() {
        Maze.moveUp();
    });

    Trigger::on(SDLK_RIGHT, []() {
        Maze.moveRight();
    });

    Trigger::on(SDLK_DOWN, []() {
        Maze.moveDown();
    });

    Trigger::on(SDLK_LEFT, []() {
        Maze.moveLeft();
    });

    combinations.push_back(Combination("Randomize Map", {SDLK_LCTRL, SDLK_LSHIFT, SDLK_r}));
    combinations.push_back(Combination("Drop Coin", {SDLK_LCTRL, SDLK_SPACE}));
    combinations.push_back(Combination("Move Up", {SDLK_UP}));
    combinations.push_back(Combination("Move Right", {SDLK_RIGHT}));
    combinations.push_back(Combination("Move Down", {SDLK_DOWN}));
    combinations.push_back(Combination("Move Left", {SDLK_LEFT}));
    combinations.push_back(Combination("Close This Demo", {SDLK_q}));
    combinations.push_back(Combination("Also Close This Demo", {SDLK_ESCAPE}));

    Maze.generate();

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

        KeyPressLog::autoScroll();

        SDL_FillRect(surface, NULL, Surface::colorFor(0, 0, 0));

        int recordY = HEIGHT;
        int i = 0;
        for (auto& record : KeyPressLog::records) {

            recordY -= 20;

            Uint8 color = 100 / KeyPressLog::maxRecords * (KeyPressLog::maxRecords - i);
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

        SDL_Surface *githubSurface = Surface::ofText("https://github.com/Semmu/SDL_Trigger");
        SDL_Rect githubRect;
        githubRect.x = WIDTH - githubSurface->w - 10;
        githubRect.y = HEIGHT - githubSurface->h - 10;
        SDL_BlitSurface(githubSurface, NULL, surface, &githubRect);
        SDL_FreeSurface(githubSurface);

        SDL_Surface* mazeSurface = Maze.render();
        SDL_Rect mazeRect;
        mazeRect.y = HEIGHT - mazeSurface->h - 50;
        mazeRect.x = WIDTH - mazeSurface->w - 30;
        SDL_BlitSurface(mazeSurface, NULL, surface, &mazeRect);

        SDL_Surface* levelSurface = Surface::ofText((std::string("Level #") + std::to_string(Maze.level) + std::string(" - Coins: ") + std::to_string(Maze.coinsCollected)).c_str());
        SDL_Rect levelRect;
        levelRect.x = mazeRect.x + + (mazeSurface->w - levelSurface->w) / 2;
        levelRect.y = mazeRect.y - 20;
        SDL_BlitSurface(levelSurface, NULL, surface, &levelRect);
        SDL_FreeSurface(levelSurface);

        SDL_Rect combinationRect;
        combinationRect.x = 10;
        combinationRect.y = 10;
        for (auto &combination : combinations) {
            SDL_BlitSurface(combination.render(), NULL, surface, &combinationRect);

            combinationRect.y += combination.surface->h + 10;
        }

        SDL_UpdateWindowSurface(window);

        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
