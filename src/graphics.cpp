#include "graphics.h"
#include "util.h"

void Button::setKeyStatePtr(Trigger::KeyState* newKeyState) {
    keyState = newKeyState;
}

SDL_Surface* Button::render() {
    if (keyState == NULL) {
        throw std::runtime_error("Button::keyState points to NULL, can't render!");
    }

    SDL_Surface* labelSurface = Surface::ofText(SDL_GetKeyName(keyState->key), {90, 120, 50});
    if (labelSurface == NULL) {
        throw std::runtime_error("Could not render label!");
    }

    const int surfaceWitdh = labelSurface->w + 2 * BUTTON_PADDING;
    const int surfaceHeight = labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT;

    SDL_FreeSurface(surface);
    surface = Surface::create(labelSurface->w + 2 * BUTTON_PADDING,
                              labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT);

    SDL_Rect facingSideRect;
    facingSideRect.w = surfaceWitdh;
    facingSideRect.h = surfaceHeight;
    facingSideRect.x = 0;
    facingSideRect.y = (keyState->isDown ? BUTTON_DEPTH : 0);
    SDL_FillRect(surface, &facingSideRect, Surface::colorFor(120, 150, 70));

    SDL_Rect outlineRect;
    outlineRect.w = surfaceWitdh - 2;
    outlineRect.h = labelSurface->h + 2 * BUTTON_PADDING - 2;
    outlineRect.x = 1;
    outlineRect.y = (keyState->isDown ? BUTTON_DEPTH : 0) + 1;
    SDL_FillRect(surface, &outlineRect, Surface::colorFor(230, 250, 180));

    SDL_Rect topRect;
    topRect.w = surfaceWitdh - 4;
    topRect.h = surfaceHeight - BUTTON_HEIGHT - 4;
    topRect.x = 2;
    topRect.y = (keyState->isDown ? BUTTON_DEPTH : 0) + 2;
    SDL_FillRect(surface, &topRect, Surface::colorFor(170, 210, 100));

    SDL_Rect labelRect;
    labelRect.x = BUTTON_PADDING;
    labelRect.y = BUTTON_PADDING + (keyState->isDown ? BUTTON_DEPTH : 0);
    SDL_BlitSurface(labelSurface, NULL, surface, &labelRect);
    SDL_FreeSurface(labelSurface);

    return surface;
}
