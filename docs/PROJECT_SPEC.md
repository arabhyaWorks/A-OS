# A-OS (Application Operating System)
## Comprehensive Technical Specification & Development Guide

**Version:** 0.1.0
**Last Updated:** 2026-01-09
**Status:** MVP Foundation Complete

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Vision & Philosophy](#vision--philosophy)
3. [Core Architecture](#core-architecture)
4. [Coding Standards](#coding-standards)
5. [Project Structure](#project-structure)
6. [Subsystem Specifications](#subsystem-specifications)
7. [Application Development Guide](#application-development-guide)
8. [Build & Deployment](#build--deployment)
9. [Development Workflow](#development-workflow)
10. [Roadmap & Milestones](#roadmap--milestones)
11. [Design Decisions & Rationale](#design-decisions--rationale)

---

## 1. Executive Summary

### What is A-OS?

**A-OS (Application Operating System)** is a **console-style, full-screen application platform** that runs on top of Linux for embedded devices (Raspberry Pi 5, Pi Zero 2 W).

It is **NOT**:
- ❌ A Linux distribution
- ❌ A desktop environment (GNOME, KDE)
- ❌ A window manager (X11/Wayland)
- ❌ Android or Android-compatible

It **IS**:
- ✅ A single-purpose, full-screen system UI
- ✅ A native C++ application framework
- ✅ A device OS (like PlayStation, SteamOS, Android SystemUI)
- ✅ Voice-enabled from the ground up
- ✅ Designed for deterministic, console-like behavior

### One-Sentence Positioning

> **A-OS is a Linux-powered, console-style Application Operating System that transforms Raspberry Pi into a dedicated, voice-enabled, app-centric device—without being Linux or Android.**

### Key Characteristics

| Aspect | Description |
|--------|-------------|
| **UI Model** | Full-screen, single active app |
| **Input** | Unified events (buttons, keyboard, gamepad, voice) |
| **Language** | C++17 |
| **Graphics** | SDL2 (framebuffer or window) |
| **Audio** | ALSA (direct PCM access) |
| **Boot** | Power ON → Splash → Home Screen |
| **Apps** | Native C++ binaries |
| **Voice** | First-class input (on-device ASR) |

---

## 2. Vision & Philosophy

### Design Principles

#### Principle 1: Device-First Mentality
**A-OS is a product, not a PC.**

- Users never see Linux
- No terminal access
- No desktop
- Single-purpose experience

**Example:** PlayStation OS, Steam Deck UI, Smart TV interfaces

---

#### Principle 2: Deterministic Behavior
**Predictable, console-like operation.**

- No background processes fighting for resources
- One foreground app at a time
- Controlled lifecycle
- Fixed memory budgets

**Anti-pattern:** Desktop multitasking chaos

---

#### Principle 3: Event-Driven Everything
**All input becomes events. Hardware is abstracted.**

```
Physical Input → HAL → Event → EventBus → Active App
```

Apps **never** touch:
- `/dev/input/*`
- `/dev/fb0`
- `/dev/snd/*`

Apps **only** use:
- `EventBus` (input)
- `Renderer` (display)
- `AudioManager` (sound)

---

#### Principle 4: Voice as First-Class Input
**Voice is not a feature. It's an input device.**

Just like buttons, keyboard, and gamepad, voice emits events:

```cpp
VOICE_WAKE       // Wake word detected
VOICE_FINAL      // "open settings"
VOICE_COMMAND    // Parsed intent
```

Apps subscribe to voice events like any other input.

---

#### Principle 5: Native Performance
**No web stack. No Electron. C++ only.**

- Direct SDL2 rendering
- Manual memory management
- Zero garbage collection
- Sub-millisecond latency

**Target:** 60 FPS, <16ms frame time

---

#### Principle 6: Desktop-First Development
**Build on PC, deploy to Pi.**

- Fast iteration on desktop
- Same code runs on Pi (recompile)
- SDL window simulates framebuffer
- No SD card flashing during dev

**Workflow:**
```bash
# Desktop
make && ./aos

# Pi (when ready)
scp aos pi:~/ && ssh pi ./aos
```

---

### What A-OS Is Intentionally NOT

| ❌ NOT This | ✅ Instead This |
|------------|----------------|
| Multi-user OS | Single-user device |
| Desktop replacement | Console experience |
| General-purpose | Application-specific |
| Browser-centric | Native apps |
| APK-compatible | Custom app model |
| X11/Wayland | Framebuffer/SDL |

---

## 3. Core Architecture

### High-Level System Diagram

```
┌─────────────────────────────────────────────────────────┐
│                       A-OS Core                         │
│                                                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │  OSCore      │  │  EventBus    │  │  AppManager  │ │
│  │              │  │              │  │              │ │
│  │ - Init       │  │ - Pub/Sub    │  │ - Lifecycle  │ │
│  │ - MainLoop   │  │ - Queue      │  │ - Switching  │ │
│  │ - Shutdown   │  │ - Dispatch   │  │ - Registry   │ │
│  └──────────────┘  └──────────────┘  └──────────────┘ │
│         │                  │                  │         │
│  ┌──────────────────────────────────────────────────┐  │
│  │           Hardware Abstraction Layer             │  │
│  │                                                  │  │
│  │  ┌─────────────┐  ┌─────────────┐  ┌──────────┐│  │
│  │  │ InputMgr    │  │ AudioMgr    │  │ Renderer ││  │
│  │  │             │  │             │  │          ││  │
│  │  │ - Keyboard  │  │ - ALSA      │  │ - SDL2   ││  │
│  │  │ - Gamepad   │  │ - Capture   │  │ - Shapes ││  │
│  │  │ - Buttons   │  │ - Playback  │  │ - Text   ││  │
│  │  │ - Voice*    │  │ - TTS*      │  │ - Images*││  │
│  │  └─────────────┘  └─────────────┘  └──────────┘│  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │                 Applications                     │  │
│  │                                                  │  │
│  │  [Home] [Settings] [Camera*] [Voice*] [Game*]   │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                         │
                         ▼
                  Linux Kernel
         (hardware drivers, process management)
```

**Legend:**
- `*` = Future/Planned feature
- `[ ]` = Application component

---

### Boot Sequence

```
Power ON
  ↓
Bootloader (U-Boot/UEFI)
  ↓
Linux Kernel Initialization
  ↓
systemd Init
  ↓
systemd launches aos.service
  ↓
OSCore::initialize()
  ├─ SDL_Init()
  ├─ Create Window/Framebuffer
  ├─ Initialize EventBus
  ├─ Initialize AppManager
  ├─ Initialize InputManager
  └─ Initialize AudioManager
  ↓
Register Built-in Apps
  ├─ HomeApp
  ├─ SettingsApp
  └─ [Other apps]
  ↓
Launch Home Screen
  ↓
OSCore::run() [Main Loop]
  ├─ Poll SDL events
  ├─ InputManager processes input
  ├─ EventBus dispatches events
  ├─ Active app updates
  ├─ Active app renders
  └─ Frame rate limiter (60 FPS)
```

**No login. No shell. Just UI.**

---

### Main Loop (60 FPS Target)

```cpp
void OSCore::run() {
    running = true;
    lastFrameTime = SDL_GetPerformanceCounter();

    while (running) {
        // 1. Process SDL events (quit, window, etc.)
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            inputManager->handleSDLEvent(e);
        }

        // 2. Process event queue (dispatches to active app)
        EventBus::getInstance().processEvents();

        // 3. Update active app
        float deltaTime = getDeltaTime();
        if (appManager->hasActiveApp()) {
            appManager->update(deltaTime);
        }

        // 4. Render
        renderer->clear();
        if (appManager->hasActiveApp()) {
            appManager->render(*renderer);
        }
        renderer->present();

        // 5. Frame rate cap (16ms = 60 FPS)
        SDL_Delay(1);
    }
}
```

**Key Points:**
- Fixed timestep considered (currently variable)
- Input → Events → Update → Render
- Active app owns the frame

---

## 4. Coding Standards

### Language & Compiler

- **Language:** C++17
- **Compiler:** GCC 7+, Clang 5+, MSVC 2019+
- **Standard Library:** Use STL freely

### File Organization

```
Header files:    .h
Source files:    .cpp
Namespace:       AOS::
```

### Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| **Classes** | PascalCase | `AppManager`, `EventBus` |
| **Functions** | camelCase | `launchApp()`, `getDeltaTime()` |
| **Variables** | camelCase | `activeApp`, `focusedIndex` |
| **Constants** | UPPER_SNAKE_CASE | `MAX_APPS`, `DEFAULT_FPS` |
| **Private Members** | camelCase (no prefix) | `running`, `lastFrameTime` |
| **Enums** | PascalCase | `EventType::KEY_UP` |
| **Namespaces** | PascalCase | `AOS::` |
| **Files** | snake_case | `app_manager.cpp`, `event_bus.h` |

### Code Style

**Brace Style:** K&R variant
```cpp
void function() {
    if (condition) {
        doSomething();
    } else {
        doOtherThing();
    }
}
```

**Indentation:** 4 spaces (no tabs)

**Line Length:** Prefer <100 characters, hard limit 120

**Header Guards:** `#pragma once` (not `#ifndef`)

```cpp
#pragma once

namespace AOS {
    // declarations
}
```

**Include Order:**
1. Corresponding header (if .cpp)
2. System headers (`<SDL2/SDL.h>`)
3. Project headers (`"os/app.h"`)

```cpp
#include "home_app.h"        // 1. Own header

#include <iostream>          // 2. System
#include <memory>

#include "os/app_manager.h"  // 3. Project
```

**Namespace Usage:**
- Always wrap code in `namespace AOS { }`
- Never `using namespace std;` in headers
- Okay to use in `.cpp` files if localized

### Memory Management

**Ownership Model:**

| Scenario | Use |
|----------|-----|
| **Unique ownership** | `std::unique_ptr<T>` |
| **Shared ownership** | `std::shared_ptr<T>` (rare) |
| **Non-owning reference** | Raw pointer `T*` or reference `T&` |
| **Optional value** | `std::optional<T>` or pointer |

**Example:**
```cpp
class AppManager {
private:
    std::vector<std::unique_ptr<App>> apps;     // Owns apps
    std::vector<App*> appPointers;               // Non-owning
    App* activeApp;                              // Non-owning
};
```

**Rules:**
- Prefer stack allocation
- Use RAII (constructors/destructors)
- No manual `new`/`delete` unless necessary
- No raw `malloc`/`free`

### Error Handling

**Strategy:** Return `bool` for success/failure, log errors

```cpp
bool OSCore::initialize(int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}
```

**Do NOT use exceptions** (embedded-friendly code)

**Assertions:** Use `assert()` for impossible conditions during development

```cpp
#include <cassert>

void AppManager::launchApp(size_t index) {
    assert(index < apps.size());  // Debug check
    // ...
}
```

### Logging

**Use `std::cout` for info, `std::cerr` for errors:**

```cpp
std::cout << "HomeApp: Started" << std::endl;
std::cerr << "Failed to load texture: " << path << std::endl;
```

**Future:** Custom logging system with levels (DEBUG, INFO, WARN, ERROR)

### Comments & Documentation

**Header Documentation:**
```cpp
/**
 * AppManager - Manages application lifecycle and switching
 *
 * Responsibilities:
 * - Register installed apps
 * - Launch and switch between apps
 * - Manage app lifecycle (start, pause, resume, stop)
 * - Only one app active (foreground) at a time
 */
class AppManager {
    // ...
};
```

**Inline Comments:**
- Explain **why**, not **what**
- Avoid obvious comments

```cpp
// BAD
i++;  // Increment i

// GOOD
// Skip home app in launcher display
if (apps[i]->getName() == "Home") {
    continue;
}
```

### Code Review Checklist

Before committing:
- [ ] Code compiles without warnings
- [ ] Follows naming conventions
- [ ] No memory leaks (use valgrind/sanitizers)
- [ ] Proper ownership (smart pointers)
- [ ] Commented where necessary
- [ ] Tested on desktop
- [ ] Checked on Pi (if HAL changes)

---

## 5. Project Structure

### Directory Layout

```
aos/
├── src/
│   ├── main.cpp                    # Entry point, app registration
│   │
│   ├── os/                         # Core OS subsystems
│   │   ├── os_core.h/.cpp          # Main coordinator, main loop
│   │   ├── app_manager.h/.cpp      # App lifecycle & switching
│   │   ├── event_bus.h/.cpp        # Pub/sub event system
│   │   └── app.h                   # Base App interface
│   │
│   ├── hal/                        # Hardware Abstraction Layer
│   │   ├── input_manager.h/.cpp    # Unified input handling
│   │   └── audio_manager.h/.cpp    # ALSA audio (stub)
│   │
│   ├── ui/                         # UI & Rendering
│   │   └── renderer.h/.cpp         # SDL2 wrapper, shapes, text
│   │
│   └── apps/                       # Built-in applications
│       ├── home_app.h/.cpp         # Launcher/home screen
│       └── settings_app.h/.cpp     # System settings
│
├── assets/                         # Resources
│   ├── fonts/                      # TTF fonts (future)
│   ├── images/                     # Icons, sprites (future)
│   └── sounds/                     # UI sounds (future)
│
├── docs/                           # Documentation
│   ├── PROJECT_SPEC.md             # This file
│   ├── API_REFERENCE.md            # App SDK docs (future)
│   └── DEPLOYMENT.md               # Pi deployment guide (future)
│
├── build/                          # Build output (gitignored)
│
├── CMakeLists.txt                  # Build configuration
├── README.md                       # Quick start guide
└── .gitignore
```

### File Responsibilities

| File | Purpose | Touch Frequency |
|------|---------|-----------------|
| `main.cpp` | App registration, OS init | When adding apps |
| `os_core.cpp` | Main loop, subsystem init | Rarely |
| `app_manager.cpp` | App switching logic | Occasionally |
| `event_bus.cpp` | Event routing | Rarely |
| `input_manager.cpp` | Input mapping | When adding inputs |
| `renderer.cpp` | Drawing primitives | When adding features |
| `home_app.cpp` | Launcher UI | Frequently (UX) |
| `*_app.cpp` | Individual apps | Per-app development |

---

## 6. Subsystem Specifications

### 6.1 OSCore (Main Coordinator)

**File:** `src/os/os_core.h`, `src/os/os_core.cpp`

**Responsibilities:**
1. Initialize SDL (video, audio, events)
2. Create window or fullscreen surface
3. Instantiate all subsystems (AppManager, EventBus, etc.)
4. Run main loop (60 FPS target)
5. Coordinate shutdown

**Key Methods:**

```cpp
class OSCore {
public:
    bool initialize(int width, int height, bool fullscreen);
    void run();           // Blocks until quit
    void shutdown();
    AppManager& getAppManager();

private:
    void mainLoop();
    float getDeltaTime();
};
```

**Initialization Order:**
1. SDL_Init
2. Create window/renderer
3. Create Renderer wrapper
4. Create AppManager
5. Create InputManager
6. Create AudioManager

**Shutdown Order:**
1. Destroy subsystems (reverse order)
2. Destroy SDL renderer
3. Destroy SDL window
4. SDL_Quit

---

### 6.2 EventBus (Pub/Sub System)

**File:** `src/os/event_bus.h`, `src/os/event_bus.cpp`

**Responsibilities:**
- Central event distribution
- Queue events for next frame
- Dispatch events to subscribers

**Design Pattern:** Singleton

```cpp
class EventBus {
public:
    static EventBus& getInstance();

    void subscribe(EventType type, EventHandler handler);
    void publish(const Event& event);
    void processEvents();  // Called each frame by OSCore
};
```

**Event Structure:**

```cpp
enum class EventType {
    // System
    SYSTEM_STARTUP,
    SYSTEM_SHUTDOWN,

    // Input
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SELECT, KEY_BACK,

    // Voice (future)
    VOICE_WAKE, VOICE_PARTIAL, VOICE_FINAL, VOICE_COMMAND,

    // App lifecycle
    APP_STARTED, APP_PAUSED, APP_RESUMED, APP_STOPPED,

    // Custom
    CUSTOM
};

struct Event {
    EventType type;
    std::string payload;   // Text data
    int data_int;          // Numeric data
};
```

**Usage Pattern (Apps):**

```cpp
void MyApp::onStart() {
    // Subscribe to events
    EventBus::getInstance().subscribe(EventType::KEY_SELECT,
        [this](const Event& e) {
            this->onSelectPressed();
        });
}

void MyApp::onEvent(const Event& event) {
    // Or handle in app's main event handler
    if (event.type == EventType::KEY_UP) {
        moveUp();
    }
}
```

**Usage Pattern (System):**

```cpp
// InputManager publishes
EventBus::getInstance().publish(Event(EventType::KEY_UP));

// AppManager delivers to active app
void AppManager::update(float dt) {
    if (activeApp) {
        // Events already dispatched by EventBus::processEvents()
    }
}
```

---

### 6.3 AppManager (Lifecycle Manager)

**File:** `src/os/app_manager.h`, `src/os/app_manager.cpp`

**Responsibilities:**
- Register apps at startup
- Launch apps by index or name
- Manage single active app
- Call lifecycle hooks (onStart, onPause, onResume, onStop)
- Forward update/render to active app

**Key Methods:**

```cpp
class AppManager {
public:
    void registerApp(std::unique_ptr<App> app);
    const std::vector<App*>& getInstalledApps() const;

    void launchApp(size_t index);
    void launchAppByName(const std::string& name);
    void returnToHome();  // Launch app index 0

    App* getActiveApp() const;
    bool hasActiveApp() const;

    void update(float deltaTime);
    void render(Renderer& renderer);
};
```

**Lifecycle State Machine:**

```
┌─────────┐
│ Stopped │ (initial state)
└─────────┘
     │ launchApp()
     ↓
┌─────────┐
│ Started │──── onStart()
└─────────┘
     │ switchToApp(other)
     ↓
┌─────────┐
│ Paused  │──── onPause()
└─────────┘
     │ returnToApp()
     ↓
┌─────────┐
│ Resumed │──── onResume()
└─────────┘
     │ launchApp(other) or shutdown
     ↓
┌─────────┐
│ Stopped │──── onStop()
└─────────┘
```

**Switching Behavior:**

```cpp
void AppManager::switchToApp(App* newApp) {
    if (activeApp) {
        activeApp->onPause();
    }

    activeApp = newApp;

    if (activeApp) {
        // First launch or resuming?
        if (!activeApp->hasStarted()) {
            activeApp->onStart();
        } else {
            activeApp->onResume();
        }
    }
}
```

---

### 6.4 App Interface (Base Class)

**File:** `src/os/app.h`

**Responsibilities:**
- Define app contract
- Provide lifecycle hooks
- Receive events, update, render

**Interface:**

```cpp
namespace AOS {

class Renderer;  // Forward declaration
struct Event;

class App {
public:
    virtual ~App() = default;

    // Lifecycle
    virtual void onStart() {}      // First launch
    virtual void onPause() {}      // Backgrounded
    virtual void onResume() {}     // Foregrounded again
    virtual void onStop() {}       // Destroyed

    // Frame callbacks
    virtual void update(float deltaTime) = 0;
    virtual void render(Renderer& renderer) = 0;

    // Input
    virtual void onEvent(const Event& event) {}

    // Metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const { return ""; }
};

} // namespace AOS
```

**App Implementation Example:**

```cpp
class HomeApp : public AOS::App {
public:
    void onStart() override {
        refreshAppList();
    }

    void update(float deltaTime) override {
        // Update animations, etc.
    }

    void render(Renderer& renderer) override {
        // Draw tiles, text
    }

    void onEvent(const Event& event) override {
        if (event.type == EventType::KEY_SELECT) {
            launchFocusedApp();
        }
    }

    std::string getName() const override {
        return "Home";
    }
};
```

---

### 6.5 InputManager (Input Abstraction)

**File:** `src/hal/input_manager.h`, `src/hal/input_manager.cpp`

**Responsibilities:**
- Receive SDL input events
- Map keys/buttons to EventType
- Publish input events to EventBus
- Support multiple input sources (keyboard, gamepad, GPIO future)

**Key Methods:**

```cpp
class InputManager {
public:
    void handleSDLEvent(const SDL_Event& event);

    // Future: GPIO polling
    void pollGPIO();
};
```

**Mapping Table:**

| SDL Key | EventType |
|---------|-----------|
| SDLK_UP / SDLK_w | KEY_UP |
| SDLK_DOWN / SDLK_s | KEY_DOWN |
| SDLK_LEFT / SDLK_a | KEY_LEFT |
| SDLK_RIGHT / SDLK_d | KEY_RIGHT |
| SDLK_RETURN / SDLK_SPACE | KEY_SELECT |
| SDLK_ESCAPE / SDLK_BACKSPACE | KEY_BACK |

**Future:**
- Gamepad mapping (SDL_CONTROLLER*)
- GPIO button polling (Pi-specific)
- Voice input events (from ASR service)

---

### 6.6 Renderer (Display Abstraction)

**File:** `src/ui/renderer.h`, `src/ui/renderer.cpp`

**Responsibilities:**
- Wrap SDL_Renderer
- Provide high-level drawing primitives
- Abstract framebuffer vs window rendering

**Key Methods:**

```cpp
class Renderer {
public:
    // Lifecycle
    void clear(Color color = Color::Black());
    void present();

    // Primitives
    void drawRect(const Rect& rect, const Color& color, bool filled);
    void drawText(const std::string& text, int x, int y,
                  const Color& color, int size = 16);  // Future
    void drawTexture(Texture* tex, const Rect& dest);   // Future

    // Info
    int getWidth() const;
    int getHeight() const;
};
```

**Data Structures:**

```cpp
struct Color {
    uint8_t r, g, b, a;

    static Color Black()  { return {0, 0, 0, 255}; }
    static Color White()  { return {255, 255, 255, 255}; }
    static Color Red()    { return {255, 0, 0, 255}; }
    // ...
};

struct Rect {
    int x, y, w, h;
};
```

**Current State:**
- ✅ Rectangles
- ❌ Text (needs SDL_ttf)
- ❌ Images (needs SDL_image)

---

### 6.7 AudioManager (Sound System)

**File:** `src/hal/audio_manager.h`, `src/hal/audio_manager.cpp`

**Status:** STUB (future implementation)

**Planned Responsibilities:**
- ALSA PCM playback
- Microphone capture
- Voice event routing
- TTS output

**Planned Interface:**

```cpp
class AudioManager {
public:
    // Playback
    void playSound(const std::string& path);
    void playStream(AudioStream* stream);

    // Capture (voice)
    void startCapture();
    void stopCapture();
    AudioBuffer* getRecordedAudio();

    // TTS (future)
    void speak(const std::string& text);
};
```

---

## 7. Application Development Guide

### 7.1 Creating a New App

**Step 1:** Create header file `src/apps/my_app.h`

```cpp
#pragma once

#include "os/app.h"

namespace AOS {

class MyApp : public App {
public:
    // Lifecycle
    void onStart() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    // Metadata
    std::string getName() const override { return "My App"; }

private:
    // App state
    int counter;
};

} // namespace AOS
```

**Step 2:** Implement in `src/apps/my_app.cpp`

```cpp
#include "my_app.h"
#include <iostream>

namespace AOS {

void MyApp::onStart() {
    std::cout << "MyApp: Started" << std::endl;
    counter = 0;
}

void MyApp::update(float deltaTime) {
    // Update logic (60 FPS)
    counter++;
}

void MyApp::render(Renderer& renderer) {
    // Clear screen
    renderer.clear(Color(20, 20, 40));

    // Draw something
    renderer.drawRect(Rect(100, 100, 200, 100), Color::White(), false);
}

void MyApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        // Return to home
        extern AppManager* g_appManager;
        g_appManager->returnToHome();
    }
}

} // namespace AOS
```

**Step 3:** Register in `src/main.cpp`

```cpp
#include "apps/my_app.h"

// Inside main():
g_appManager->registerApp(std::make_unique<AOS::MyApp>());
```

**Step 4:** Add to `CMakeLists.txt`

```cmake
set(AOS_SOURCES
    # ... existing files ...
    src/apps/my_app.cpp
)
```

**Step 5:** Build and test

```bash
cd build
cmake --build .
./aos
```

---

### 7.2 App Lifecycle Best Practices

**onStart():**
- Initialize resources
- Load assets
- Subscribe to events (if using EventBus directly)
- **Do NOT:** Assume state from previous run

**onPause():**
- Save state if needed
- Release temporary resources
- **Do NOT:** Stop critical timers (might resume)

**onResume():**
- Refresh state (e.g., home screen app list)
- Resume animations
- **Do NOT:** Re-initialize everything (use onStart for that)

**onStop():**
- Free all resources
- Unsubscribe from events
- **Do NOT:** Assume you'll be resumed

---

### 7.3 Input Handling Patterns

**Pattern 1: Direct Event Handler**

```cpp
void MyApp::onEvent(const Event& event) {
    switch (event.type) {
        case EventType::KEY_UP:
            moveCursorUp();
            break;
        case EventType::KEY_DOWN:
            moveCursorDown();
            break;
        case EventType::KEY_SELECT:
            activateItem();
            break;
        case EventType::KEY_BACK:
            g_appManager->returnToHome();
            break;
    }
}
```

**Pattern 2: EventBus Subscription**

```cpp
void MyApp::onStart() {
    EventBus::getInstance().subscribe(EventType::VOICE_COMMAND,
        [this](const Event& e) {
            handleVoiceCommand(e.payload);
        });
}
```

**Pattern 3: State Machine**

```cpp
enum class State { MENU, PLAYING, PAUSED };

void MyApp::onEvent(const Event& event) {
    switch (currentState) {
        case State::MENU:
            handleMenuInput(event);
            break;
        case State::PLAYING:
            handleGameInput(event);
            break;
        case State::PAUSED:
            handlePauseInput(event);
            break;
    }
}
```

---

### 7.4 Rendering Guidelines

**Frame Structure:**

```cpp
void MyApp::render(Renderer& renderer) {
    // 1. Clear
    renderer.clear(backgroundColor);

    // 2. Background layers
    drawBackground(renderer);

    // 3. Content
    drawContent(renderer);

    // 4. UI overlays
    drawUI(renderer);

    // 5. Debug (if enabled)
    if (debugMode) {
        drawDebugInfo(renderer);
    }
}
```

**Performance Tips:**
- Keep render calls minimal
- Cache computed layouts
- Avoid allocations in render()
- Target <16ms frame time

**Coordinate System:**
- Origin (0,0) = top-left
- X increases right
- Y increases down

---

### 7.5 App Communication

**Apps should NOT directly reference each other.**

**Use EventBus:**

```cpp
// App A publishes
EventBus::getInstance().publish(
    Event(EventType::CUSTOM, "camera_photo_taken"));

// App B subscribes
EventBus::getInstance().subscribe(EventType::CUSTOM,
    [this](const Event& e) {
        if (e.payload == "camera_photo_taken") {
            refreshGallery();
        }
    });
```

---

## 8. Build & Deployment

### 8.1 Build System (CMake)

**Minimum CMake:** 3.15

**Build Targets:**
- `aos` - Main executable

**Build Types:**
- `Debug` - Debugging symbols, no optimization
- `Release` - Full optimization, stripped

---

### 8.2 Desktop Build (Windows/Linux/macOS)

**Prerequisites:**
- CMake 3.15+
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2019+)
- SDL2 development libraries

**Windows (vcpkg):**

```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.bat

# Install SDL2
./vcpkg install sdl2:x64-windows

# Build A-OS
cd /path/to/aos
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

**Linux:**

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install cmake g++ libsdl2-dev

# Build A-OS
cd /path/to/aos
mkdir build && cd build
cmake ..
make -j$(nproc)
```

**macOS:**

```bash
# Install dependencies
brew install cmake sdl2

# Build A-OS
cd /path/to/aos
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

---

### 8.3 Raspberry Pi Build

**Option A: Native Build (on Pi)**

```bash
# On Raspberry Pi
sudo apt-get update
sudo apt-get install cmake g++ libsdl2-dev

cd ~/aos
mkdir build && cd build
cmake ..
make -j4

# Run
./aos
```

**Option B: Cross-Compile (from Linux PC)**

```bash
# Install cross-compiler
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# Build
mkdir build-pi && cd build-pi
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/pi-toolchain.cmake
make -j$(nproc)

# Deploy
scp aos pi@raspberrypi.local:~/
```

---

### 8.4 Deployment to Raspberry Pi

**Step 1: Copy binary**

```bash
scp aos pi@raspberrypi.local:~/aos/
```

**Step 2: Install systemd service**

Create `/etc/systemd/system/aos.service`:

```ini
[Unit]
Description=A-OS Application Operating System
After=multi-user.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/aos
ExecStart=/home/pi/aos/aos
Restart=always
RestartSec=5

# Run on specific TTY
StandardInput=tty
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

**Step 3: Enable and start**

```bash
sudo systemctl daemon-reload
sudo systemctl enable aos.service
sudo systemctl start aos.service
```

**Step 4: Disable console login (optional)**

```bash
# Disable getty on tty1
sudo systemctl disable getty@tty1.service

# Auto-login (if needed)
sudo systemctl edit getty@tty1.service
```

Add:
```ini
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin pi --noclear %I $TERM
```

---

## 9. Development Workflow

### 9.1 Development Modes

**Mode A: Desktop Simulation (Primary)**

```bash
# Edit code
vim src/apps/home_app.cpp

# Build
cd build
make

# Run
./aos

# Iterate (Ctrl+C, edit, rebuild, run)
```

**Advantages:**
- Fast compilation
- Familiar debugging tools (gdb, lldb, Visual Studio)
- No hardware dependencies
- Easy screen capture

**Limitations:**
- No GPIO
- No Pi-specific hardware

---

**Mode B: Pi Live Run**

```bash
# On PC: build and deploy
make && scp aos pi@raspberrypi.local:~/aos/

# On Pi: run manually
ssh pi@raspberrypi.local
cd aos
./aos

# Ctrl+C to stop, iterate
```

**Advantages:**
- Real hardware
- Test GPIO, camera, etc.
- Still manual control

**Limitations:**
- Slower iteration
- Network dependency

---

**Mode C: Installed OS Mode (Production)**

```bash
# Deploy as systemd service
sudo systemctl restart aos.service

# View logs
sudo journalctl -u aos.service -f
```

**Advantages:**
- Production environment
- Auto-start on boot
- Systemd monitoring

**Limitations:**
- Slowest iteration
- Must use journalctl for logs

---

### 9.2 Debugging Strategies

**Desktop Debugging:**

```bash
# GDB
cd build
gdb ./aos
(gdb) run
(gdb) break HomeApp::onEvent
(gdb) continue
```

**Printf Debugging:**

```cpp
std::cout << "[DEBUG] Focused index: " << focusedIndex << std::endl;
```

**Pi Remote Debugging:**

```bash
# On Pi
gdbserver :1234 ./aos

# On PC
gdb ./aos
(gdb) target remote raspberrypi.local:1234
(gdb) continue
```

---

### 9.3 Testing Strategy

**Current:** Manual testing

**Future:**
- Unit tests (Google Test)
- Integration tests
- Hardware-in-loop tests

---

### 9.4 Version Control

**Git Workflow:**

```bash
# Feature branch
git checkout -b feature/voice-input

# Commit often
git add src/hal/voice_manager.cpp
git commit -m "Add voice capture with ALSA"

# Push when ready
git push origin feature/voice-input

# Merge via PR
```

**Commit Message Format:**

```
<type>: <subject>

<body>

Examples:
feat: Add voice input manager
fix: Resolve app switching crash
docs: Update project spec with voice architecture
refactor: Simplify event dispatch logic
```

---

## 10. Roadmap & Milestones

### MVP (Foundation) ✅ COMPLETE

**Goal:** Prove architecture on desktop

- [x] OSCore with main loop
- [x] AppManager with lifecycle
- [x] EventBus pub/sub
- [x] InputManager (keyboard)
- [x] Renderer (shapes only)
- [x] HomeApp with navigation
- [x] SettingsApp demo
- [x] Desktop simulation mode

---

### Phase 1: Text & Polish (CURRENT)

**Goal:** Professional UI

- [ ] SDL_ttf integration
- [ ] Text rendering in Renderer
- [ ] Home screen with proper labels
- [ ] Font asset pipeline
- [ ] Instructions overlay

**Target:** 1-2 weeks

---

### Phase 2: Raspberry Pi Deployment

**Goal:** Run on real hardware

- [ ] Cross-compile for Pi
- [ ] Fullscreen framebuffer mode
- [ ] GPIO button support
- [ ] systemd service setup
- [ ] Boot-to-UI configuration
- [ ] Performance profiling

**Target:** 2-3 weeks

---

### Phase 3: Voice Foundation (v1.0)

**Goal:** Voice as first-class input

- [ ] ALSA audio capture
- [ ] Voice event types
- [ ] Sherpa-ONNX integration
- [ ] Wake word detection (Porcupine)
- [ ] Voice command routing
- [ ] TTS output (optional)

**Target:** 4-6 weeks

---

### Phase 4: App Ecosystem

**Goal:** Multiple useful apps

- [ ] Camera app (capture, preview)
- [ ] Gallery app (view photos)
- [ ] Voice assistant app
- [ ] System info app
- [ ] Game demo app

**Target:** 6-8 weeks

---

### Phase 5: App Packaging (v2.0)

**Goal:** Install/uninstall apps

- [ ] Define .appbundle format
- [ ] App manifest parser
- [ ] Installation system
- [ ] Permission model
- [ ] Signature verification

**Target:** 8-12 weeks

---

### Future Phases

**Voice Advanced:**
- Multi-language ASR
- Custom wake words
- Voice profiles

**Hardware:**
- Display glasses integration
- Eye tracking
- Hand gestures
- ESP32 coprocessor

**Network:**
- OTA updates
- App marketplace
- Cloud sync

**Developer Tools:**
- App emulator
- Visual app builder
- Deployment tools

---

## 11. Design Decisions & Rationale

### Why C++17, not Rust or C?

**C++17 provides:**
- Low-level control (like C)
- Modern conveniences (smart pointers, lambdas)
- Mature ecosystem (SDL2, libraries)
- Cross-platform support

**vs Rust:**
- Steeper learning curve
- Less embedded library support
- Overkill for single-user device

**vs C:**
- Manual memory management burden
- No RAII
- Verbose event handling

---

### Why SDL2, not custom framebuffer code?

**SDL2 benefits:**
- Cross-platform (desktop + Pi)
- Input abstraction (keyboard, gamepad, touch)
- Framebuffer backend available
- Mature, stable

**vs Pure Framebuffer:**
- Desktop simulation would require separate code
- Input handling from scratch
- No portability

---

### Why single active app, not multi-window?

**Console model advantages:**
- Predictable memory usage
- No Z-ordering complexity
- Deterministic focus
- Simpler UI code
- Battery/power efficient

**Matches user expectation** for device-style OS (phone, console, TV)

---

### Why EventBus, not direct function calls?

**EventBus benefits:**
- Decouples components
- Voice can emit same events as buttons
- Easy to add new event sources
- Testable (inject events)

**Trade-off:**
- Slight overhead (queue)
- Indirect call flow

**Judgment:** Worth it for flexibility

---

### Why Linux substrate, not bare metal?

**Linux provides:**
- Hardware drivers (WiFi, USB, camera)
- Process management (future services)
- Filesystem
- Networking stack
- Security (users, permissions)

**vs Bare Metal:**
- Would have to rewrite drivers
- No networking
- Months of work

**Linux is invisible to user** - best of both worlds

---

### Why desktop-first development?

**Speeds up iteration:**
- Compile: 5-10s (vs 30-60s on Pi)
- Run: instant (vs SSH deploy)
- Debug: native tools (vs cross-debug)
- Screen: easy capture (vs camera setup)

**80% of code is platform-agnostic** - test on desktop first

---

### Why no exceptions?

**Embedded considerations:**
- Binary size
- Unpredictable performance (stack unwinding)
- RTOS compatibility (future)

**Alternative:** Return bool, log errors, assert impossible

---

## Appendix A: Glossary

| Term | Definition |
|------|------------|
| **A-OS** | Application Operating System (this project) |
| **HAL** | Hardware Abstraction Layer |
| **App** | User-facing program (Home, Settings, etc.) |
| **Service** | Background system component |
| **EventBus** | Pub/sub communication system |
| **OSCore** | Main coordinator and loop |
| **AppManager** | Lifecycle and switching manager |
| **Renderer** | Display abstraction |
| **InputManager** | Input normalization |
| **SDL2** | Simple DirectMedia Layer (graphics/input library) |
| **ASR** | Automatic Speech Recognition |
| **TTS** | Text-to-Speech |
| **ALSA** | Advanced Linux Sound Architecture |
| **Framebuffer** | Direct pixel access (`/dev/fb0`) |
| **systemd** | Linux init system |

---

## Appendix B: Key Files Quick Reference

| File | Description | Lines |
|------|-------------|-------|
| `main.cpp` | Entry point, app registration | ~60 |
| `os_core.h/cpp` | Main loop, initialization | ~150 |
| `app_manager.h/cpp` | App lifecycle management | ~150 |
| `event_bus.h/cpp` | Event pub/sub system | ~100 |
| `app.h` | Base app interface | ~50 |
| `input_manager.h/cpp` | Input abstraction | ~100 |
| `renderer.h/cpp` | Display abstraction | ~150 |
| `home_app.h/cpp` | Launcher application | ~200 |
| `CMakeLists.txt` | Build configuration | ~65 |

**Total Core Codebase:** ~1,000 lines (very manageable)

---

## Appendix C: External Resources

**SDL2 Documentation:**
- https://wiki.libsdl.org/

**Raspberry Pi:**
- https://www.raspberrypi.com/documentation/

**Sherpa-ONNX (Voice):**
- https://github.com/k2-fsa/sherpa-onnx

**CMake:**
- https://cmake.org/documentation/

---

## Document History

| Version | Date | Changes |
|---------|------|---------|
| 0.1.0 | 2026-01-09 | Initial comprehensive specification |

---

**END OF SPECIFICATION**

This document is the **single source of truth** for A-OS architecture, standards, and development practices. When in doubt, refer here.
