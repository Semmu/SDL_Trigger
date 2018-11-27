#ifndef SDL_TRIGGER_H
#define SDL_TRIGGER_H

#include "SDL2/SDL.h"
#include <functional>

namespace Trigger {
    using Action = std::function<void(void)>;

    struct KeyState {
        SDL_Keycode key;
        bool isDown;
    };

    std::ostream &operator<<(std::ostream &stream, const KeyState &keyState);
    
} // namespace Trigger

#endif /* SDL_TRIGGER_H */