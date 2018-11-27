#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <list>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

std::string currentTime();

void fatal(const char *reason);

struct Surface {
    static SDL_PixelFormat* format;
    static TTF_Font* font;

    static int COLOR_TRANSPARENT;

    static void setFormat(SDL_PixelFormat* format);
    static void setFont(TTF_Font* font);

    static SDL_Surface* create(int width, int height);
    static int colorFor(int r, int g, int b);
    static int colorFor(SDL_Color color);

    static SDL_Surface* ofText(const char* string, SDL_Color color = {150, 150, 150});
};

struct KeyPressLog {
    static std::list<std::string> records;
    static int maxRecords;
    static Uint32 SCROLLS_PER_SECOND;
    static Uint32 lastAutoScroll;

    static void insert(std::string record);
    static void autoScroll();
};


#endif /* UTIL_H */