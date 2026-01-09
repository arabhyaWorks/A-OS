#include "settings_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <cmath>

extern AOS::AppManager* g_appManager;

namespace AOS {

void SettingsApp::onStart() {
    std::cout << "SettingsApp: Started" << std::endl;
    animationTime = 0.0f;
}

void SettingsApp::onStop() {
    std::cout << "SettingsApp: Stopped" << std::endl;
}

void SettingsApp::update(float deltaTime) {
    animationTime += deltaTime;
}

void SettingsApp::render(Renderer& renderer) {
    // Draw header background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), 80), Color(60, 40, 40), true);

    // Draw header text
    renderer.drawText("Settings", 20, 25, Color::White(), 28);

    // Draw a simple animated element to show the app is running
    int centerX = renderer.getWidth() / 2;
    int centerY = renderer.getHeight() / 2;

    float wave = std::sin(animationTime * 2.0f) * 0.5f + 0.5f;
    int size = 50 + (int)(wave * 30);

    Color boxColor(100 + (int)(wave * 155), 100, 100);
    renderer.drawRect(
        Rect(centerX - size/2, centerY - size/2, size, size),
        boxColor,
        true
    );

    // Draw demo text
    renderer.drawText("Settings Demo", centerX - 80, centerY - 100, Color::White(), 24);
    renderer.drawText("This is a placeholder app", centerX - 120, centerY + 80, Color(200, 200, 200), 20);

    // Draw instructions at bottom
    renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void SettingsApp::onEvent(const Event& event) {
    // Back button returns to home
    if (event.type == EventType::KEY_BACK) {
        std::cout << "SettingsApp: Returning to home" << std::endl;
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    }
}

} // namespace AOS
