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
        float hoverAnimation;     // 0.0 to 1.0 for smooth transitions
        float iconHue;            // Color hue for app icon (0-360)
    };

    std::vector<AppTile> appTiles;
    size_t focusedIndex;
    
    // Animation state
    float globalTime;             // Total elapsed time for animations
    float focusTransition;        // Smooth focus transition (0.0 to 1.0)
    size_t previousFocusIndex;    // For smooth transitions
    float scrollOffset;           // Smooth scrolling offset
    float targetScrollOffset;     // Target scroll position
    
    // UI Layout constants
    static constexpr int TILE_WIDTH = 500;
    static constexpr int TILE_HEIGHT = 100;
    static constexpr int TILE_SPACING = 20;
    static constexpr int TILE_MARGIN_LEFT = 80;
    static constexpr int TILE_START_Y = 140;
    static constexpr int HEADER_HEIGHT = 100;

    void refreshAppList();
    void moveFocusUp();
    void moveFocusDown();
    void launchFocusedApp();
    void updateAnimations(float deltaTime);
    void drawModernHeader(Renderer& renderer);
    void drawModernTile(Renderer& renderer, const AppTile& tile, int index);
    void drawAppIcon(Renderer& renderer, int x, int y, int size, float hue, float hover = 0.0f);
};

} // namespace AOS
