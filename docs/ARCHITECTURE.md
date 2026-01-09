# A-OS Architecture Documentation

## Overview

A-OS is structured as a layered system where Linux provides hardware abstraction, and our OS layer provides the application environment.

```
┌─────────────────────────────────────────┐
│         Applications (Apps)             │
│  Home, Settings, Games, Voice UI, etc.  │
├─────────────────────────────────────────┤
│         Application Framework           │
│    App, AppManager, EventBus            │
├─────────────────────────────────────────┤
│           UI & Rendering                │
│    Renderer (SDL2 abstraction)          │
├─────────────────────────────────────────┤
│   Hardware Abstraction Layer (HAL)      │
│  InputManager, AudioManager, etc.       │
├─────────────────────────────────────────┤
│             OS Core                     │
│    Main loop, subsystem coordination    │
├─────────────────────────────────────────┤
│          SDL2 / ALSA / Linux            │
│    Hardware drivers, kernel, processes  │
└─────────────────────────────────────────┘
```

## Core Subsystems

### 1. EventBus (Nervous System)

**Purpose:** Central communication hub for the entire OS.

**Design Pattern:** Publisher-Subscriber

**Key Concepts:**
- All input becomes events (keyboard → `KEY_SELECT`, voice → `VOICE_FINAL`)
- Apps subscribe to events they care about
- Decouples input sources from app logic
- Enables voice, buttons, and gestures to be equivalent inputs

**Event Flow:**
```
Input Device → InputManager → EventBus.publish()
→ EventBus.processEvents() → App.onEvent()
```

**Example:**
```cpp
// Subscribe to button events
EventBus::getInstance().subscribe(EventType::KEY_SELECT,
    [this](const Event& e) {
        // Handle selection
    });

// Publish an event
EventBus::getInstance().publish(Event(EventType::KEY_SELECT));
```

### 2. AppManager (Process Manager)

**Purpose:** Manages application lifecycle, similar to Android's ActivityManager.

**Key Responsibilities:**
- Register installed apps
- Launch and switch between apps
- Enforce single-app-at-a-time model
- Call lifecycle methods (onStart, onPause, onResume, onStop)

**Lifecycle Flow:**
```
App A running → User selects App B
→ App A: onPause() → onStop()
→ App B: onStart() → onResume()
→ App B now active
```

**Why Single App?**
- Memory efficiency (512MB on Pi Zero 2 W)
- Deterministic behavior
- Console-like experience
- Clear focus model

### 3. App (Base Class)

**Purpose:** Interface that all applications implement.

**Lifecycle Methods:**
- `onStart()` - App is launched (load resources)
- `onResume()` - App gains focus (resume animations)
- `onPause()` - App loses focus (pause timers)
- `onStop()` - App is closed (cleanup)

**Frame Methods:**
- `update(deltaTime)` - Logic update (60 FPS)
- `render(renderer)` - Drawing (60 FPS)
- `onEvent(event)` - Input handling

**Example App:**
```cpp
class GameApp : public App {
    void onStart() override {
        // Load game assets
    }

    void update(float dt) override {
        // Update game logic
    }

    void render(Renderer& r) override {
        // Draw game
    }

    void onEvent(const Event& e) override {
        if (e.type == EventType::KEY_BACK) {
            // Return to home
        }
    }
};
```

### 4. Renderer (Display Abstraction)

**Purpose:** Hardware-independent drawing API.

**Backend:** SDL2 (framebuffer on Pi, window on desktop)

**Primitives:**
- `clear(color)` - Fill screen
- `drawRect(rect, color, filled)` - Rectangles
- `drawText(text, x, y, color)` - Text (TODO: SDL_ttf)
- `present()` - Swap buffers

**Why SDL2?**
- Cross-platform (desktop + Pi)
- Direct framebuffer access
- No X11/Wayland needed
- Input + audio included
- Well-tested, stable

### 5. InputManager (Input Abstraction)

**Purpose:** Convert hardware input to OS events.

**Current Support:**
- Keyboard (desktop simulation)
- Future: GPIO buttons, gamepad, voice

**Mapping (Desktop):**
```
Arrow keys → KEY_UP/DOWN/LEFT/RIGHT
Enter      → KEY_SELECT
Escape     → KEY_BACK
```

**Future (Raspberry Pi):**
```
GPIO pin 17 → KEY_SELECT
USB gamepad → KEY_* events
Voice "open settings" → VOICE_COMMAND
```

### 6. OSCore (Main Loop)

**Purpose:** Initialize subsystems and run the main loop.

**Main Loop (60 FPS):**
```cpp
while (running) {
    // 1. Poll input
    inputManager->pollInput();

    // 2. Process events
    EventBus::getInstance().processEvents();

    // 3. Update active app
    appManager->update(deltaTime);

    // 4. Render active app
    renderer->clear();
    appManager->render(*renderer);
    renderer->present();

    // 5. Frame cap (VSYNC)
}
```

**Why 60 FPS?**
- Smooth animations
- Responsive input
- Standard for UI systems
- VSYNC prevents tearing

## Design Patterns Used

### 1. Singleton
- `EventBus::getInstance()` - One event bus for entire OS

### 2. Observer Pattern
- EventBus subscription model

### 3. Strategy Pattern
- App interface allows different implementations

### 4. Facade Pattern
- Renderer hides SDL2 complexity

## Data Flow Example: User Presses "Select"

1. **Input Layer:** SDL detects SDLK_RETURN keypress
2. **HAL Layer:** `InputManager` maps to `KEY_SELECT` event
3. **Event Layer:** `EventBus.publish(KEY_SELECT)`
4. **App Layer:** `HomeApp.onEvent()` receives event
5. **App Logic:** Home app launches selected app
6. **AppManager:** Switches active app (lifecycle callbacks)
7. **Render Layer:** New app renders on next frame

## Memory Management

**Ownership Model:**
- Apps: owned by `AppManager` (unique_ptr)
- Subsystems: owned by `OSCore` (unique_ptr)
- EventBus: singleton (static lifetime)

**No Manual `new`/`delete`:**
- Use `std::unique_ptr` and `std::make_unique`
- RAII for automatic cleanup
- No memory leaks

## Threading Model (Current)

**Single-threaded:**
- Main loop on one thread
- Simplifies development
- Sufficient for MVP

**Future (v1+):**
- Audio capture thread
- ASR processing thread
- Camera capture thread
- Apps still single-threaded (communicate via events)

## Platform Abstraction Strategy

**Desktop vs. Pi:**
- Same C++ code
- Same SDL2 API
- Different compilation targets
- Different SDL2 backends (window vs framebuffer)

**Conditional Code (when needed):**
```cpp
#ifdef __linux__
    // Pi-specific code (GPIO, etc.)
#elif _WIN32
    // Windows-specific code
#endif
```

**Keep Minimal:**
- Most code should be platform-agnostic
- Push platform differences into HAL

## Future Expansion Points

### Voice Input (v1)
- Add `VoiceManager` in HAL
- Integrate Sherpa-ONNX
- Emit `VOICE_*` events to EventBus
- Apps don't know voice vs button

### Camera (v1+)
- Add `CameraManager` in HAL
- Background capture thread
- Apps request frames via API

### Display Glasses (v2+)
- Renderer backend selection
- Same drawing API, different output
- Potentially stereo rendering

### Gesture Input (v2+)
- Add `GestureManager` in HAL
- MediaPipe integration
- Emit gesture events (e.g., `GESTURE_SWIPE_LEFT`)

## Performance Considerations

### Target Hardware: Raspberry Pi 5
- 4-core ARM Cortex-A76 (2.4 GHz)
- Up to 16GB RAM
- VideoCore VII GPU (optional use)
- Plenty of headroom for voice AI

### Optimization Strategy
1. **Start simple** - CPU rendering fine for UI
2. **Profile first** - Don't optimize prematurely
3. **GPU later** - Add OpenGL ES if needed
4. **Voice offload** - Dedicated thread for ASR

### Memory Budget (Conservative)
- OS Core: ~50MB
- SDL2/libs: ~30MB
- Active app: ~100MB
- Voice model: ~200MB (Pi 5)
- Headroom: ~3.5GB (on 4GB Pi 5)

## Debugging Strategy

### Desktop Development
- Standard debuggers (gdb, VS debugger)
- Fast iteration
- Easy breakpoints

### Raspberry Pi Debugging
- SSH + gdb
- Serial console (if needed)
- Log files
- LED indicators for system states

## Next Steps

See the main README.md for build instructions and roadmap.

For application development guide, see APPS.md (TODO).

---

Last updated: 2026-01-09
