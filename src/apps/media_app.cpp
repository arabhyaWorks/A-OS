#include "media_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

extern AOS::AppManager* g_appManager;

namespace AOS {

const MediaApp::Track MediaApp::tracks[] = {
    {"Cosmic Journey", "Space Orchestra", 245.0f},
    {"Digital Dreams", "Synth Wave", 198.0f},
    {"Morning Light", "Acoustic Trio", 212.0f},
    {"Night Drive", "Electric Beats", 267.0f},
    {"Ocean Waves", "Nature Sounds", 180.0f}
};

const int MediaApp::trackCount = sizeof(MediaApp::tracks) / sizeof(MediaApp::tracks[0]);

MediaApp::MediaApp()
    : state(STOPPED)
    , trackPosition(0.0f)
    , trackDuration(0.0f)
    , currentTrack(0)
{
}

void MediaApp::onStart() {
    std::cout << "MediaApp: Started" << std::endl;
    loadTrack(0);
}

void MediaApp::onStop() {
    std::cout << "MediaApp: Stopped" << std::endl;
    state = STOPPED;
}

void MediaApp::update(float deltaTime) {
    if (state == PLAYING) {
        trackPosition += deltaTime;

        // Loop track when finished
        if (trackPosition >= trackDuration) {
            trackPosition = 0.0f;
            nextTrack();
        }
    }
}

void MediaApp::render(Renderer& renderer) {
    // Draw header background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), 80), Color(60, 40, 80), true);
    renderer.drawText("Media Player", 20, 25, Color::White(), 28);

    int centerX = renderer.getWidth() / 2;
    int centerY = renderer.getHeight() / 2;

    // Album art placeholder (animated when playing)
    float rotation = state == PLAYING ? trackPosition * 20.0f : 0.0f;
    int artSize = 200;
    int artX = centerX - artSize / 2;
    int artY = 150;

    renderer.drawRect(
        Rect(artX, artY, artSize, artSize),
        Color(80, 60, 100),
        true
    );

    // Animated vinyl effect when playing
    if (state == PLAYING) {
        for (int i = 0; i < 5; i++) {
            int radius = 20 + i * 30;
            float phase = rotation + i * 30.0f;
            int brightness = 100 + (int)(std::sin(phase * 0.1f) * 50);
            renderer.drawRect(
                Rect(centerX - radius, artY + artSize/2 - 2, radius * 2, 4),
                Color(brightness, brightness - 20, brightness + 20),
                false
            );
        }
    }

    // Border
    renderer.drawRect(Rect(artX, artY, artSize, artSize), Color(150, 120, 180), false);

    // Track info
    renderer.drawText(
        tracks[currentTrack].title,
        centerX - 150,
        artY + artSize + 40,
        Color::White(),
        24
    );

    renderer.drawText(
        tracks[currentTrack].artist,
        centerX - 100,
        artY + artSize + 70,
        Color(200, 200, 200),
        18
    );

    // Progress bar
    int barWidth = 600;
    int barHeight = 8;
    int barX = centerX - barWidth / 2;
    int barY = artY + artSize + 110;

    // Background
    renderer.drawRect(
        Rect(barX, barY, barWidth, barHeight),
        Color(60, 60, 60),
        true
    );

    // Progress
    float progress = trackDuration > 0 ? trackPosition / trackDuration : 0.0f;
    int progressWidth = (int)(barWidth * progress);
    renderer.drawRect(
        Rect(barX, barY, progressWidth, barHeight),
        Color(150, 100, 200),
        true
    );

    // Time labels
    auto formatTime = [](float seconds) -> std::string {
        int mins = (int)(seconds / 60);
        int secs = (int)seconds % 60;
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << mins << ":"
            << std::setfill('0') << std::setw(2) << secs;
        return oss.str();
    };

    renderer.drawText(
        formatTime(trackPosition),
        barX,
        barY + 20,
        Color(150, 150, 150),
        16
    );

    renderer.drawText(
        formatTime(trackDuration),
        barX + barWidth - 50,
        barY + 20,
        Color(150, 150, 150),
        16
    );

    // Controls status
    const char* stateText = "STOPPED";
    Color stateColor = Color(150, 150, 150);

    if (state == PLAYING) {
        stateText = "▶ PLAYING";
        stateColor = Color(100, 255, 100);
    } else if (state == PAUSED) {
        stateText = "⏸ PAUSED";
        stateColor = Color(255, 255, 100);
    }

    renderer.drawText(
        stateText,
        centerX - 50,
        barY + 60,
        stateColor,
        20
    );

    // Track counter
    char trackText[32];
    snprintf(trackText, sizeof(trackText), "Track %d/%d", currentTrack + 1, trackCount);
    renderer.drawText(trackText, centerX - 40, barY + 90, Color(180, 180, 180), 18);

    // Instructions
    renderer.drawText("ENTER: Play/Pause  |  LEFT/RIGHT: Change Track",
                      centerX - 240, renderer.getHeight() - 80, Color(150, 150, 200), 18);
    renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void MediaApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        std::cout << "MediaApp: Returning to home" << std::endl;
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    } else if (event.type == EventType::KEY_SELECT) {
        togglePlayPause();
    } else if (event.type == EventType::KEY_RIGHT) {
        nextTrack();
    } else if (event.type == EventType::KEY_LEFT) {
        prevTrack();
    }
}

void MediaApp::togglePlayPause() {
    if (state == PLAYING) {
        state = PAUSED;
        std::cout << "MediaApp: Paused" << std::endl;
    } else {
        state = PLAYING;
        std::cout << "MediaApp: Playing - " << tracks[currentTrack].title << std::endl;
    }
}

void MediaApp::nextTrack() {
    currentTrack = (currentTrack + 1) % trackCount;
    loadTrack(currentTrack);
    std::cout << "MediaApp: Next track - " << tracks[currentTrack].title << std::endl;
}

void MediaApp::prevTrack() {
    currentTrack = (currentTrack - 1 + trackCount) % trackCount;
    loadTrack(currentTrack);
    std::cout << "MediaApp: Previous track - " << tracks[currentTrack].title << std::endl;
}

void MediaApp::loadTrack(int index) {
    currentTrack = index;
    trackDuration = tracks[index].duration;
    trackPosition = 0.0f;

    // Auto-play when changing tracks
    if (state != STOPPED) {
        state = PLAYING;
    }
}

} // namespace AOS
