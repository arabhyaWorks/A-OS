#include "home_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <cmath>

// Forward declaration to access global AppManager
extern AOS::AppManager* g_appManager;

namespace AOS {

HomeApp::HomeApp()
    : focusedIndex(0)
    , globalTime(0.0f)
    , focusTransition(1.0f)
    , previousFocusIndex(0)
    , scrollOffset(0.0f)
    , targetScrollOffset(0.0f)
{
}

void HomeApp::onStart() {
    std::cout << "HomeApp: Started with modern UI" << std::endl;
}

void HomeApp::onResume() {
    // Refresh app list each time we return to home
    refreshAppList();
    globalTime = 0.0f;
}

void HomeApp::update(float deltaTime) {
    updateAnimations(deltaTime);
}

void HomeApp::updateAnimations(float deltaTime) {
    globalTime += deltaTime;
    
    // Smooth focus transition
    if (focusTransition < 1.0f) {
        focusTransition += deltaTime * 8.0f; // Fast transition
        if (focusTransition > 1.0f) {
            focusTransition = 1.0f;
        }
    }
    
    // Smooth scrolling
    float scrollDiff = targetScrollOffset - scrollOffset;
    scrollOffset += scrollDiff * deltaTime * 10.0f; // Smooth easing
    
    // Update tile animations
    for (size_t i = 0; i < appTiles.size(); ++i) {
        auto& tile = appTiles[i];
        
        float targetHover = (i == focusedIndex) ? 1.0f : 0.0f;
        float hoverDiff = targetHover - tile.hoverAnimation;
        tile.hoverAnimation += hoverDiff * deltaTime * 10.0f;
        
        // Clamp
        if (tile.hoverAnimation < 0.0f) tile.hoverAnimation = 0.0f;
        if (tile.hoverAnimation > 1.0f) tile.hoverAnimation = 1.0f;
    }
}

void HomeApp::render(Renderer& renderer) {
    // Simple clean gradient background
    Color bgTop(16, 20, 38);
    Color bgBottom(10, 12, 22);
    renderer.drawGradientRect(Rect(0, 0, renderer.getWidth(), renderer.getHeight()), bgTop, bgBottom);
    
    // Draw header
    drawModernHeader(renderer);
    
    // Draw app tiles
    for (size_t i = 0; i < appTiles.size(); ++i) {
        drawModernTile(renderer, appTiles[i], (int)i);
    }
    
    // Clean footer
    int footerY = renderer.getHeight() - 65;
    Color footerBg(15, 18, 30, 240);
    renderer.drawRect(Rect(0, footerY, renderer.getWidth(), 65), footerBg, true);
    renderer.drawLine(0, footerY, renderer.getWidth(), footerY, Color(80, 110, 180, 100), 1);
    
    std::string instructions = "Navigate: UP/DOWN     Select: ENTER     Back: ESC";
    int textWidth = instructions.length() * 10;
    int textX = (renderer.getWidth() - textWidth) / 2;
    renderer.drawText(instructions, textX, footerY + 24, Color(190, 200, 220), 18);
}

void HomeApp::drawModernHeader(Renderer& renderer) {
    // Clean dark header
    Color headerBg(20, 25, 45);
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), HEADER_HEIGHT), headerBg, true);
    
    // Single clean accent line
    float pulse = (sinf(globalTime * 2.0f) + 1.0f) * 0.5f;
    int accentAlpha = 120 + (int)(80 * pulse);
    renderer.drawLine(0, HEADER_HEIGHT - 2, renderer.getWidth(), HEADER_HEIGHT - 2,
                     Color(100, 150, 240, accentAlpha), 2);
    
    // Logo text
    renderer.drawText("A-OS", 45, 28, Color(255, 255, 255), 42);
    renderer.drawText("Application Operating System", 48, 68, Color(160, 175, 200), 15);
    
    // System status
    int statusX = renderer.getWidth() - 190;
    int statusY = 42;
    
    // Animated status dot
    int dotX = statusX;
    int dotY = statusY + 8;
    float dotPulse = (sinf(globalTime * 3.0f) + 1.0f) * 0.5f;
    int dotSize = 5 + (int)(2 * dotPulse);
    renderer.drawCircle(dotX, dotY, dotSize, Color(80, 255, 150), true);
    
    renderer.drawText("System Active", dotX + 18, statusY, Color(200, 215, 235), 17);
}

void HomeApp::drawModernTile(Renderer& renderer, const AppTile& tile, int index) {
    float hover = tile.hoverAnimation;
    
    int drawY = tile.y + (int)scrollOffset;
    
    if (drawY + tile.h < HEADER_HEIGHT || drawY > renderer.getHeight() - 70) {
        return;
    }
    
    // Subtle shadow only when focused
    if (hover > 0.3f) {
        renderer.drawShadow(Rect(tile.x, drawY, tile.w, tile.h), 6, 12);
    }
    
    // Clean, solid dark card
    Color cardBg(28 + (int)(15 * hover), 32 + (int)(18 * hover), 52 + (int)(30 * hover));
    renderer.drawRect(Rect(tile.x, drawY, tile.w, tile.h), cardBg, true);
    
    // Border when focused
    if (hover > 0.5f) {
        Color borderColor(100 + (int)(100 * hover), 140 + (int)(80 * hover), 220, (int)(200 * hover));
        // Draw border manually for clean lines
        for (int i = 0; i < 2; ++i) {
            renderer.drawRect(Rect(tile.x + i, drawY + i, tile.w - i*2, tile.h - i*2), borderColor, false);
        }
    }
    
    // Professional app icon
    int iconSize = 54 + (int)(6 * hover);
    int iconX = tile.x + 30;
    int iconY = drawY + (tile.h - iconSize) / 2;
    drawAppIcon(renderer, iconX, iconY, iconSize, tile.iconHue, hover);
    
    // High contrast app name
    int textX = iconX + iconSize + 30;
    int textY = drawY + tile.h / 2 - 18;
    renderer.drawText(tile.name, textX, textY, Color(255, 255, 255), 28);
    
    // Subtle subtitle
    renderer.drawText("Launch application", textX, textY + 32, Color(150, 160, 180), 15);
    
    // Simple arrow when focused
    if (hover > 0.5f) {
        int arrowX = tile.x + tile.w - 50;
        int arrowY = drawY + tile.h / 2;
        Color arrowColor(200, 220, 255);
        renderer.drawLine(arrowX, arrowY, arrowX + 14, arrowY, arrowColor, 2);
        renderer.drawLine(arrowX + 14, arrowY, arrowX + 8, arrowY - 5, arrowColor, 2);
        renderer.drawLine(arrowX + 14, arrowY, arrowX + 8, arrowY + 5, arrowColor, 2);
    }
    
    // Number badge
    int badgeX = tile.x + tile.w - 28;
    int badgeY = drawY + 18;
    Color badgeBg(50 + (int)(20 * hover), 55 + (int)(25 * hover), 80 + (int)(30 * hover));
    renderer.drawCircle(badgeX, badgeY, 14, badgeBg, true);
    
    std::string indexStr = std::to_string(index + 1);
    int numX = badgeX - (indexStr.length() > 1 ? 7 : 4);
    renderer.drawText(indexStr, numX, badgeY - 9, Color(200, 210, 230), 16);
}

void HomeApp::drawAppIcon(Renderer& renderer, int x, int y, int size, float hue, float hover) {
    // Convert HSV to RGB
    float h = hue / 60.0f;
    int hi = (int)h;
    float f = h - hi;
    float s = 0.70f;
    float v = 0.85f;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    
    float r = 0, g = 0, b = 0;
    switch (hi % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    
    int centerX = x + size / 2;
    int centerY = y + size / 2;
    int radius = size / 2;
    
    // Simple solid color circle
    Color iconColor((int)(r * 220), (int)(g * 220), (int)(b * 220));
    renderer.drawCircle(centerX, centerY, radius, iconColor, true);
    
    // Subtle highlight
    Color highlightColor(255, 255, 255, 50);
    int hlRadius = (int)(radius * 0.6f);
    renderer.drawCircle(centerX - 3, centerY - 3, hlRadius, highlightColor, true);
    
    // Simple clean icon - single rounded square
    int squareSize = (int)(size * 0.4f);
    int squareX = centerX - squareSize / 2;
    int squareY = centerY - squareSize / 2;
    Color symbolColor(255, 255, 255, 220);
    
    // Draw symbol as filled rectangle with slight rounding
    for (int dy = 0; dy < squareSize; ++dy) {
        for (int dx = 0; dx < squareSize; ++dx) {
            // Skip corners for rounded effect
            if ((dx < 3 && dy < 3) || (dx < 3 && dy > squareSize - 4) ||
                (dx > squareSize - 4 && dy < 3) || (dx > squareSize - 4 && dy > squareSize - 4)) {
                continue;
            }
            renderer.drawRect(Rect(squareX + dx, squareY + dy, 1, 1), symbolColor, true);
        }
    }
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

    // Layout tiles in a modern vertical list with improved spacing
    int startX = TILE_MARGIN_LEFT;
    int startY = TILE_START_Y;

    float hueStep = 360.0f / (apps.size() > 1 ? apps.size() - 1 : 1);
    int tileIndex = 0;

    for (size_t i = 0; i < apps.size(); ++i) {
        // Skip self (Home app)
        if (apps[i]->getName() == "Home") {
            continue;
        }

        AppTile tile;
        tile.name = apps[i]->getName();
        tile.x = startX;
        tile.y = startY + tileIndex * (TILE_HEIGHT + TILE_SPACING);
        tile.w = TILE_WIDTH;
        tile.h = TILE_HEIGHT;
        tile.focused = (tileIndex == (int)focusedIndex);
        tile.hoverAnimation = tile.focused ? 1.0f : 0.0f;
        tile.iconHue = hueStep * tileIndex;

        appTiles.push_back(tile);
        tileIndex++;
    }

    // Ensure focus is valid
    if (focusedIndex >= appTiles.size() && !appTiles.empty()) {
        focusedIndex = 0;
    }

    // Update focus state
    for (size_t i = 0; i < appTiles.size(); ++i) {
        appTiles[i].focused = (i == focusedIndex);
    }
    
    // Reset animations
    focusTransition = 1.0f;
    previousFocusIndex = focusedIndex;
}

void HomeApp::moveFocusUp() {
    if (appTiles.empty()) return;

    if (focusedIndex > 0) {
        previousFocusIndex = focusedIndex;
        focusedIndex--;
        focusTransition = 0.0f;
        
        // Update target scroll offset for smooth scrolling
        int targetY = appTiles[focusedIndex].y;
        targetScrollOffset = -(targetY - TILE_START_Y);
    }

    for (size_t i = 0; i < appTiles.size(); ++i) {
        appTiles[i].focused = (i == focusedIndex);
    }
}

void HomeApp::moveFocusDown() {
    if (appTiles.empty()) return;

    if (focusedIndex < appTiles.size() - 1) {
        previousFocusIndex = focusedIndex;
        focusedIndex++;
        focusTransition = 0.0f;
        
        // Update target scroll offset for smooth scrolling
        int targetY = appTiles[focusedIndex].y;
        targetScrollOffset = -(targetY - TILE_START_Y);
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
