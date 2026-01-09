#pragma once

#include "os/app.h"
#include "ui/renderer.h"
#include <string>
#include <vector>

namespace AOS {

/**
 * SysInfoApp - System information viewer
 *
 * Displays:
 * - OS version
 * - Hardware info (simulated for desktop)
 * - Memory usage
 * - Uptime
 * - Platform details
 *
 * In production:
 * - Real hardware detection
 * - Temperature monitoring
 * - Network status
 */
class SysInfoApp : public App {
public:
    SysInfoApp();
    ~SysInfoApp() override = default;

    void onStart() override;
    void onStop() override;
    void onResume() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "System Info"; }

private:
    struct InfoItem {
        std::string label;
        std::string value;
    };

    std::vector<InfoItem> infoItems;
    float uptimeSeconds;

    void refreshSystemInfo();
};

} // namespace AOS
