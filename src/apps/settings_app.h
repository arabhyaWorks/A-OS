#pragma once

#include "os/app.h"
#include "ui/renderer.h"

namespace AOS {

/**
 * SettingsApp - System settings interface
 *
 * A simple demonstration app that shows:
 * - How apps respond to events
 * - How to return to home
 * - Basic app structure
 *
 * In a full system, this would contain:
 * - Network settings
 * - Display settings
 * - Audio settings
 * - System info
 * - etc.
 */
class SettingsApp : public App {
public:
    SettingsApp() = default;
    ~SettingsApp() override = default;

    void onStart() override;
    void onStop() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "Settings"; }

private:
    float animationTime;
};

} // namespace AOS
