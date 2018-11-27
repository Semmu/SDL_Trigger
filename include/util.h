#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <SDL2/SDL.h>

std::string currentTime();

void fatal(const char *reason);

struct Surface {
    static SDL_PixelFormat* format;

    static void setFormat(SDL_PixelFormat* format);

    static SDL_Surface* create(int width, int height);
    static int colorFor(int r, int g, int b);
    static int colorFor(SDL_Color color);
};


#endif /* UTIL_H */