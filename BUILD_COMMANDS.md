# A-OS Complete Build & Test Commands

**Complete step-by-step guide for building and testing A-OS**

---

## Prerequisites Check

First, verify you have the required tools installed:

```bash
# Check CMake version (need 3.15+)
cmake --version

# Check C++ compiler
g++ --version    # Linux/MinGW
cl               # Windows MSVC
clang --version  # macOS

# Check if vcpkg is available (Windows recommended)
vcpkg version
```

---

## Step 1: Install SDL2 and SDL2_ttf

### Option A: Windows with vcpkg (RECOMMENDED)

```bash
# If you don't have vcpkg, install it first:
cd C:\
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat

# Add vcpkg to PATH or note the full path
# Example: C:\vcpkg\vcpkg.exe

# Install SDL2 and SDL2_ttf
vcpkg install sdl2:x64-windows
vcpkg install sdl2-ttf:x64-windows

# Integrate with system (makes libraries available globally)
vcpkg integrate install
```

### Option B: Windows with MSYS2/MinGW

```bash
# Using MSYS2 package manager
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-SDL2_ttf
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gcc
```

### Option C: Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake
sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
```

### Option D: Linux (Fedora/RHEL)

```bash
sudo dnf install cmake gcc-c++
sudo dnf install SDL2-devel SDL2_ttf-devel
```

### Option E: macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake sdl2 sdl2_ttf
```

---

## Step 2: Navigate to Project Directory

```bash
# Change to your A-OS project directory
cd e:\arabhyaWorks\test-aos

# Or on Linux/macOS:
# cd /path/to/test-aos
```

---

## Step 3: Clean Previous Build (if any)

```bash
# Remove old build directory
rm -rf build        # Linux/macOS/Git Bash
# or
rmdir /s /q build   # Windows CMD
# or
Remove-Item -Recurse -Force build  # Windows PowerShell

# Create fresh build directory
mkdir build
```

---

## Step 4: Configure CMake

### Option A: Windows with vcpkg

```bash
cd build

# Replace C:\vcpkg with your actual vcpkg path
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Alternative if vcpkg integrate install was run:
cmake ..
```

### Option B: Windows with MSYS2/MinGW

```bash
cd build

# Specify MinGW generator
cmake .. -G "MinGW Makefiles"
```

### Option C: Windows with Visual Studio

```bash
cd build

# Visual Studio 2019
cmake .. -G "Visual Studio 16 2019" -A x64

# Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -A x64
```

### Option D: Linux/macOS

```bash
cd build

# Default Unix Makefiles
cmake ..

# Or specify build type
cmake .. -DCMAKE_BUILD_TYPE=Release
```

---

## Step 5: Build the Project

### Windows (Visual Studio)

```bash
# Build Release configuration
cmake --build . --config Release

# Or build Debug configuration
cmake --build . --config Debug

# Build with parallel jobs (faster)
cmake --build . --config Release --parallel 8
```

### Windows (MinGW)

```bash
# Build with mingw32-make
mingw32-make -j8

# Or use cmake wrapper
cmake --build . --parallel 8
```

### Linux/macOS

```bash
# Build with make
make -j$(nproc)    # Linux (uses all CPU cores)
make -j$(sysctl -n hw.ncpu)  # macOS (uses all CPU cores)

# Or use cmake wrapper
cmake --build . --parallel 8
```

---

## Step 6: Verify Build Success

Look for these success indicators:

```
[100%] Building CXX object CMakeFiles/aos.dir/...
[100%] Linking CXX executable aos (or aos.exe)
[100%] Built target aos
```

Check if executable exists:

```bash
# Windows (Visual Studio)
dir Release\aos.exe
ls Release/aos.exe

# Windows (MinGW) or Linux/macOS
ls aos
./aos --version  # (won't work yet, just checks if file exists)
```

---

## Step 7: Run A-OS

### Windows (Visual Studio build)

```bash
# From build directory
.\Release\aos.exe

# Or with full path
e:\arabhyaWorks\test-aos\build\Release\aos.exe
```

### Windows (MinGW build)

```bash
# From build directory
.\aos.exe

# Or
./aos.exe
```

### Linux/macOS

```bash
# From build directory
./aos

# Or with full path
/path/to/test-aos/build/aos
```

---

## Step 8: Test the Application

Once A-OS window opens, test these features:

### Visual Tests

1. **Window Opens**
   - 1280x720 window appears
   - Title bar shows "A-OS v0.1 - Application Operating System"

2. **Home Screen**
   - Blue/gray background
   - "Settings" app tile visible
   - Instructions at bottom: "Use Arrow Keys to navigate..."

3. **Text Rendering**
   - All text should be clear and readable
   - No placeholder rectangles
   - Different font sizes visible

### Interaction Tests

```
Test 1: Navigation
- Press UP arrow    → Focus indicator (blue border) should be visible
- Press DOWN arrow  → Focus should move (if multiple apps)

Test 2: Launch App
- Press ENTER       → Settings app should open
- Observe:
  * Title bar changes to "Settings"
  * Animated red box in center
  * "Settings Demo" label visible
  * Instructions at bottom

Test 3: Return to Home
- Press ESC         → Should return to Home screen
- Observe:
  * Home screen restored
  * Settings tile still visible

Test 4: Quit Application
- Press ESC (on Home) → Window should close
- Or close window with X button
```

---

## Complete Command Sequence (Copy-Paste)

### For Windows with vcpkg:

```bash
# 1. Install dependencies (one-time setup)
vcpkg install sdl2:x64-windows sdl2-ttf:x64-windows

# 2. Navigate to project
cd e:\arabhyaWorks\test-aos

# 3. Clean and create build directory
rm -rf build
mkdir build
cd build

# 4. Configure (replace vcpkg path)
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# 5. Build
cmake --build . --config Release --parallel 8

# 6. Run
.\Release\aos.exe
```

### For Linux:

```bash
# 1. Install dependencies (one-time setup)
sudo apt-get update
sudo apt-get install -y build-essential cmake libsdl2-dev libsdl2-ttf-dev

# 2. Navigate to project
cd /path/to/test-aos

# 3. Clean and create build directory
rm -rf build
mkdir build
cd build

# 4. Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# 5. Build
make -j$(nproc)

# 6. Run
./aos
```

### For macOS:

```bash
# 1. Install dependencies (one-time setup)
brew install cmake sdl2 sdl2_ttf

# 2. Navigate to project
cd /path/to/test-aos

# 3. Clean and create build directory
rm -rf build
mkdir build
cd build

# 4. Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# 5. Build
make -j$(sysctl -n hw.ncpu)

# 6. Run
./aos
```

---

## Troubleshooting

### Issue 1: SDL2 not found

**Error:**
```
Could NOT find SDL2 (missing: SDL2_LIBRARIES SDL2_INCLUDE_DIRS)
```

**Solution:**
- Verify SDL2 is installed: `vcpkg list` (Windows) or `dpkg -l | grep sdl2` (Linux)
- For vcpkg: ensure toolchain file path is correct
- For Linux: `sudo apt-get install libsdl2-dev`
- For macOS: `brew install sdl2`

### Issue 2: SDL2_ttf not found

**Error:**
```
Could NOT find SDL2_ttf
```

**Solution:**
- Install SDL2_ttf: `vcpkg install sdl2-ttf:x64-windows`
- Or Linux: `sudo apt-get install libsdl2-ttf-dev`
- Or macOS: `brew install sdl2_ttf`

### Issue 3: Compiler not found

**Error:**
```
No CMAKE_CXX_COMPILER could be found
```

**Solution:**
- Windows: Install Visual Studio 2019+ or MinGW-w64
- Linux: `sudo apt-get install build-essential`
- macOS: `xcode-select --install`

### Issue 4: Font not loading

**Warning in output:**
```
Could not load any system font for size 24
```

**Solution:**
This is not fatal. Text will render as rectangles (fallback mode).
- Windows: Fonts should exist at `C:\Windows\Fonts\`
- Linux: Install fonts: `sudo apt-get install fonts-dejavu`
- macOS: System fonts should exist by default

### Issue 5: Build succeeds but executable crashes

**Symptom:**
Window opens briefly then closes, or crash on startup.

**Solution:**
- Check console output for error messages
- Verify SDL2 DLLs are in PATH (Windows)
- Run from build directory (assets are copied there)
- Try Debug build for more info: `cmake --build . --config Debug`

### Issue 6: "TTF_Init failed"

**Symptom:**
```
TTF_Init failed: Could not load SDL2_ttf
```

**Solution:**
- SDL2_ttf library not properly linked
- Rebuild after installing SDL2_ttf
- Check CMake output for SDL2_ttf detection messages

---

## Verify Installation Success

After running `./aos`, you should see console output like:

```
    ╔═══════════════════════════════════════╗
    ║                                       ║
    ║              A-OS v0.1                ║
    ║   Application Operating System        ║
    ║                                       ║
    ╚═══════════════════════════════════════╝

Registering applications...
Applications registered.
Renderer initialized: 1280x720
SDL_ttf initialized successfully
Font loaded: C:/Windows/Fonts/arial.ttf (size 28)
HomeApp: Started
Font loaded: C:/Windows/Fonts/arial.ttf (size 24)
Font loaded: C:/Windows/Fonts/arial.ttf (size 18)
```

If you see these messages, **text rendering is working!**

---

## Performance Expectations

- **Startup time:** <1 second
- **Frame rate:** Solid 60 FPS
- **Memory usage:** <50 MB
- **CPU usage:** <5% when idle, <20% during animations

---

## Next Steps After Successful Build

1. ✅ Take a screenshot of A-OS running
2. ✅ Test all keyboard controls
3. ✅ Verify text is rendering (not rectangles)
4. ✅ Try launching Settings app
5. ✅ Check console output for errors

Then move on to:
- Add more apps (camera, voice, etc.)
- Deploy to Raspberry Pi
- Implement GPIO button support
- Add voice input system

---

## Quick Reference

| Action | Command |
|--------|---------|
| **Clean build** | `rm -rf build && mkdir build` |
| **Configure** | `cd build && cmake ..` |
| **Build** | `cmake --build . --config Release` |
| **Run** | `./aos` or `.\Release\aos.exe` |
| **Rebuild** | `cmake --build . --config Release --clean-first` |
| **Verbose build** | `cmake --build . --config Release --verbose` |

---

## Build Output Locations

| Platform | Executable Path |
|----------|----------------|
| **Windows (MSVC)** | `build/Release/aos.exe` |
| **Windows (MinGW)** | `build/aos.exe` |
| **Linux** | `build/aos` |
| **macOS** | `build/aos` |

---

**You're ready to build!** Follow the commands for your platform above.

Need help? Check BUILD_STATUS.md for troubleshooting or review the error messages during build.
