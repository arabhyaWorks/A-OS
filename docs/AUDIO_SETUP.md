# Audio Setup for A-OS Media Player

**Last Updated:** 2026-01-09
**Version:** v0.2

## Current Status

The Media Player app is currently in **simulation mode**. It demonstrates the UI and controls but does not produce actual audio output.

### What Works

✅ Media Player UI
✅ Track selection (5 demo tracks)
✅ Play/Pause/Next/Previous controls
✅ Progress bar animation
✅ Animated vinyl effect
✅ Time display (MM:SS)

### What's Missing

❌ Actual audio playback (no sound output)
❌ Audio files (tracks are simulated)

---

## To Enable Real Audio Playback

### 1. Install SDL_mixer (Already Done)

SDL_mixer is already installed and linked:

```bash
# Check installation
pacman -Q mingw-w64-x86_64-SDL2_mixer
# Output: mingw-w64-x86_64-SDL2_mixer 2.8.1-4
```

CMakeLists.txt has been updated to link SDL2_mixer.

### 2. Add Audio Files

Create an `assets/audio/` directory and add audio files:

```bash
mkdir -p assets/audio
```

**Required audio files:**
- `assets/audio/cosmic_journey.ogg` (or .mp3, .wav)
- `assets/audio/digital_dreams.ogg`
- `assets/audio/morning_light.ogg`
- `assets/audio/night_drive.ogg`
- `assets/audio/ocean_waves.ogg`

**Supported formats:**
- OGG Vorbis (recommended - smaller size)
- MP3 (widely compatible)
- WAV (uncompressed - larger files)
- FLAC (lossless)

### 3. Update Media Player Code

The media player needs to be updated to:

1. Initialize SDL_mixer
2. Load audio files from `assets/audio/`
3. Play/pause/stop using Mix_Music functions
4. Handle audio completion callbacks

**Example SDL_mixer usage:**

```cpp
#include <SDL2/SDL_mixer.h>

// Initialize (in onStart)
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer init failed: " << Mix_GetError() << std::endl;
}

// Load music
Mix_Music* music = Mix_LoadMUS("assets/audio/cosmic_journey.ogg");
if (!music) {
    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
}

// Play
Mix_PlayMusic(music, 0); // 0 = play once, -1 = loop

// Pause/Resume
Mix_PauseMusic();
Mix_ResumeMusic();

// Check if playing
bool isPlaying = Mix_PlayingMusic();

// Cleanup
Mix_FreeMusic(music);
Mix_CloseAudio();
```

---

## Quick Test with Free Audio

### Option 1: Download Free Music

```bash
cd assets
mkdir audio
cd audio

# Download from FreeMusicArchive or similar
# Example: wget https://example.com/free-music.ogg
```

### Option 2: Generate Test Tones

You can use ffmpeg to generate simple test tones:

```bash
# Install ffmpeg (if not already installed)
pacman -S mingw-w64-x86_64-ffmpeg

# Generate 5-second test tones
ffmpeg -f lavfi -i "sine=frequency=440:duration=5" cosmic_journey.wav
ffmpeg -f lavfi -i "sine=frequency=523:duration=5" digital_dreams.wav
ffmpeg -f lavfi -i "sine=frequency=587:duration=5" morning_light.wav
ffmpeg -f lavfi -i "sine=frequency=659:duration=5" night_drive.wav
ffmpeg -f lavfi -i "sine=frequency=698:duration=5" ocean_waves.wav
```

### Option 3: Use Existing Music

Copy any music files you own:

```bash
cp /path/to/your/music/*.ogg assets/audio/
# Rename to match the expected filenames
```

---

## Why Doesn't It Play Sound Now?

The Media Player currently:

1. **Simulates track playback** - increments position based on deltaTime
2. **Shows visual feedback** - progress bar, animated vinyl
3. **Does NOT call SDL_mixer** - no actual audio output

This is intentional for the demo phase because:
- No audio files are included in the repository
- Different users may want different music
- Keeps the repository size small
- Allows testing UI without audio dependencies

---

## Roadmap for Audio Integration

### Phase 1 (Current): Simulation
✅ UI fully functional
✅ Track metadata displayed
✅ Controls work correctly
❌ No audio output

### Phase 2: Basic Playback
- [ ] Load OGG/MP3 files
- [ ] Play/pause actual audio
- [ ] Volume control
- [ ] Seek functionality

### Phase 3: Advanced Features
- [ ] Playlist support
- [ ] File browser
- [ ] Album art from metadata
- [ ] Equalizer
- [ ] Shuffle/repeat modes

---

## Testing the Current Implementation

Even without audio, you can test:

1. Launch A-OS
2. Navigate to Media Player
3. Test controls:
   - **Enter:** Play/Pause (watch for ▶ PLAYING indicator)
   - **Left/Right:** Change tracks (see track name change)
   - **Track progression:** Progress bar animates when playing
   - **Visual feedback:** Vinyl animation when playing

Console output will show:
```
MediaApp: Playing - Cosmic Journey
MediaApp: Next track - Digital Dreams
MediaApp: Paused
```

---

## Implementation Example

If you want to add real audio now, here's a minimal example:

**1. Update media_app.h:**

```cpp
#include <SDL2/SDL_mixer.h>

private:
    std::vector<Mix_Music*> musicTracks;
    bool audioInitialized;
```

**2. Update media_app.cpp onStart():**

```cpp
void MediaApp::onStart() {
    audioInitialized = false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer error: " << Mix_GetError() << std::endl;
        return;
    }

    audioInitialized = true;

    // Load all tracks
    for (int i = 0; i < trackCount; i++) {
        std::string path = "assets/audio/" + tracks[i].filename;
        Mix_Music* music = Mix_LoadMUS(path.c_str());

        if (music) {
            musicTracks.push_back(music);
        } else {
            std::cerr << "Failed to load: " << path << std::endl;
            musicTracks.push_back(nullptr);
        }
    }
}
```

**3. Update togglePlayPause():**

```cpp
void MediaApp::togglePlayPause() {
    if (!audioInitialized) return;

    if (state == PLAYING) {
        Mix_PauseMusic();
        state = PAUSED;
    } else {
        if (musicTracks[currentTrack]) {
            Mix_ResumeMusic();
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(musicTracks[currentTrack], 0);
            }
        }
        state = PLAYING;
    }
}
```

---

## Troubleshooting

### SDL_mixer not linking

```bash
# Reinstall SDL_mixer
pacman -S mingw-w64-x86_64-SDL2_mixer

# Reconfigure CMake
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j8
```

### Audio files not loading

- Check file paths are correct: `assets/audio/filename.ogg`
- Verify assets directory is copied to build folder
- Check file format is supported (use `file` command)
- Try converting to OGG: `ffmpeg -i input.mp3 output.ogg`

### No sound output

- Check Windows volume mixer
- Verify audio device is working
- Test with other apps
- Check SDL_mixer initialization errors in console

---

## Summary

**Current State:** Media Player is fully functional UI-wise but produces no audio.

**To get audio working:**
1. Add audio files to `assets/audio/`
2. Update media_app.cpp to use SDL_mixer
3. Rebuild and test

**Why wait?** You can develop and test all other A-OS features without audio. When you're ready for audio, the integration is straightforward with SDL_mixer already linked.

For now, enjoy the visual media player experience! 🎵

---

## Related Documentation

- [APPS.md](APPS.md) - Complete application documentation
- [PROJECT_SPEC.md](PROJECT_SPEC.md) - Technical specifications
- [BUILD_STATUS.md](BUILD_STATUS.md) - Current build status

**SDL_mixer Documentation:** https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer.html
