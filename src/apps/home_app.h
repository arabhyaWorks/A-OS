#pragma once

#include "os/app.h"
#include "ui/renderer.h"
#include <vector>

namespace AOS {

/**
 * HomeApp - The launcher/home screen
 *
 * This is the "start menu" of A-OS. It displays:
 * - Grid of installed application tiles
 * - Focus-based navigation (arrows + select)
 * - Launch apps when selected
 *
 * This app is special: it's always the first app registered
 * and the OS returns here when pressing "back" from other apps.
 */
class HomeApp : public App {
public:
    HomeApp();
    ~HomeApp() override = default;

    void onStart() override;
    void onResume() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "Home"; }

private:
    struct AppTile {
        std::string name;
        int x, y, w, h;
        bool focused;
    };

    std::vector<AppTile> appTiles;
    size_t focusedIndex;

    void refreshAppList();
    void moveFocusUp();
    void moveFocusDown();
    void launchFocusedApp();
};

} // namespace AOS
