#include "sdl_trigger.h"
#include <iostream>

namespace Trigger {

    std::vector<Trigger> triggers;

    bool KeyCombination::hasKey(SDL_Keycode key) const {
        for(size_t i = 0; i < keys.size(); i++) {
            if (keys[i].key == key) {
                return true;
            }
        }

        return false;
    }

    void KeyCombination::markKeyDown(SDL_Keycode key) {
        for(size_t i = 0; i < keys.size(); i++) {
            if (keys[i].key == key) {
                keys[i].isDown = true;
            }
        }
    }

    void KeyCombination::markKeyUp(SDL_Keycode key) {
        for(size_t i = 0; i < keys.size(); i++) {
            if (keys[i].key == key) {
                keys[i].isDown = false;
            }
        }
    }

    void KeyCombination::reset() {
        for(size_t i = 0; i < keys.size(); i++) {
            keys[i].isDown = false;
        }
    }

    bool KeyCombination::isFulfilled() const {
        for(size_t i = 0; i < keys.size(); i++) {
            if (!keys[i].isDown) {
                return false;
            }
        }

        return true;
    }

    void on(std::vector<SDL_Keycode> keys, Action action) {
        KeyCombination keyCombination;
        for(const auto key : keys) {
            keyCombination.keys.push_back({key, false});
        }

        Trigger trigger;
        trigger.combination = keyCombination;
        trigger.action = action;

        triggers.push_back(trigger);
    }

    std::vector<KeyState>& referenceOf(std::vector<SDL_Keycode> keys) {
        for(auto& trigger : triggers) {
            if (trigger.combination.keys.size() == keys.size()) {
                bool matches = true;
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

        throw std::runtime_error("Trigger::referenceOf error, combination not found!");
    }

    void processEvent(SDL_Event& e) {
        SDL_Keycode key = e.key.keysym.sym;

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
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
            for (auto& trigger : triggers) {
                if (trigger.combination.hasKey(key)) {
                    trigger.combination.markKeyUp(key);
                }
            }
        }
    }

} // namespace Trigger
