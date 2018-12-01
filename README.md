# SDL_Trigger

**Super easy-to-use keyboard shortcut library for SDL2.**

![SDL_Trigger Visual Demo](demo.gif)

Just define your keyboard shortcuts with the corresponding callbacks and let SDL_Trigger handle the rest!

## Features:

 * Super easy-to-use
 * Handles single key and compound keyboard shortcuts
 * Can handle any type of callback that [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function) supports
 * Can group shortcuts to enable/disable them on-demand

## Requirements

 * SDL2 development packages
 * C++11 enabled compiler

## How To Use

**0. Add to your project:**

Copy the `include/sdl_trigger.h` and `src/sdl_trigger.cpp` files into your project directory, configure your IDE/build system/etc. to compile and link the necessary files, and `#include "sdl_trigger.h"` where you want to use it.

**1. Define your keybindings:**

Single key shortcut:
```cpp
Trigger::on(SDLK_LEFT, [&player]() {
    std::cout << "Left pressed, moving player left." << std::endl;
    player.moveLeft();
});
```

Compound shortcut:
```cpp
Trigger::on({SDLK_RCTRL, SDLK_a}, [&items]() {
    std::cout << "(Right)Ctrl + A pressed, selecting all items." << std::endl;
    items.selectAll();
});
```

(The second parameters with the syntax `[](){}` used here are [C++11 lambda functions,](https://en.cppreference.com/w/cpp/language/lambda) but other types of callbacks are supported as well. More details below.)

**(Optional) 1+0.5. Create groups for your keybindings:**

If you want to enable/disable certain triggers while your application is running, you can create a `Trigger::Group` for them, add your keybindings to the new group, and toggle its state when you want!

```cpp
Trigger::Group mainMenuGroup;

mainMenuGroup.on(SDLK_s, [&stateManager]() {
    stateManager.switchTo(stateManager::STATE_SINGLE_PLAYER);
});

mainMenuGroup.on(SDLK_m, [&stateManager]() {
    stateManager.switchTo(stateManager::STATE_MULTIPLAYER);
});



// somewhere in the state manager...

void StateManager::switchTo(State newState) {
    // ...

    if (newState == STATE_MAIN_MENU) {
        mainMenuGroup.enable(); // since we are transitioning to the main menu, we need to enable those keybindings
    } else {
        mainMenuGroup.disable(); // the next state is not the main menu, we don't want to trigger anything related to it
    }

    // ...
}
```

In our hypotethical game, we have a [finite state machine](http://gameprogrammingpatterns.com/state.html) to keep record of our applications' state, and it has access to our keybinding groups, so it can enable/disable them when needed.

You can read more about groups below later.

(By the way I really recommend the [Game Programming Patterns](http://gameprogrammingpatterns.com/) book, it's free and really awesome!)

**2. Pass `SDL_Event`s to SDL_Trigger:**

In your event loop:

```cpp
while (SDL_PollEvent(&e) != 0)
{
    Trigger::processEvent(e);

    // ...
}
```

Just pass every `SDL_Event` to SDL_Trigger so it can detect keypresses.

**3. Profit!**

SDL_Trigger maintains an internal state of your shortcuts and the keys pressed, and whenever a keyboard shortcut is fulfilled, SDL_Trigger calls the provided callback. Simple, huh?

## Types of keyboard shortcuts

Right now SDL_Trigger supports 2 types of shortcuts: single key and compound ones.

 * **Single key shortcuts:**

   Just pass any key, like `SDLK_a` for `Trigger::on`, and every time that key is pressed, the callback will be called.

 * **Compound key shortcuts:**

   You can pass an arbitrary long list of `SDL_Keycode`s for `Trigger::on`, eg. `{SDLK_RCTRL, SDLK_RSHIFT, SDLK_SPACE}`, and it will call the callback **every time those keys are pressed in any order, but without other keys.** That means no other key is allowed to be pressed during the process, because that will invalidate the shortcuts' state and the callback won't be called. But the order of the pressed keys in the shortcut does not matter. So when you are holding down every key of a shortcut doesn't necessarily mean that it is going to be activated, only if no other key was pressed during the process! (There is a visual demo provided, play with it to see how it behaves.)

## Type of callbacks

SDL_Trigger only supports one type of callback:

```cpp
using Callback = std::function<void(void)>;
```

That means only functions with `void` return type and no arguments are supported. If you want to call functions with arguments, you could use [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind) or simply define a lambda and do anything you want in it, eg. call other functions with parameters.

Some examples:
  
 * **Calling a global function:**
 
   ```cpp
   void globalFunction() {
       std::cout << "global function called." << std::endl;
   }

   Trigger::on(SDLK_g, globalFunction);
   ```

 * **Calling a lambda:**
 
   ```cpp
   Trigger::on(SDLK_l, []() {
       std::cout << "lambda called." << std::endl;
   });
   ```

   (Of course this [lambda](https://en.cppreference.com/w/cpp/language/lambda) could capture any variable or object from the outside.)

 * **Calling a functor:**
 
   ```cpp
   // the functor type
   struct Functor_t {
       std::string someString;

       Functor_t(std::string string) : someString{string} {
           // nothing
       }

       void operator()(void) {
           std::cout << "Functor called with someString = " << someString << std::endl;
       }
   };
   
   // the functor instance
   Functor_t aFunctor("A functor");

   // calling it on keypress
   Trigger::on(SDLK_f, aFunctor);
   ```

   A functor is just an object which has the `operator()` defined. [Click for an article about functors.](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html)
   
 * **Calling an object method:**
 
   ```cpp
   // the functor type
   struct Object {
       std::string someString;

       Object(std::string string) : someString{string} {
           // nothing
       }

       void printString(void) {
           std::cout << "Object printing string, someString = " << someString << std::endl;
       }
   };
   
   // the functor instance
   Object anObject("Hi, I'm an object");

   // calling it on keypress
   Trigger::on(SDLK_f, [&anObject]() {
       anObject.printString();
   });
   ```
   
   The important piece here is to capture the object (by reference for better performance) while defining the lambda function.

There are other ways to define callbacks, for example using [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind), but the simplest method is just to define a lambda with no arguments and no return value, and do anything you want inside that lambda.

## Trigger groups and working with them

Trigger groups can be used to define keyboard shortcuts and toggle them on-demand. Think of shortcuts which shouldn't always be enabled, because they are tied to a context/state, like in a menu or on the settings screen or in gameplay. `Trigger::Group`s are the solution to this problem!

**1. First, create the group to hold the triggers:**

```cpp
Trigger::Group gameplayGroup;
```

This creates your group, which is **enabled** by default.

**2. Now add triggers to the group:**

```cpp
gameplayGroup.on(SDLK_LEFT, [&player]() {
    player.moveLeft();
});

gameplayGroup.on({SDLK_LCTRL, SDLK_r}, [&player]() {
    player.reloadGun();
});
```

You can add single key and compound keyboard triggers to groups the same way you did with global triggers. To be honest, SDL_Trigger creates a group named `globalGroup` automatically for you, and when you call `Trigger::on` to define triggers, it just passes those arguments to the global group. So effectively you are always using groups.

**3. Enable/disable certain groups when you need:**

Groups have an `isEnabled` boolean variable to indicate their state, but for easier usage, they also have simple functions to change their state:

```cpp
gameplayGroup.enable();  // enables the triggers
gameplayGroup.disable(); // disables the triggers
gameplayGroup.toggle();  // toggles its state
```

**You should call these functions instead of manipulating the `isEnabled` variable directly,** since these methods do other stuff as well (for example reseting every keybinding in the group when it gets disabled)

Groups can be created and destroyed anywhere in the code. There is an internal `std::vector<Trigger::Group*>` container defined to hold these groups and its contents are updated every time a group is created or destroyed. (Destroyed here means it goes out of scope and its destructor is called.)

## Visual Demo

The visual demo provided (gif above) shows the behaviour of the library when pressing single or compound keyboard shortcuts. It contains a simple "maze" with yellow "coins" in it, and the blue colored "player" can be controlled with the arrow keys to collect the "coins." In the background a simple log of the keypresses is scrolling. The keyboard shortcuts are visible on the screen with the buttons also being pressed if the corresponding key is physically down (and the shortcut is not invalidated.)

**Requirements:**

 * This demo requires the SDL2_ttf development packages as well
 
**Building and running:**

 * Build: `make`
 * Run: `make run` or execute `./bin/demo`

## TODO

 * **Handling of generic modifier keys without specifying which side (left or right) it is on.**
 
   Right now, since SDL_Trigger is based on the built-in `SDL_Keycode` type, there is no way to define a generic keyboard shortcut with for example simply the Control button, no matter which side it is on. If you want to support both Control buttons, you need to define a trigger with `SDLK_LCTRL` and `SDLK_RCTRL` as well. It sucks and SDL_Trigger should somehow define new types or values for these modifier keys and handle it internally.
 
 * **Shortcut reset on press.**
 
   This should make it possible to "reset" a keyboard shortcut if it's activated, so every key in it should have to be released and pressed again for its next activation. (You couldn't just spam one key and activate the compound shortcut again.)

 * **Repeating shortcuts.**
 
   This should make it possible to tell SDL_Trigger to process repeating keypresses when holding down a key and call callbacks if needed.

 * **Strict keypress order mode.**
 
   This should make it possible to only activate keyboard shortcuts if the keys were pressed in the exact same order as the trigger is defined.

(For the last three items a flag system could be used, a third parameter for `Trigger::on` could be `OR`'d flags to define behaviour.)

## License

MIT.