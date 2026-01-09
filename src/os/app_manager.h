#pragma once

#include <memory>
#include <vector>
#include <string>
#include "app.h"

namespace AOS {

/**
 * AppManager - Manages application lifecycle and switching
 *
 * Responsibilities:
 * - Register installed apps
 * - Launch and switch between apps
 * - Manage app lifecycle (start, pause, resume, stop)
 * - Only one app active (foreground) at a time
 *
 * This is the core of the "console experience" - apps don't overlap,
 * only one is visible and interactive at any time.
 */
class AppManager {
public:
    AppManager();
    ~AppManager();

    // Register an app (called during OS initialization)
    void registerApp(std::unique_ptr<App> app);

    // Get list of all installed apps (for home screen display)
    const std::vector<App*>& getInstalledApps() const;

    // Launch an app by index
    void launchApp(size_t index);

    // Launch an app by name
    void launchAppByName(const std::string& name);

    // Return to home screen
    void returnToHome();

    // Get currently active app
    App* getActiveApp() const { return activeApp; }

    // Check if an app is active
    bool hasActiveApp() const { return activeApp != nullptr; }

    // Frame update for active app
    void update(float deltaTime);

    // Render active app
    void render(Renderer& renderer);

private:
    std::vector<std::unique_ptr<App>> apps;
    std::vector<App*> appPointers;  // Non-owning pointers for easy access
    App* activeApp;
    size_t homeAppIndex;

    void switchToApp(App* newApp);
};

} // namespace AOS
