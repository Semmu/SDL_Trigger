#include "sdl_trigger.h"
#include <iostream>

namespace Trigger {
    std::ostream &operator<<(std::ostream &stream, const Trigger::KeyState &keyState) {
        return (stream << "Key " << SDL_GetKeyName(keyState.key) << " is " << (keyState.isDown ? "DOWN" : "UP"));
    }
} // namespace Trigger
