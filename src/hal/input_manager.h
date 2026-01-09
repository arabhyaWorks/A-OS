#pragma once

#include <SDL2/SDL.h>
#include "os/event_bus.h"

namespace AOS {

/**
 * InputManager - Handles input from keyboard/gamepad and converts to events
 *
 * Responsibilities:
 * - Poll SDL input events
 * - Map keyboard keys to OS events
 * - Map gamepad buttons to OS events (future)
 * - Publish input events to EventBus
 *
 * Desktop simulation mapping:
 * - Arrow keys -> KEY_UP/DOWN/LEFT/RIGHT
 * - Enter -> KEY_SELECT
 * - Escape -> KEY_BACK
 *
 * On Raspberry Pi, this will also handle:
 * - GPIO buttons
 * - USB/Bluetooth controllers
 */
class InputManager {
public:
    InputManager();
    ~InputManager();

    // Poll and process input (called each frame)
    void pollInput();

    // Check if quit was requested (window close, etc.)
    bool isQuitRequested() const { return quitRequested; }

private:
    bool quitRequested;

    void handleKeyDown(SDL_Keycode key);
    void handleKeyUp(SDL_Keycode key);
};

} // namespace AOS
