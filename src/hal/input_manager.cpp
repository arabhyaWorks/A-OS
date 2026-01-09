#include "input_manager.h"

namespace AOS {

InputManager::InputManager()
    : quitRequested(false)
{
}

InputManager::~InputManager() {
}

void InputManager::pollInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested = true;
                break;

            case SDL_KEYDOWN:
                handleKeyDown(event.key.keysym.sym);
                break;

            case SDL_KEYUP:
                handleKeyUp(event.key.keysym.sym);
                break;

            default:
                break;
        }
    }
}

void InputManager::handleKeyDown(SDL_Keycode key) {
    auto& eventBus = EventBus::getInstance();

    switch (key) {
        case SDLK_UP:
            eventBus.publish(Event(EventType::KEY_UP));
            break;
        case SDLK_DOWN:
            eventBus.publish(Event(EventType::KEY_DOWN));
            break;
        case SDLK_LEFT:
            eventBus.publish(Event(EventType::KEY_LEFT));
            break;
        case SDLK_RIGHT:
            eventBus.publish(Event(EventType::KEY_RIGHT));
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            eventBus.publish(Event(EventType::KEY_SELECT));
            break;
        case SDLK_ESCAPE:
            eventBus.publish(Event(EventType::KEY_BACK));
            break;
        default:
            break;
    }
}

void InputManager::handleKeyUp(SDL_Keycode key) {
    // Currently not used, but available for future features
    // (e.g., long press detection, hold events)
    (void)key;
}

} // namespace AOS
