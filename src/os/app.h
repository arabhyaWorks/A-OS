#pragma once

#include <string>
#include "event_bus.h"

namespace AOS {

// Forward declaration
class Renderer;

/**
 * App - Base class for all A-OS applications
 *
 * All applications inherit from this and implement the lifecycle methods.
 * Similar to Android Activity or iOS ViewController lifecycle.
 *
 * Lifecycle flow:
 *   onStart() -> onResume() -> [running] -> onPause() -> onStop()
 */
class App {
public:
    virtual ~App() = default;

    // Lifecycle methods
    virtual void onStart() {}       // App is being launched
    virtual void onPause() {}       // App is losing focus
    virtual void onResume() {}      // App is regaining focus
    virtual void onStop() {}        // App is being closed

    // Frame update (called every frame while app is active)
    virtual void update(float deltaTime) {}

    // Rendering (called every frame while app is active)
    virtual void render(Renderer& renderer) {}

    // Event handling
    virtual void onEvent(const Event& event) {}

    // App metadata
    virtual std::string getName() const = 0;
    virtual std::string getIcon() const { return ""; }  // Path to icon asset

protected:
    App() = default;
};

} // namespace AOS
