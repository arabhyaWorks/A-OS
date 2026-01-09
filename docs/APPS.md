# A-OS Applications

**Last Updated:** 2026-01-09
**Version:** v0.2

This document describes all applications included in A-OS.

---

## Application Overview

A-OS now includes **5 built-in applications** that demonstrate the system's capabilities:

| App Name | Purpose | Status |
|----------|---------|--------|
| Home | App launcher/home screen | ✅ Complete |
| Settings | System settings interface | ✅ Complete |
| Camera | Camera preview and capture | ✅ Complete |
| System Info | Hardware/system information | ✅ Complete |
| Media Player | Audio playback interface | ✅ Complete |

---

## 1. Home App

**Name:** Home
**Type:** System App (Always first)
**Files:** `src/apps/home_app.h`, `src/apps/home_app.cpp`

### Description
The Home app is the launcher and main entry point of A-OS. It displays all installed applications in a grid layout and allows navigation between them.

### Features
- Grid display of all installed apps
- Arrow key navigation with visual focus
- Launch apps with Enter key
- Always the first registered app

### Controls
- **Arrow Keys:** Navigate up/down through apps
- **Enter:** Launch selected app
- **ESC:** (N/A - already at home)

### Technical Details
- Subscribes to `SYSTEM_STARTUP` event to refresh app list
- Queries `AppManager` for installed apps on resume
- Displays app tiles with focused/unfocused states

---

## 2. Settings App

**Name:** Settings
**Type:** Demo App
**Files:** `src/apps/settings_app.h`, `src/apps/settings_app.cpp`

### Description
A demonstration settings interface showing how apps can implement animated UI elements and respond to events.

### Features
- Animated pulsing box (demonstrates update loop)
- Text labels and instructions
- Return to home functionality

### Controls
- **ESC:** Return to Home

### Technical Details
- Uses `animationTime` to create smooth animations
- Simple demonstration of app lifecycle

### Future Enhancements
- Network settings
- Display settings
- Audio settings
- System information
- App management

---

## 3. Camera App

**Name:** Camera
**Type:** Demo App
**Files:** `src/apps/camera_app.h`, `src/apps/camera_app.cpp`

### Description
Simulated camera preview and photo capture interface demonstrating real-time rendering and user interaction.

### Features
- Animated "video feed" effect
- Live crosshair overlay
- Photo capture with flash effect
- Photo counter
- Visual feedback on capture

### Controls
- **Enter:** Capture photo
- **ESC:** Return to Home

### Technical Details
- Uses `previewTime` for animated wave effect
- Implements capture flash animation (0.3s duration)
- Tracks photo count across session
- Simulates real-time preview with animated horizontal bars

### Future Enhancements
- V4L2 camera integration (Linux/Pi)
- Actual image capture to storage
- Gallery integration
- Photo review mode
- Camera settings (resolution, quality)

---

## 4. System Info App

**Name:** System Info
**Type:** Information App
**Files:** `src/apps/sysinfo_app.h`, `src/apps/sysinfo_app.cpp`

### Description
Displays comprehensive system information including OS version, hardware details, and uptime.

### Features
- OS name and version
- Platform detection (Windows/Linux)
- CPU information (cores)
- Memory information
- Graphics system info
- Real-time uptime counter (HH:MM:SS)
- Target hardware display

### Controls
- **ESC:** Return to Home

### Technical Details
- Platform-specific code using `#ifdef _WIN32`
- Uses Windows API (`GetSystemInfo`, `GlobalMemoryStatusEx`) on Windows
- Uses POSIX APIs (`uname`, `sysconf`) on Linux/Unix
- Updates uptime every second in `update()` loop
- Refreshes all info on `onResume()`

### Displayed Information

**On Windows:**
- Platform: "Windows (Desktop Simulation)"
- CPU: x86_64 with core count
- Memory: Total physical RAM in MB
- Graphics: SDL2 Renderer
- Display: 1280x720 (Simulated)

**On Linux/Raspberry Pi:**
- Platform: Linux
- Kernel: Kernel version
- Architecture: ARM64/x86_64
- Memory: Total system RAM
- Target: Raspberry Pi 5

### Future Enhancements
- Temperature monitoring (CPU, GPU)
- Network status (IP, WiFi strength)
- Disk space information
- Battery status (for portable devices)
- Process/memory usage graphs

---

## 5. Media Player App

**Name:** Media Player
**Type:** Demo App
**Files:** `src/apps/media_app.h`, `src/apps/media_app.cpp`

### Description
Simulated media player with playback controls, progress tracking, and multiple tracks.

### Features
- 5 demo tracks with artist/title information
- Play/Pause control
- Track navigation (next/previous)
- Progress bar with time display
- Animated "vinyl" effect when playing
- Auto-play next track when finished
- Visual playback state indicator

### Controls
- **Enter:** Play/Pause toggle
- **Left Arrow:** Previous track
- **Right Arrow:** Next track
- **ESC:** Return to Home

### Technical Details
- Three playback states: STOPPED, PLAYING, PAUSED
- Tracks defined as static array with title, artist, duration
- Progress calculated in `update()` loop
- Time formatting (MM:SS) for position and duration
- Auto-loops to next track when current track finishes

### Demo Tracks
1. **Cosmic Journey** - Space Orchestra (4:05)
2. **Digital Dreams** - Synth Wave (3:18)
3. **Morning Light** - Acoustic Trio (3:32)
4. **Night Drive** - Electric Beats (4:27)
5. **Ocean Waves** - Nature Sounds (3:00)

### Future Enhancements
- Actual audio playback via ALSA/SDL_mixer
- File browser for audio files
- Playlist management
- Album art display from file metadata
- Equalizer/audio effects
- Shuffle and repeat modes
- Volume control

---

## Application Architecture

### App Lifecycle

All apps inherit from `AOS::App` and implement these methods:

```cpp
void onStart()    // Called when app first launches
void onStop()     // Called when app is closed
void onPause()    // Called when app goes to background
void onResume()   // Called when app returns to foreground
void update(float deltaTime)  // Called every frame
void render(Renderer& renderer)  // Called every frame
void onEvent(const Event& event)  // Called for input events
```

### Event Handling

Apps receive events through `onEvent()`:
- `KEY_UP`, `KEY_DOWN`, `KEY_LEFT`, `KEY_RIGHT` - Navigation
- `KEY_SELECT` - Confirm/Action (Enter/Space)
- `KEY_BACK` - Back/Cancel (ESC)

### Returning to Home

All apps should handle the back button:

```cpp
void MyApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    }
}
```

---

## Adding New Applications

### 1. Create Header File

Create `src/apps/myapp.h`:

```cpp
#pragma once
#include "os/app.h"
#include "ui/renderer.h"

namespace AOS {

class MyApp : public App {
public:
    MyApp() = default;
    ~MyApp() override = default;

    void onStart() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "My App"; }

private:
    // App state variables
};

} // namespace AOS
```

### 2. Create Implementation File

Create `src/apps/myapp.cpp`:

```cpp
#include "myapp.h"
#include "os/app_manager.h"
#include <iostream>

extern AOS::AppManager* g_appManager;

namespace AOS {

void MyApp::onStart() {
    std::cout << "MyApp: Started" << std::endl;
}

void MyApp::update(float deltaTime) {
    // Update logic
}

void MyApp::render(Renderer& renderer) {
    // Draw UI
    renderer.drawText("My App", 20, 25, Color::White(), 28);
}

void MyApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    }
}

} // namespace AOS
```

### 3. Register in CMakeLists.txt

Add to `AOS_SOURCES`:

```cmake
set(AOS_SOURCES
    # ... existing files ...
    src/apps/myapp.cpp
)
```

### 4. Register in main.cpp

Add includes and registration:

```cpp
#include "apps/myapp.h"

// In main():
g_appManager->registerApp(std::make_unique<AOS::MyApp>());
```

### 5. Build and Test

```bash
cd build
mingw32-make -j8
./aos.exe
```

---

## App Development Best Practices

### 1. Resource Management
- Initialize resources in `onStart()`
- Clean up in `onStop()`
- Use RAII principles for memory management

### 2. Performance
- Keep `update()` and `render()` fast (target 60 FPS)
- Avoid heavy operations in the main loop
- Cache computed values when possible

### 3. User Experience
- Always respond to KEY_BACK event
- Provide visual feedback for actions
- Show clear instructions at bottom of screen
- Use consistent color scheme

### 4. Debugging
- Use `std::cout` for debug logging
- Check console output for app lifecycle events
- Verify all events are handled

### 5. Code Style
- Follow A-OS coding standards (see `CODING_RULES.md`)
- Use descriptive variable names
- Add comments for complex logic
- Keep functions small and focused

---

## Testing Applications

### Manual Testing Checklist

For each app:
- [ ] App appears in Home screen
- [ ] App launches successfully (Enter)
- [ ] App renders correctly (no visual glitches)
- [ ] All controls respond properly
- [ ] ESC returns to Home without crash
- [ ] Can re-launch app multiple times
- [ ] No memory leaks (check over extended use)
- [ ] 60 FPS performance maintained

### Known Limitations

**Current (Desktop Simulation):**
- All apps are simulations/demos
- No actual hardware integration
- No real audio/video playback
- No persistent storage

**Future (Raspberry Pi):**
- Will integrate real hardware
- GPIO button support
- Camera module support
- Audio playback via ALSA
- Storage to SD card

---

## Future App Ideas

### Phase 2 (Raspberry Pi Deployment)
- **GPIO Control** - Control GPIO pins for external hardware
- **Network Config** - WiFi setup and network management
- **File Browser** - Navigate and view files

### Phase 3 (Voice Integration)
- **Voice Assistant** - Voice command interface
- **Voice Notes** - Record and playback voice memos
- **Voice Settings** - Configure ASR/TTS

### Phase 4 (Advanced Features)
- **Robot Control** - Control robot actuators
- **Computer Vision** - Object detection/tracking
- **Web Browser** - Simple web interface
- **Games** - Simple interactive games
- **Photo Gallery** - View captured photos
- **Video Player** - Play video files

---

## Troubleshooting

### App doesn't appear in Home screen
- Check if registered in `main.cpp`
- Verify `getName()` returns valid string
- Check console for registration message

### App crashes on launch
- Check `onStart()` implementation
- Verify all pointers are valid
- Check console for error messages

### ESC key doesn't work
- Verify `onEvent()` handles `KEY_BACK`
- Check if `g_appManager` is not null
- Verify `returnToHome()` is called

### Rendering issues
- Check `render()` implementation
- Verify Renderer API calls are correct
- Check coordinates are within screen bounds
- Verify colors have valid RGBA values (0-255)

---

## Related Documentation

- [PROJECT_SPEC.md](PROJECT_SPEC.md) - Complete technical specification
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture details
- [CODING_RULES.md](CODING_RULES.md) - Code standards and conventions
- [BUILD_STATUS.md](BUILD_STATUS.md) - Current build status and testing

---

**A-OS v0.2** - Five applications demonstrating the complete app framework with text rendering, navigation, and interactive features.
