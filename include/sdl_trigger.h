#ifndef SDL_TRIGGER_H
#define SDL_TRIGGER_H

#include "SDL2/SDL.h"
#include <functional>
#include <vector>

namespace Trigger {
    using Callback = std::function<void(void)>;

    struct KeyState {
        SDL_Keycode key;
        bool isDown;
    };

    struct KeyCombination {
        std::vector<KeyState> keys;

        bool hasKey(SDL_Keycode key) const;
        void markKeyDown(SDL_Keycode key);
        void markKeyUp(SDL_Keycode key);
        void reset();
        bool isFulfilled() const;
    };

    struct Trigger {
        KeyCombination combination;
        Callback callback;
    };
    extern std::vector<Trigger> triggers;

    void on(SDL_Keycode key, Callback callback);
    void on(std::vector<SDL_Keycode> keys, Callback callback);

    void processEvent(SDL_Event &e);
} // namespace Trigger

#endif /* SDL_TRIGGER_H */