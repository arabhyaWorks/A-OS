#include "app_manager.h"
#include <iostream>

namespace AOS {

AppManager::AppManager()
    : activeApp(nullptr)
    , homeAppIndex(0)
{
    // Subscribe to all input events and forward to active app
    auto& eventBus = EventBus::getInstance();

    eventBus.subscribe(EventType::KEY_UP, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });

    eventBus.subscribe(EventType::KEY_DOWN, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });

    eventBus.subscribe(EventType::KEY_LEFT, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });

    eventBus.subscribe(EventType::KEY_RIGHT, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });

    eventBus.subscribe(EventType::KEY_SELECT, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });

    eventBus.subscribe(EventType::KEY_BACK, [this](const Event& e) {
        if (activeApp) activeApp->onEvent(e);
    });
}

AppManager::~AppManager() {
}

void AppManager::registerApp(std::unique_ptr<App> app) {
    appPointers.push_back(app.get());
    apps.push_back(std::move(app));

    // First registered app is assumed to be Home
    if (apps.size() == 1) {
        homeAppIndex = 0;
    }
}

const std::vector<App*>& AppManager::getInstalledApps() const {
    return appPointers;
}

void AppManager::launchApp(size_t index) {
    if (index >= apps.size()) {
        std::cerr << "Invalid app index: " << index << std::endl;
        return;
    }

    switchToApp(apps[index].get());
}

void AppManager::launchAppByName(const std::string& name) {
    for (size_t i = 0; i < apps.size(); ++i) {
        if (apps[i]->getName() == name) {
            launchApp(i);
            return;
        }
    }

    std::cerr << "App not found: " << name << std::endl;
}

void AppManager::returnToHome() {
    if (homeAppIndex < apps.size()) {
        switchToApp(apps[homeAppIndex].get());
    }
}

void AppManager::update(float deltaTime) {
    if (activeApp) {
        activeApp->update(deltaTime);
    }
}

void AppManager::render(Renderer& renderer) {
    if (activeApp) {
        activeApp->render(renderer);
    }
}

void AppManager::switchToApp(App* newApp) {
    if (activeApp == newApp) {
        return;  // Already active
    }

    // Pause current app
    if (activeApp) {
        activeApp->onPause();
        activeApp->onStop();
    }

    // Start new app
    activeApp = newApp;
    if (activeApp) {
        std::cout << "Launching app: " << activeApp->getName() << std::endl;
        activeApp->onStart();
        activeApp->onResume();
    }
}

} // namespace AOS
