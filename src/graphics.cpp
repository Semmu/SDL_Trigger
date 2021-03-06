#include "sdl_trigger.h"
#include "graphics.h"
#include "util.h"

Button::Button(std::vector<SDL_Keycode> keys, size_t index) : keys{keys}, index{index} {
    //
}

Button Button::forCombinationKey(std::vector<SDL_Keycode> keys, size_t index) {

    if (!(index < keys.size())) {
        throw std::runtime_error("Key combination index too big while creating Button!");
    }

    Button button(keys, index);

    button.findKeyState();

    return button;
}

void Button::findKeyState() {
    for(auto& group : Trigger::groups) {
        for(auto& trigger : group->triggers) {
            if (trigger.combination.keys.size() == keys.size()) {
                bool matches = true;
                for (size_t i = 0; i < keys.size(); i++) {
                    if (trigger.combination.keys[i].key != keys[i]) {
                        matches = false;
                    }
                }

                if (matches) {
                    keyState = &trigger.combination.keys[index];
                    isEnabled = group->isEnabled;
                    return;
                }
            }
        }
    }

    throw std::runtime_error("Button couldn't find corresponding key combination in Trigger::triggers!");
}

SDL_Surface* Button::render() {

    findKeyState();

    SDL_Color labelColor = isEnabled ? SDL_Color{90, 120, 50} : SDL_Color{90, 90, 90};
    SDL_Surface* labelSurface = Surface::ofText(SDL_GetKeyName(keyState->key), labelColor);
    if (labelSurface == NULL) {
        throw std::runtime_error("Could not render label!");
    }

    const int surfaceWitdh = labelSurface->w + 2 * BUTTON_PADDING;
    const int surfaceHeight = labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT;

    SDL_FreeSurface(surface);
    surface = Surface::create(labelSurface->w + 2 * BUTTON_PADDING,
                              labelSurface->h + 2 * BUTTON_PADDING + BUTTON_HEIGHT);

    int facingSideColor = isEnabled ? Surface::colorFor(120, 150, 70) : Surface::colorFor(100, 100, 100);
    SDL_Rect facingSideRect;
    facingSideRect.w = surfaceWitdh;
    facingSideRect.h = surfaceHeight;
    facingSideRect.x = 0;
    facingSideRect.y = (keyState->isDown ? BUTTON_DEPTH : 0);
    SDL_FillRect(surface, &facingSideRect, facingSideColor);

    int outlineColor = isEnabled ? Surface::colorFor(230, 250, 180) : Surface::colorFor(180, 180, 180);
    SDL_Rect outlineRect;
    outlineRect.w = surfaceWitdh - 2;
    outlineRect.h = labelSurface->h + 2 * BUTTON_PADDING - 2;
    outlineRect.x = 1;
    outlineRect.y = (keyState->isDown ? BUTTON_DEPTH : 0) + 1;
    SDL_FillRect(surface, &outlineRect, outlineColor);

    int topColor = isEnabled ? (keyState->isDown ? outlineColor : Surface::colorFor(170, 210, 100)) : Surface::colorFor(150, 150, 150);
    SDL_Rect topRect;
    topRect.w = surfaceWitdh - 4;
    topRect.h = surfaceHeight - BUTTON_HEIGHT - 4;
    topRect.x = 2;
    topRect.y = (keyState->isDown ? BUTTON_DEPTH : 0) + 2;
    SDL_FillRect(surface, &topRect, topColor);

    SDL_Rect labelRect;
    labelRect.x = BUTTON_PADDING;
    labelRect.y = BUTTON_PADDING + (keyState->isDown ? BUTTON_DEPTH : 0);
    SDL_BlitSurface(labelSurface, NULL, surface, &labelRect);
    SDL_FreeSurface(labelSurface);

    return surface;
}

std::vector<Combination> combinations;

Combination::Combination(std::string description, std::vector<SDL_Keycode> keys) : description{description}, buttons{}, surface{NULL} {
    for (size_t i = 0; i < keys.size(); i++) {
        buttons.push_back(Button(keys, i));
    }
}

SDL_Surface* Combination::render() {
    SDL_Surface *descriptionSurface = Surface::ofText(description.c_str());
    for(size_t i = 0; i < buttons.size(); i++) {
        buttons[i].render();
    }

    int height = buttons[0].surface->h;
    int width = DESCRIPTION_WIDTH + BUTTON_DISTANCE;
    for (auto& button : buttons) {
        width += button.surface->w + BUTTON_DISTANCE;
    }

    SDL_FreeSurface(surface);
    surface = Surface::create(width, height);

    SDL_Rect descriptionRect;
    descriptionRect.x = (DESCRIPTION_WIDTH - descriptionSurface->w) - BUTTON_DISTANCE;
    descriptionRect.y = (height - descriptionSurface->h) / 2;
    SDL_BlitSurface(descriptionSurface, NULL, surface, &descriptionRect);

    SDL_Rect buttonRect;
    buttonRect.x = DESCRIPTION_WIDTH;
    buttonRect.y = 0;
    for (auto& button : buttons) {
        buttonRect.w = button.surface->w;
        buttonRect.h = button.surface->h;

        SDL_BlitSurface(button.render(), NULL, surface, &buttonRect);
        buttonRect.x += button.surface->w + BUTTON_DISTANCE;
    }

    SDL_FreeSurface(descriptionSurface);
    return surface;
}
