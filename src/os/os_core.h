#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "app_manager.h"
#include "event_bus.h"
#include "ui/renderer.h"
#include "hal/input_manager.h"
#include "hal/audio_manager.h"

namespace AOS {

/**
 * OSCore - Main OS coordinator
 *
 * This is the heart of A-OS. It:
 * - Initializes SDL and creates the window
 * - Creates and manages all core subsystems
 * - Runs the main loop (60 FPS target)
 * - Coordinates frame updates and rendering
 *
 * The main loop is:
 *   1. Poll input
 *   2. Process events
 *   3. Update active app
 *   4. Render active app
 *   5. Cap to 60 FPS
 */
class OSCore {
public:
    OSCore();
    ~OSCore();

    // Initialize the OS
    bool initialize(int width = 1280, int height = 720, bool fullscreen = false);

    // Shutdown the OS
    void shutdown();

    // Run the main loop (blocks until quit)
    void run();

    // Get subsystems (for app registration, etc.)
    AppManager& getAppManager() { return *appManager; }

private:
    // SDL components
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;

    // Core subsystems
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<AppManager> appManager;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<AudioManager> audioManager;

    // Main loop
    bool running;
    Uint64 lastFrameTime;

    void mainLoop();
    float getDeltaTime();
};

} // namespace AOS
