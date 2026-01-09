#include "sysinfo_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#include <unistd.h>
#endif

extern AOS::AppManager* g_appManager;

namespace AOS {

SysInfoApp::SysInfoApp()
    : uptimeSeconds(0.0f)
{
}

void SysInfoApp::onStart() {
    std::cout << "SysInfoApp: Started" << std::endl;
    refreshSystemInfo();
}

void SysInfoApp::onStop() {
    std::cout << "SysInfoApp: Stopped" << std::endl;
}

void SysInfoApp::onResume() {
    refreshSystemInfo();
}

void SysInfoApp::update(float deltaTime) {
    uptimeSeconds += deltaTime;

    // Update uptime display every second
    static float lastUpdate = 0.0f;
    lastUpdate += deltaTime;
    if (lastUpdate >= 1.0f) {
        lastUpdate = 0.0f;

        // Update uptime in info items
        int hours = (int)(uptimeSeconds / 3600);
        int minutes = (int)((uptimeSeconds - hours * 3600) / 60);
        int seconds = (int)(uptimeSeconds) % 60;

        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << minutes << ":"
            << std::setfill('0') << std::setw(2) << seconds;

        // Find and update uptime
        for (auto& item : infoItems) {
            if (item.label == "Uptime:") {
                item.value = oss.str();
                break;
            }
        }
    }
}

void SysInfoApp::render(Renderer& renderer) {
    // Draw header background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), 80), Color(40, 40, 80), true);
    renderer.drawText("System Information", 20, 25, Color::White(), 28);

    // Draw system info
    int startY = 120;
    int lineHeight = 35;

    for (size_t i = 0; i < infoItems.size(); i++) {
        int y = startY + i * lineHeight;

        // Label
        renderer.drawText(
            infoItems[i].label,
            50,
            y,
            Color(150, 150, 200),
            20
        );

        // Value
        renderer.drawText(
            infoItems[i].value,
            300,
            y,
            Color::White(),
            20
        );
    }

    // Draw decorative separator
    renderer.drawRect(
        Rect(50, 100, renderer.getWidth() - 100, 2),
        Color(100, 100, 150),
        true
    );

    // Instructions
    renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void SysInfoApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        std::cout << "SysInfoApp: Returning to home" << std::endl;
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    }
}

void SysInfoApp::refreshSystemInfo() {
    infoItems.clear();

    // OS Name and Version
    infoItems.push_back({"OS Name:", "A-OS (Application Operating System)"});
    infoItems.push_back({"Version:", "v0.2 - Text Rendering"});

    // Platform detection
#ifdef _WIN32
    infoItems.push_back({"Platform:", "Windows (Desktop Simulation)"});

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    std::ostringstream cpuOss;
    cpuOss << "x86_64 (" << sysInfo.dwNumberOfProcessors << " cores)";
    infoItems.push_back({"CPU:", cpuOss.str()});

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    std::ostringstream memOss;
    memOss << (totalPhysMem / 1024 / 1024) << " MB";
    infoItems.push_back({"Memory:", memOss.str()});

#else
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        infoItems.push_back({"Platform:", std::string(unameData.sysname)});
        infoItems.push_back({"Kernel:", std::string(unameData.release)});
        infoItems.push_back({"Architecture:", std::string(unameData.machine)});
    } else {
        infoItems.push_back({"Platform:", "Linux/Unix"});
    }

    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (pages > 0 && page_size > 0) {
        std::ostringstream memOss;
        memOss << (pages * page_size / 1024 / 1024) << " MB";
        infoItems.push_back({"Memory:", memOss.str()});
    }
#endif

    // Graphics
    infoItems.push_back({"Graphics:", "SDL2 Renderer"});
    infoItems.push_back({"Display:", "1280x720 (Simulated)"});

    // Uptime (will be updated in update())
    infoItems.push_back({"Uptime:", "00:00:00"});

    // Target hardware info
    infoItems.push_back({"Target:", "Raspberry Pi 5 (4GB/8GB)"});
}

} // namespace AOS
