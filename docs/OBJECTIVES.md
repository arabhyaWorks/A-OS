# A-OS Project Objectives & Goals

**Strategic Vision and Success Criteria**

---

## Primary Mission

**Build a console-style Application Operating System that transforms Raspberry Pi into a dedicated, voice-enabled, app-centric device—running on Linux substrate but hiding Linux completely.**

---

## Core Objectives

### 1. Device Experience (Not PC Experience)

**Objective:** Create an OS that feels like a purpose-built product.

**Success Criteria:**
- ✅ User never sees Linux
- ✅ Boot directly to custom UI (no login)
- ✅ Single active app at all times
- ✅ No terminal, no shell, no desktop
- ✅ Deterministic, predictable behavior

**Anti-Goals:**
- ❌ Not a Linux distribution
- ❌ Not a desktop replacement
- ❌ Not multi-user

---

### 2. Voice as First-Class Input

**Objective:** Treat voice like buttons and keyboard—core input device.

**Success Criteria:**
- ✅ On-device ASR (no cloud required)
- ✅ Voice events unified with button events
- ✅ Wake word support
- ✅ Voice navigation ("open settings")
- ✅ App-level voice commands

**Technical Target:**
- Latency: <500ms from speech end to action
- Accuracy: >90% for common commands
- Works offline

---

### 3. Native Performance

**Objective:** Smooth, responsive, 60 FPS experience.

**Success Criteria:**
- ✅ 60 FPS UI rendering
- ✅ <16ms frame time
- ✅ Sub-second app launches
- ✅ Instant input response

**Technical Stack:**
- C++17 for performance
- SDL2 for low-level graphics
- ALSA for audio
- No web stack bloat

---

### 4. Desktop-First Development

**Objective:** Fast iteration without hardware dependency.

**Success Criteria:**
- ✅ Same code runs on Windows/Linux/macOS
- ✅ Build time <10 seconds
- ✅ SDL window simulates device screen
- ✅ Keyboard simulates buttons
- ✅ No SD card reflashing during dev

**Workflow:**
```
Edit → Build → Run → Test (all on PC)
  ↓ (when ready)
Deploy to Pi
```

---

### 5. Application Ecosystem

**Objective:** Enable third-party apps (future).

**Success Criteria:**
- ✅ Clean App SDK
- ✅ Lifecycle hooks (onStart, onPause, etc.)
- ✅ Event-driven architecture
- ✅ Sandboxed execution
- ✅ App packaging format

**Phase 1:** Built-in apps only
**Phase 2:** External app support

---

## Technical Goals

### Architecture

| Goal | Status | Target |
|------|--------|--------|
| Modular HAL | ✅ Done | v0.1 |
| EventBus pub/sub | ✅ Done | v0.1 |
| App lifecycle | ✅ Done | v0.1 |
| Text rendering | 🚧 In Progress | v0.2 |
| Voice input | ⏳ Planned | v1.0 |
| App packaging | ⏳ Planned | v2.0 |

### Performance

| Metric | Target | Pi Zero 2 W | Pi 5 |
|--------|--------|-------------|------|
| **Frame Rate** | 60 FPS | ✅ Achievable | ✅ Easy |
| **Frame Time** | <16.67ms | ✅ Target | ✅ Target |
| **App Launch** | <1s | ✅ Achievable | ✅ Easy |
| **Boot Time** | <10s | ⏳ Future | ✅ Achievable |
| **Memory** | <128MB | ✅ Target | ✅ Easy |

### Portability

| Platform | Status | Notes |
|----------|--------|-------|
| **Windows Desktop** | ✅ Supported | Development primary |
| **Linux Desktop** | ✅ Supported | Development |
| **macOS Desktop** | ✅ Should work | Untested |
| **Raspberry Pi 5** | ⏳ Target | Primary deployment |
| **Pi Zero 2 W** | ⏳ Future | Secondary target |

---

## User Experience Goals

### Boot Experience

**Current State:**
```
Power ON → Linux boot logs → Login prompt
```

**Target State:**
```
Power ON → Splash screen → Home screen (5-10s)
```

**No visible Linux.**

---

### App Launching Experience

**Target Flow:**

1. User sees Home screen with app tiles
2. Navigate with arrow keys or voice ("open camera")
3. Press Enter or say command
4. App launches <1 second
5. Full-screen app UI

**No windows. No multitasking. Console-like.**

---

### Input Experience

**Unified Input Model:**

| Input Source | Maps To | Example |
|-------------|---------|---------|
| Keyboard Arrow Up | `KEY_UP` | Menu navigation |
| Gamepad D-Pad Up | `KEY_UP` | Menu navigation |
| GPIO Button 1 | `KEY_UP` | Menu navigation |
| Voice "up" | `KEY_UP` | Menu navigation |

**All inputs become events. Apps never care about source.**

---

## Development Goals

### Code Quality

| Metric | Target | Current |
|--------|--------|---------|
| **Compile Warnings** | 0 | ✅ 0 |
| **Memory Leaks** | 0 | ✅ 0 (checked) |
| **Code Coverage** | >80% | ⏳ No tests yet |
| **Documentation** | All public APIs | ✅ Headers documented |

### Developer Experience

**Goals:**
- Simple SDK (like console SDKs)
- Clear examples
- Fast compile times
- Good error messages

**App Creation Flow:**
```cpp
// 1. Subclass App
class MyApp : public AOS::App {
    void onStart() override { /* init */ }
    void update(float dt) override { /* logic */ }
    void render(Renderer& r) override { /* draw */ }
};

// 2. Register in main()
g_appManager->registerApp(std::make_unique<MyApp>());

// Done!
```

---

## Success Metrics (By Phase)

### MVP (v0.1) ✅ ACHIEVED

- [x] OSCore running on desktop
- [x] AppManager switching apps
- [x] EventBus working
- [x] Home screen navigable
- [x] Settings app demo

**Result:** Architecture proven.

---

### Phase 1 (v0.2) - Polish 🚧 IN PROGRESS

**Goals:**
- [ ] Text rendering (SDL_ttf)
- [ ] Professional-looking Home UI
- [ ] Smooth animations
- [ ] Icon support

**Success:** Looks like a real OS.

---

### Phase 2 (v0.5) - Hardware

**Goals:**
- [ ] Runs on Raspberry Pi 5
- [ ] Fullscreen framebuffer
- [ ] GPIO buttons work
- [ ] Boots to UI

**Success:** Real device deployment.

---

### Phase 3 (v1.0) - Voice

**Goals:**
- [ ] On-device ASR (Sherpa-ONNX)
- [ ] Voice events working
- [ ] Voice navigation
- [ ] Wake word detection

**Success:** "Voice OS" achieved.

---

### Phase 4 (v1.5) - Apps

**Goals:**
- [ ] Camera app
- [ ] Gallery app
- [ ] Voice assistant app
- [ ] System info app

**Success:** Useful product.

---

### Phase 5 (v2.0) - Packaging

**Goals:**
- [ ] App packaging format
- [ ] Install/uninstall
- [ ] App permissions
- [ ] Signature verification

**Success:** Third-party app platform.

---

## Non-Goals (What We're NOT Building)

### ❌ Not Building

1. **General-Purpose OS**
   - Not competing with Ubuntu/Debian
   - Not multi-user
   - Not for servers

2. **Desktop Environment**
   - Not replacing GNOME/KDE
   - No windowing system
   - No overlapping windows

3. **Android Alternative**
   - Not APK-compatible
   - Different app model
   - Simpler, more constrained

4. **Web Platform**
   - No browser engine
   - No HTML/CSS/JS
   - Native apps only

5. **Cloud-Dependent System**
   - Must work offline
   - On-device AI
   - Local-first

---

## Long-Term Vision (3-5 Years)

### Possible Future Directions

**1. Display Glasses Integration**
- 2D output to glasses
- Eye tracking input
- Hand gesture control

**2. ESP32 Coprocessor**
- ESP32 handles real-time I/O
- Pi handles complex apps
- UART/SPI bridge

**3. Developer Ecosystem**
- App marketplace
- Visual app builder
- Emulator
- Deployment tools

**4. AI Integration**
- On-device LLM (small models)
- Computer vision
- Intent understanding

**5. Multi-Device**
- Multiple A-OS devices
- Shared state
- Device handoff

---

## Why This Matters

### Problem Statement

**Current Raspberry Pi Software:**
- Generic Linux distros (too complex)
- Desktop environments (wrong UX)
- Manual configuration hell
- No unified input story
- Voice is afterthought

**A-OS Solution:**
- Purpose-built OS
- Console-like UX
- Pre-configured
- Unified input (buttons + voice)
- Voice-first design

---

### Target Use Cases

**1. Voice-Controlled Robot**
- A-OS on Pi
- Apps for motor control, sensors
- Voice commands ("move forward")

**2. Smart Display**
- Home automation UI
- Voice assistant
- Camera viewer

**3. Handheld Device**
- Game console style
- Buttons + screen
- Native apps

**4. Embedded Kiosk**
- Single-purpose device
- Locked-down UI
- Auto-start

---

## Comparison to Similar Systems

| Feature | A-OS | Raspberry Pi OS | Android | RetroPie |
|---------|------|----------------|---------|----------|
| **Boot to UI** | ✅ | ❌ (Desktop) | ✅ | ✅ |
| **Single App** | ✅ | ❌ | ❌ | ✅ |
| **Voice Input** | ✅ | ❌ | ✅ | ❌ |
| **On-Device ASR** | ✅ | ❌ | ⚠️ | ❌ |
| **Native Apps** | ✅ | ✅ | ⚠️ | ✅ |
| **Desktop Dev** | ✅ | ❌ | ⚠️ | ❌ |
| **App SDK** | ✅ | ❌ | ✅ | ❌ |

**A-OS Unique Position:**
- Console UX + Voice + Native performance + Desktop dev

---

## Key Principles (Never Compromise)

1. **Device First**
   - If it feels like a PC, we failed

2. **Voice Equal**
   - Voice is not a feature; it's input

3. **Native Only**
   - No web stack compromise

4. **Linux Hidden**
   - User never knows Linux exists

5. **Desktop Dev**
   - Must iterate on PC first

6. **60 FPS**
   - Performance is not negotiable

7. **Offline First**
   - Cloud is optional, not required

---

## Definition of Done (Release Criteria)

### v0.1 MVP ✅
- [x] Compiles on desktop
- [x] Runs in window
- [x] Apps switch
- [x] Input works

### v1.0 Voice OS
- [ ] Runs on Pi 5
- [ ] Voice navigation works
- [ ] Boot-to-UI (<10s)
- [ ] 5+ built-in apps
- [ ] Stable (no crashes)
- [ ] Documentation complete

### v2.0 Platform
- [ ] App packaging works
- [ ] External apps installable
- [ ] Permission system
- [ ] OTA updates
- [ ] Developer tools

---

## Success Definition

**We succeed when:**

1. Someone can pick up a Pi running A-OS and:
   - Use it without reading docs
   - Navigate purely by voice
   - Never see Linux
   - Experience feels cohesive

2. A developer can:
   - Build an app in <1 hour
   - Test on desktop
   - Deploy to Pi seamlessly

3. The system:
   - Runs smoothly (60 FPS)
   - Works offline
   - Boots fast
   - Never crashes

---

## Project Values

**Simplicity over Features**
- Add features only when necessary
- Remove complexity aggressively

**Performance over Convenience**
- C++ over scripting
- Native over web
- Direct hardware access

**Transparency over Magic**
- Clear code
- Obvious behavior
- Predictable performance

**User Experience over Flexibility**
- Limited, polished > unlimited, rough
- Opinionated design
- Consistent UX

---

## Timeline Overview

```
2026 Q1  │ v0.1 MVP               ✅ Done
         │ v0.2 Text Rendering     🚧 In Progress
         │
2026 Q2  │ v0.5 Pi Deployment     ⏳ Planned
         │
2026 Q3  │ v1.0 Voice OS          ⏳ Planned
         │
2026 Q4  │ v1.5 App Ecosystem     ⏳ Future
         │
2027+    │ v2.0 Platform          ⏳ Future
         │
```

---

## How to Measure Success

### Quantitative Metrics

- **Performance:** Maintain 60 FPS
- **Boot time:** <10 seconds
- **Memory:** <128MB RSS
- **Build time:** <10 seconds (desktop)
- **Voice latency:** <500ms

### Qualitative Metrics

- **Ease of use:** Can non-technical user navigate?
- **Developer experience:** Can dev build app in 1 hour?
- **Stability:** No crashes in 24-hour run
- **Feel:** Does it feel like a device or a PC?

---

## Open Questions (To Be Decided)

### Technical

1. **App Packaging Format**
   - Custom format or adapt existing? (tar.gz + manifest?)

2. **Voice ASR Engine**
   - Sherpa-ONNX vs Vosk vs Whisper.cpp?

3. **GPU Acceleration**
   - Use Pi GPU (VideoCore) or stick to CPU?

4. **Threading Model**
   - Single-threaded or multi-threaded?

### Product

1. **App Store**
   - Central repository or sideload only?

2. **Update Mechanism**
   - OTA updates or manual?

3. **Target Hardware**
   - Pi 5 only or support Zero 2 W?

4. **Licensing**
   - Open source (MIT?) or proprietary?

---

## Conclusion

A-OS aims to be:

> **The operating system that makes Raspberry Pi feel like a purpose-built device, not a tiny computer—with voice as natural as pressing a button.**

**Core Philosophy:**
- Device, not PC
- Voice, not afterthought
- Native, not web
- Simple, not complex

**Success is:**
- User delighted
- Developer productive
- Performance excellent
- Vision realized

---

**Document Version:** 1.0
**Last Updated:** 2026-01-09
**Status:** Living document (will evolve)
