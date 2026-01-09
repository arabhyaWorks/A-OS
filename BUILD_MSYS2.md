# A-OS Build Guide for MSYS2/MinGW

**Complete step-by-step commands for building A-OS with MSYS2 (no vcpkg needed!)**

---

## ✅ You Have MSYS2 - Perfect!

MSYS2 is simpler than vcpkg. No toolchain files needed!

---

## Step 1: Open MSYS2 Terminal

**Important:** Use the correct MSYS2 terminal:

```
Start Menu → MSYS2 → MSYS2 MINGW64
```

**NOT** "MSYS2 MSYS" - use **"MSYS2 MINGW64"**

---

## Step 2: Install Dependencies (One-Time Setup)

Copy and paste these commands into MSYS2 MINGW64 terminal:

```bash
# Update package database
pacman -Syu

# If it closes, reopen MSYS2 MINGW64 and run:
pacman -Su

# Install build tools
pacman -S --needed mingw-w64-x86_64-toolchain
pacman -S --needed mingw-w64-x86_64-cmake
pacman -S --needed make

# Install SDL2 and SDL2_ttf
pacman -S --needed mingw-w64-x86_64-SDL2
pacman -S --needed mingw-w64-x86_64-SDL2_ttf

# Verify installation
which gcc
which cmake
```

**Expected output:**
```
/mingw64/bin/gcc
/mingw64/bin/cmake
```

---

## Step 3: Navigate to Project

```bash
# Navigate to your project (use forward slashes)
cd /e/arabhyaWorks/test-aos

# Verify you're in the right place
pwd
ls CMakeLists.txt
```

**Expected:** Should show CMakeLists.txt exists

---

## Step 4: Clean Build Directory

```bash
# Remove old build if it exists
rm -rf build

# Create fresh build directory
mkdir build
cd build
```

---

## Step 5: Configure CMake

```bash
# Configure with MinGW Makefiles
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Watch for these success messages:
# -- Found SDL2: ...
# -- Found SDL2_ttf: ... (or will try fallback)
# -- Configuring done
# -- Generating done
```

**Expected output:**
```
-- The CXX compiler identification is GNU ...
-- Check for working CXX compiler: .../mingw64/bin/g++.exe
-- Check for working CXX compiler: .../mingw64/bin/g++.exe - works
-- Detecting CXX compiler ABI info - done
-- Found SDL2: ...
-- Configuring done
-- Generating done
-- Build files have been written to: /e/arabhyaWorks/test-aos/build
```

---

## Step 6: Build the Project

```bash
# Build with all CPU cores (fast)
mingw32-make -j8

# Or just:
make -j8
```

**Expected output:**
```
Scanning dependencies of target aos
[  7%] Building CXX object CMakeFiles/aos.dir/src/main.cpp.obj
[ 14%] Building CXX object CMakeFiles/aos.dir/src/os/event_bus.cpp.obj
[ 21%] Building CXX object CMakeFiles/aos.dir/src/os/app_manager.cpp.obj
[ 28%] Building CXX object CMakeFiles/aos.dir/src/os/os_core.cpp.obj
[ 35%] Building CXX object CMakeFiles/aos.dir/src/hal/input_manager.cpp.obj
[ 42%] Building CXX object CMakeFiles/aos.dir/src/hal/audio_manager.cpp.obj
[ 50%] Building CXX object CMakeFiles/aos.dir/src/ui/renderer.cpp.obj
[ 57%] Building CXX object CMakeFiles/aos.dir/src/apps/home_app.cpp.obj
[ 64%] Building CXX object CMakeFiles/aos.dir/src/apps/settings_app.cpp.obj
[100%] Linking CXX executable aos.exe
[100%] Built target aos
```

**Build time:** ~30 seconds on modern hardware

---

## Step 7: Verify Build Success

```bash
# Check if executable exists
ls -lh aos.exe

# Should show something like:
# -rwxr-xr-x 1 user 197K Jan  9 18:00 aos.exe
```

---

## Step 8: Run A-OS!

```bash
# Run from build directory
./aos.exe

# Or with full path
/e/arabhyaWorks/test-aos/build/aos.exe
```

**Expected console output:**
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

**A window should appear!** 🎉

---

## Step 9: Test the Application

### Visual Verification

**Check these elements:**
- ✅ Window title: "aos"
- ✅ Title bar text: "A-OS v0.1 - Application Operating System"
- ✅ App tile with "Settings" label (blue when focused)
- ✅ Instructions at bottom
- ✅ All text is readable (not rectangles)

### Keyboard Testing

```
Test 1: Arrow Keys
- Press UP    → (Focus indicator should be visible on Settings tile)
- Press DOWN  → (Focus stays on Settings if it's the only app)

Test 2: Launch App
- Press ENTER → Settings app opens
  ✅ Title changes to "Settings"
  ✅ Animated red box appears in center
  ✅ "Settings Demo" text visible
  ✅ "This is a placeholder app" visible
  ✅ "Press ESC to return to Home" at bottom

Test 3: Navigate Back
- Press ESC → Returns to Home screen
  ✅ Home screen restored
  ✅ Settings tile still visible and focused

Test 4: Exit
- Press ESC (while on Home) → Application closes
  ✅ Window closes cleanly
  ✅ No error messages in terminal
```

---

## Complete Command Summary (Copy-Paste All)

```bash
# === ONE-TIME SETUP ===
# (Only run these if you haven't installed SDL2 yet)
pacman -S --needed mingw-w64-x86_64-toolchain
pacman -S --needed mingw-w64-x86_64-cmake
pacman -S --needed mingw-w64-x86_64-SDL2
pacman -S --needed mingw-w64-x86_64-SDL2_ttf

# === BUILD PROCESS ===
# Navigate to project
cd /e/arabhyaWorks/test-aos

# Clean build
rm -rf build
mkdir build
cd build

# Configure
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
make -j8

# Run
./aos.exe
```

---

## Troubleshooting

### Issue 1: "cmake: command not found"

**Problem:** CMake not in PATH

**Solution:**
```bash
# Make sure you're in MSYS2 MINGW64 (not MSYS)
# Reinstall cmake:
pacman -S mingw-w64-x86_64-cmake

# Verify:
which cmake
```

### Issue 2: "Could NOT find SDL2"

**Problem:** SDL2 not installed properly

**Solution:**
```bash
# Reinstall SDL2 packages:
pacman -S --needed mingw-w64-x86_64-SDL2
pacman -S --needed mingw-w64-x86_64-SDL2_ttf

# Verify installation:
pacman -Qs SDL2
# Should show:
# mingw64/mingw-w64-x86_64-SDL2
# mingw64/mingw-w64-x86_64-SDL2_ttf
```

### Issue 3: "undefined reference to `SDL_Init`"

**Problem:** Linker can't find SDL2 libraries

**Solution:**
```bash
# Clean rebuild:
cd /e/arabhyaWorks/test-aos
rm -rf build
mkdir build
cd build

# Make sure you use MinGW Makefiles:
cmake .. -G "MinGW Makefiles"
make -j8
```

### Issue 4: Build succeeds but "./aos.exe" fails

**Problem:** Missing DLLs

**Solution:**
```bash
# Make sure you're running from MSYS2 MINGW64 terminal
# It automatically adds MinGW bin to PATH

# Verify DLLs are available:
which SDL2.dll
# Should show: /mingw64/bin/SDL2.dll

# Run from build directory:
cd /e/arabhyaWorks/test-aos/build
./aos.exe
```

### Issue 5: Fonts don't load

**Warning in console:**
```
Could not load any system font for size 24
```

**Solution:**
This is **not fatal** - text will render as rectangles (fallback).
Windows fonts at `C:/Windows/Fonts/` should exist by default.

If you really want to see text, make sure Windows fonts are accessible:
```bash
ls /c/Windows/Fonts/arial.ttf
# Should exist
```

### Issue 6: Window opens then immediately closes

**Problem:** Runtime error

**Solution:**
```bash
# Run and watch console output:
./aos.exe

# Look for error messages like:
# "Failed to initialize SDL"
# "Could not create window"

# Make sure SDL2 is properly installed:
pacman -S --needed mingw-w64-x86_64-SDL2
```

---

## Performance Expectations

**On modern Windows PC:**
- Build time: 20-40 seconds (first build)
- Rebuild time: 2-5 seconds (after changes)
- Startup time: <1 second
- Frame rate: 60 FPS (smooth)
- Memory usage: ~30-50 MB
- CPU usage: <5% idle, <20% with animations

---

## Rebuild After Code Changes

```bash
# Navigate to build directory
cd /e/arabhyaWorks/test-aos/build

# Rebuild (fast, only changed files)
make -j8

# Run
./aos.exe
```

**No need to run cmake again unless:**
- You added new source files
- You changed CMakeLists.txt
- You added new dependencies

---

## Clean Build (Start Fresh)

```bash
cd /e/arabhyaWorks/test-aos

# Delete everything
rm -rf build

# Rebuild from scratch
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make -j8
./aos.exe
```

---

## Running Outside MSYS2

To run A-OS from Windows Explorer or CMD:

**Option 1: Add MinGW to Windows PATH**
```
Control Panel → System → Advanced → Environment Variables
Add: C:\msys64\mingw64\bin
```

**Option 2: Copy DLLs to build directory**
```bash
cd /e/arabhyaWorks/test-aos/build

# Copy required DLLs
cp /mingw64/bin/SDL2.dll .
cp /mingw64/bin/SDL2_ttf.dll .
cp /mingw64/bin/libfreetype-6.dll .
cp /mingw64/bin/libharfbuzz-0.dll .
cp /mingw64/bin/libgcc_s_seh-1.dll .
cp /mingw64/bin/libstdc++-6.dll .
cp /mingw64/bin/libwinpthread-1.dll .

# Now you can double-click aos.exe in Windows Explorer!
```

---

## Verify Everything Works

Run this checklist:

```bash
# 1. Build succeeds
cd /e/arabhyaWorks/test-aos/build
make -j8
# ✅ Should end with: [100%] Built target aos

# 2. Executable exists
ls aos.exe
# ✅ Should show: aos.exe

# 3. Can run
./aos.exe
# ✅ Window opens with title and text

# 4. Console output clean
# ✅ No error messages
# ✅ Shows "SDL_ttf initialized successfully"
# ✅ Shows "Font loaded: C:/Windows/Fonts/arial.ttf"

# 5. UI functional
# ✅ Press arrow keys → Focus moves
# ✅ Press ENTER → Settings opens
# ✅ Press ESC → Returns to Home
# ✅ Press ESC → Application exits cleanly
```

---

## Success Indicators

**Console output should show:**
```
✅ SDL_ttf initialized successfully
✅ Font loaded: C:/Windows/Fonts/arial.ttf (size 28)
✅ Font loaded: C:/Windows/Fonts/arial.ttf (size 24)
✅ Font loaded: C:/Windows/Fonts/arial.ttf (size 18)
✅ HomeApp: Started
```

**Window should display:**
```
✅ Clear, readable text (not rectangles)
✅ Title: "A-OS v0.1 - Application Operating System"
✅ Settings tile with blue border when focused
✅ Instructions at bottom
```

---

## Next Steps After Success

Once you confirm it works:

1. ✅ **Take a screenshot** of A-OS running
2. ✅ **Test all keyboard controls** (arrows, enter, esc)
3. ✅ **Verify text rendering** (should see actual fonts)
4. ✅ **Check Settings app** animation works

Then move on to:
- Add more apps (Camera, Voice, etc.)
- Deploy to Raspberry Pi
- Implement GPIO buttons
- Add voice input system

---

## Quick Reference Card

| Command | Purpose |
|---------|---------|
| `cd /e/arabhyaWorks/test-aos` | Go to project |
| `rm -rf build && mkdir build` | Clean build |
| `cd build` | Enter build dir |
| `cmake .. -G "MinGW Makefiles"` | Configure |
| `make -j8` | Build |
| `./aos.exe` | Run |
| `make clean` | Clean objects |
| `pacman -S pkg` | Install package |
| `pacman -Qs SDL` | Search installed |

---

## Summary

**With MSYS2, you don't need vcpkg!**

✅ Simpler setup
✅ Faster builds
✅ Native MinGW environment
✅ Easy package management with pacman

**Just run:**
```bash
pacman -S mingw-w64-x86_64-{toolchain,cmake,SDL2,SDL2_ttf}
cd /e/arabhyaWorks/test-aos
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make -j8
./aos.exe
```

**That's it!** 🚀

---

**Ready to build? Copy the commands above and let's go!**
