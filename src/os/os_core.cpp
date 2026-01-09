#include "os_core.h"
#include <iostream>

namespace AOS {

OSCore::OSCore()
    : window(nullptr)
    , sdlRenderer(nullptr)
    , running(false)
    , lastFrameTime(0)
{
}

OSCore::~OSCore() {
    shutdown();
}

bool OSCore::initialize(int width, int height, bool fullscreen) {
    std::cout << "=== A-OS Initializing ===" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow(
        "A-OS",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        windowFlags
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    sdlRenderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!sdlRenderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create subsystems
    renderer = std::make_unique<Renderer>(window, sdlRenderer);
    appManager = std::make_unique<AppManager>();
    inputManager = std::make_unique<InputManager>();
    audioManager = std::make_unique<AudioManager>();

    audioManager->initialize();

    std::cout << "=== A-OS Initialized ===" << std::endl;

    return true;
}

void OSCore::shutdown() {
    std::cout << "=== A-OS Shutting Down ===" << std::endl;

    if (audioManager) {
        audioManager->shutdown();
    }

    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void OSCore::run() {
    if (!window || !sdlRenderer) {
        std::cerr << "Cannot run: OS not initialized" << std::endl;
        return;
    }

    running = true;
    lastFrameTime = SDL_GetPerformanceCounter();

    std::cout << "=== A-OS Running ===" << std::endl;

    // Publish startup event
    EventBus::getInstance().publish(Event(EventType::SYSTEM_STARTUP));

    // Main loop
    while (running) {
        mainLoop();
    }

    std::cout << "=== A-OS Stopped ===" << std::endl;
}

void OSCore::mainLoop() {
    // 1. Poll input
    inputManager->pollInput();
    if (inputManager->isQuitRequested()) {
        running = false;
        return;
    }

    // 2. Process events
    EventBus::getInstance().processEvents();

    // 3. Update active app
    float deltaTime = getDeltaTime();
    appManager->update(deltaTime);

    // 4. Render
    renderer->clear(Color::Black());
    appManager->render(*renderer);
    renderer->present();

    // 5. Frame rate cap (60 FPS target via VSYNC)
    // VSYNC is enabled in renderer creation, so SDL handles this
}

float OSCore::getDeltaTime() {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();

    float deltaTime = (currentTime - lastFrameTime) / (float)frequency;
    lastFrameTime = currentTime;

    return deltaTime;
}

} // namespace AOS
