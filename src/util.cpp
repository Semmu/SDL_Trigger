#include "util.h"
#include <sstream>
#include <iomanip>

std::string currentTime() {
    std::stringstream stream;
    std::time_t currentTime = std::time(nullptr);
    std::tm calendarTime = *std::localtime(&currentTime);
    stream << std::put_time(&calendarTime, "%T");
    return stream.str();
}

void fatal(const char *reason) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             "FATAL ERROR",
                             reason,
                             NULL);

    exit(1);
}

void Surface::setFormat(SDL_PixelFormat* newFormat) {
    format = newFormat;
    COLOR_TRANSPARENT = SDL_MapRGB(format, 255, 255, 0);
}

void Surface::setFont(TTF_Font* newFont) {
    font = newFont;
}

SDL_Surface* Surface::create(int width, int height) {
    if (format == nullptr) {
        throw std::runtime_error("Surface::format not set!");
    }

    SDL_Surface* newSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_Surface* optimizedSurface = SDL_ConvertSurface(newSurface, format, 0);
    SDL_FreeSurface(newSurface);

    SDL_FillRect(optimizedSurface, NULL, COLOR_TRANSPARENT);
    SDL_SetColorKey(optimizedSurface, SDL_ENABLE, COLOR_TRANSPARENT);

    return optimizedSurface;
}

int Surface::colorFor(int r, int g, int b) {
    if (format == nullptr) {
        throw std::runtime_error("Surface::format not set!");
    }

    return SDL_MapRGB(format, r, g, b);
}

int Surface::colorFor(SDL_Color color) {
    if (format == nullptr) {
        throw std::runtime_error("Surface::format not set!");
    }

    return SDL_MapRGB(format, color.r, color.g, color.b);
}

SDL_Surface* Surface::ofText(const char* string, SDL_Color color) {
    if (font == nullptr) {
        throw std::runtime_error("Surface::font not set!");
    }

    return TTF_RenderText_Solid(font, string, color);
}

SDL_PixelFormat* Surface::format = NULL;
TTF_Font* Surface::font = NULL;
int Surface::COLOR_TRANSPARENT = 0;

std::list<std::string> KeyPressLog::records;
int KeyPressLog::maxRecords = 25;
Uint32 KeyPressLog::lastAutoScroll = 0;
Uint32 KeyPressLog::SCROLLS_PER_SECOND = 5;

void KeyPressLog::insert(std::string record) {
    records.push_front(record);
    records.resize(maxRecords);
}

void KeyPressLog::autoScroll() {
    if (lastAutoScroll + (1000 / SCROLLS_PER_SECOND) < SDL_GetTicks()) {
        insert("");
        lastAutoScroll += (1000 / SCROLLS_PER_SECOND);
    }
}