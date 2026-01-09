#include "os/os_core.h"
#include "apps/home_app.h"
#include "apps/settings_app.h"
#include "apps/camera_app.h"
#include "apps/sysinfo_app.h"
#include "apps/media_app.h"
#include "apps/flappy_app.h"
#include <memory>
#include <iostream>

// Global pointer for apps to access AppManager
// (In a more sophisticated system, this would be handled via dependency injection)
AOS::AppManager* g_appManager = nullptr;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << R"(
    ╔═══════════════════════════════════════╗
    ║                                       ║
    ║              A-OS v0.2                ║
    ║   Application Operating System        ║
    ║                                       ║
    ╚═══════════════════════════════════════╝
    )" << std::endl;

    // Create OS Core
    AOS::OSCore os;

    // Initialize (desktop simulation mode: 1280x720 window)
    bool fullscreen = false;
    if (!os.initialize(1280, 720, fullscreen)) {
        std::cerr << "Failed to initialize A-OS" << std::endl;
        return 1;
    }

    // Get AppManager reference
    g_appManager = &os.getAppManager();

    // Register applications
    // Order matters: first app is Home
    std::cout << "Registering applications..." << std::endl;

    g_appManager->registerApp(std::make_unique<AOS::HomeApp>());
    g_appManager->registerApp(std::make_unique<AOS::SettingsApp>());
    g_appManager->registerApp(std::make_unique<AOS::CameraApp>());
    g_appManager->registerApp(std::make_unique<AOS::SysInfoApp>());
    g_appManager->registerApp(std::make_unique<AOS::MediaApp>());
    g_appManager->registerApp(std::make_unique<AOS::FlappyApp>());

    std::cout << "6 applications registered." << std::endl;

    // Launch home screen
    g_appManager->launchApp(0);

    // Run the OS (blocks until quit)
    os.run();

    // Cleanup
    g_appManager = nullptr;
    os.shutdown();

    std::cout << "A-OS terminated." << std::endl;

    return 0;
}
