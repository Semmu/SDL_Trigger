#include <iostream>
#include "util.h"
#include "sdl_trigger.h"

int main(int argc, char const *argv[])
{
    Trigger::KeyState keyState{SDLK_ESCAPE, true};

    std::cout << "hello from main" << std::endl;
    std::cout << currentTime() << std::endl;
    std::cout << keyState << std::endl;

    return 0;
}
