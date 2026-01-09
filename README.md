# A-OS (Application Operating System) v0.2

A console-style Application Operating System for Raspberry Pi, built on Linux substrate with full-screen UI, unified input system, and voice capabilities.

## Architecture Overview

A-OS is **NOT** a Linux distribution or desktop environment. It's an application-centric OS that runs on top of Linux, similar to:
- PlayStation OS
- SteamOS UI layer
- Android System UI (conceptually)

**Key Features:**
- Full-screen, single-app-at-a-time experience
- Unified event system (buttons, keyboard, gamepad, voice)
- Native C++ application framework
- Desktop simulation mode for fast development
- Raspberry Pi 5 deployment target

## Project Structure

```
aos/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── os/                      # Core OS subsystems
│   │   ├── event_bus.h/.cpp     # Central event system
│   │   ├── app_manager.h/.cpp   # App lifecycle manager
│   │   ├── app.h                # Base App interface
│   │   └── os_core.h/.cpp       # Main OS coordinator
│   ├── hal/                     # Hardware Abstraction Layer
│   │   ├── input_manager.h/.cpp # Input handling
│   │   └── audio_manager.h/.cpp # Audio system (stub)
│   ├── ui/                      # UI/Rendering
│   │   └── renderer.h/.cpp      # SDL2 renderer abstraction
│   └── apps/                    # Built-in applications
│       ├── home_app.h/.cpp      # Home/launcher screen
│       └── settings_app.h/.cpp  # Settings app
├── assets/                      # Images, fonts, sounds
├── build/                       # Build output
└── CMakeLists.txt              # Build configuration
```

## Prerequisites

### Windows (Desktop Simulation)
- CMake 3.15+
- C++17 compiler (MSVC, MinGW, or Clang)
- SDL2 development libraries

### Linux/Raspberry Pi
- CMake 3.15+
- GCC/G++ with C++17 support
- SDL2 development package

## Installing SDL2

### Windows - MSYS2/MinGW (Recommended)
```bash
# Install MSYS2 first from https://www.msys2.org/
# Then in MSYS2 MINGW64 terminal:
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

### Windows - vcpkg (Alternative)
```bash
vcpkg install sdl2:x64-windows
vcpkg install sdl2-ttf:x64-windows
```

### Linux/Raspberry Pi
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

## Building A-OS

### Desktop Simulation Mode (Windows/Linux)

1. **Create build directory:**
```bash
mkdir build
cd build
```

2. **Configure with CMake:**
```bash
# Windows with MSYS2/MinGW (MINGW64 terminal)
cmake .. -G "MinGW Makefiles"

# Windows with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake

# Linux/Mac
cmake ..
```

3. **Build:**
```bash
# Windows with MinGW
mingw32-make -j8

# Windows with vcpkg/MSVC
cmake --build . --config Release

# Linux/Mac
make -j8
```

4. **Run:**
```bash
# Windows (MSYS2/MinGW)
./aos.exe

# Windows (vcpkg/MSVC)
.\Release\aos.exe

# Linux/Mac
./aos
```

For detailed build instructions, see [docs/BUILD_MSYS2.md](docs/BUILD_MSYS2.md) or [docs/BUILD_COMMANDS.md](docs/BUILD_COMMANDS.md).

### Raspberry Pi Deployment (Future)

Cross-compilation and deployment instructions will be added in Phase 2.

## Controls (Desktop Simulation)

| Key        | Action           |
|------------|------------------|
| Arrow Keys | Navigate menu    |
| Enter      | Select/Confirm   |
| Escape     | Back/Return Home |

## Development Workflow

A-OS is designed for **desktop-first development**:

1. **Develop on your PC** - Fast compilation, easy debugging
2. **Test in SDL window** - Simulates full-screen behavior
3. **Deploy to Pi** - Same binary runs on Raspberry Pi (with recompilation)

### Current Status: Phase 1 Complete ✅

**v0.2 - Text Rendering Complete & Tested (2026-01-09):**
- ✅ SDL2 window/rendering
- ✅ SDL_ttf text rendering with font caching
- ✅ Event bus system with proper event forwarding
- ✅ App lifecycle management (onStart, onPause, onResume, onStop)
- ✅ Home screen with visual navigation and text labels
- ✅ Settings app demo with proper event handling
- ✅ Keyboard input mapping (Arrow keys, Enter, ESC)
- ✅ Full app switching (Home ↔ Settings)
- ✅ Build verified on Windows (MSYS2/MinGW)

**What's Next (Phase 2 - Raspberry Pi):**
- GPIO button support (Pi)
- Fullscreen framebuffer mode (Pi)
- systemd service for boot-to-UI
- Cross-compilation setup
- Performance testing on Pi 5

**Future (Phase 3 - Voice):**
- Voice input system
- ALSA audio integration
- Sherpa-ONNX ASR
- TTS output
- Camera framework

## Application Development

Apps inherit from `AOS::App` and implement lifecycle methods:

```cpp
class MyApp : public AOS::App {
public:
    void onStart() override {
        // Initialize app
    }

    void update(float deltaTime) override {
        // Update logic each frame
    }

    void render(AOS::Renderer& renderer) override {
        // Draw UI
    }

    void onEvent(const AOS::Event& event) override {
        // Handle input events
    }

    std::string getName() const override {
        return "My App";
    }
};
```

Register in `main.cpp`:
```cpp
g_appManager->registerApp(std::make_unique<MyApp>());
```

## Architecture Highlights

### Event-Driven Design
All input (buttons, keyboard, voice) becomes events. Apps never directly read hardware.

### Single Active App
Only one app runs in the foreground at a time - true console experience.

### Hardware Abstraction
Apps use `Renderer` and `EventBus` - hardware details hidden in HAL.

### Desktop Simulation
Same code runs on desktop and Pi. SDL handles platform differences.

## Roadmap

### Phase 1 ✅ (Complete - v0.2)
- Core architecture
- Desktop simulation
- Text rendering with SDL_ttf
- Event forwarding system
- Basic apps (Home, Settings)
- Build system (CMake + MSYS2)

### Phase 2 (Current - Pi Deployment)
- Raspberry Pi 5 deployment
- GPIO button support
- systemd service
- Boot-to-UI
- Framebuffer rendering

### Phase 3 (Voice Input)
- Voice input events
- On-device ASR (Sherpa-ONNX)
- Voice navigation
- Wake word detection
- TTS output

### Phase 4 (Product Features)
- App packaging system
- OTA updates
- Camera integration
- Permission system
- Additional apps

## Hardware Support

**Current:**
- Raspberry Pi 5 (primary target)
- Desktop PC (simulation)

**Future:**
- Raspberry Pi Zero 2 W
- Display glasses (2D output)
- Eye tracking input
- Hand gesture input

## License

TBD

## Contributing

This is currently a development/specification phase project. Contribution guidelines will be established as the project matures.

## Contact

For questions about A-OS architecture or development, refer to the technical specification document.

---

**A-OS**: A Linux-powered, console-style Application Operating System that transforms Raspberry Pi into a dedicated, voice-enabled, app-centric device.
