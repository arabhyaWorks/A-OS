#include "home_app.h"
#include "os/app_manager.h"
#include <iostream>

// Forward declaration to access global AppManager
extern AOS::AppManager* g_appManager;

namespace AOS {

HomeApp::HomeApp()
    : focusedIndex(0)
{
}

void HomeApp::onStart() {
    std::cout << "HomeApp: Started" << std::endl;
}

void HomeApp::onResume() {
    // Refresh app list each time we return to home
    refreshAppList();
}

void HomeApp::update(float deltaTime) {
    (void)deltaTime;
    // Home screen is mostly static, nothing to update per frame
}

void HomeApp::render(Renderer& renderer) {
    // Draw title bar background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), 80), Color(40, 40, 60), true);

    // Draw title text
    renderer.drawText("A-OS v0.1 - Application Operating System",
                      20, 25, Color::White(), 28);

    // Draw app tiles
    for (size_t i = 0; i < appTiles.size(); ++i) {
        const auto& tile = appTiles[i];

        Color tileColor = tile.focused ? Color(80, 120, 200) : Color(60, 60, 80);
        Color borderColor = tile.focused ? Color(120, 180, 255) : Color(100, 100, 120);
        Color textColor = tile.focused ? Color::White() : Color(200, 200, 200);

        // Draw tile background
        renderer.drawRect(Rect(tile.x, tile.y, tile.w, tile.h), tileColor, true);

        // Draw tile border (thicker if focused)
        renderer.drawRect(Rect(tile.x, tile.y, tile.w, tile.h), borderColor, false);
        if (tile.focused) {
            renderer.drawRect(Rect(tile.x + 1, tile.y + 1, tile.w - 2, tile.h - 2), borderColor, false);
        }

        // Draw app name
        int textX = tile.x + 20;
        int textY = tile.y + (tile.h / 2) - 12;
        renderer.drawText(tile.name, textX, textY, textColor, 24);
    }

    // Draw instructions at bottom
    int instructionY = renderer.getHeight() - 50;
    renderer.drawText("Hey Arabhaya: Use Arrow Keys to navigate | Press Enter to select | ESC to return",
                      20, instructionY, Color(150, 150, 150), 18);
}

void HomeApp::onEvent(const Event& event) {
    switch (event.type) {
        case EventType::KEY_UP:
            moveFocusUp();
            break;

        case EventType::KEY_DOWN:
            moveFocusDown();
            break;

        case EventType::KEY_SELECT:
            launchFocusedApp();
            break;

        default:
            break;
    }
}

void HomeApp::refreshAppList() {
    if (!g_appManager) {
        return;
    }

    appTiles.clear();

    const auto& apps = g_appManager->getInstalledApps();

    // Layout tiles in a vertical list
    int tileWidth = 400;
    int tileHeight = 80;
    int startX = 100;
    int startY = 120;
    int spacing = 20;

    for (size_t i = 0; i < apps.size(); ++i) {
        // Skip self (Home app)
        if (apps[i]->getName() == "Home") {
            continue;
        }

        AppTile tile;
        tile.name = apps[i]->getName();
        tile.x = startX;
        tile.y = startY + (int)appTiles.size() * (tileHeight + spacing);
        tile.w = tileWidth;
        tile.h = tileHeight;
        tile.focused = (appTiles.size() == focusedIndex);

        appTiles.push_back(tile);
    }

    // Ensure focus is valid
    if (focusedIndex >= appTiles.size() && !appTiles.empty()) {
        focusedIndex = 0;
    }

    // Update focus state
    for (size_t i = 0; i < appTiles.size(); ++i) {
        appTiles[i].focused = (i == focusedIndex);
    }
}

void HomeApp::moveFocusUp() {
    if (appTiles.empty()) return;

    if (focusedIndex > 0) {
        focusedIndex--;
    }

    for (size_t i = 0; i < appTiles.size(); ++i) {
        appTiles[i].focused = (i == focusedIndex);
    }
}

void HomeApp::moveFocusDown() {
    if (appTiles.empty()) return;

    if (focusedIndex < appTiles.size() - 1) {
        focusedIndex++;
    }

    for (size_t i = 0; i < appTiles.size(); ++i) {
        appTiles[i].focused = (i == focusedIndex);
    }
}

void HomeApp::launchFocusedApp() {
    if (!g_appManager || appTiles.empty()) {
        return;
    }

    const auto& focusedApp = appTiles[focusedIndex];
    std::cout << "Launching: " << focusedApp.name << std::endl;

    g_appManager->launchAppByName(focusedApp.name);
}

} // namespace AOS
