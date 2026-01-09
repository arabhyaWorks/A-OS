#pragma once

#include "os/app.h"
#include "ui/renderer.h"
#include <string>

namespace AOS {

/**
 * MediaApp - Media player interface
 *
 * Demonstrates:
 * - Playback controls (play/pause)
 * - Progress bar
 * - Track information
 * - Button navigation
 *
 * In production:
 * - Audio file playback
 * - Playlist management
 * - Album art display
 */
class MediaApp : public App {
public:
    MediaApp();
    ~MediaApp() override = default;

    void onStart() override;
    void onStop() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "Media Player"; }

private:
    enum PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

    PlayState state;
    float trackPosition;
    float trackDuration;
    int currentTrack;

    struct Track {
        std::string title;
        std::string artist;
        float duration;
    };

    static const Track tracks[];
    static const int trackCount;

    void togglePlayPause();
    void nextTrack();
    void prevTrack();
    void loadTrack(int index);
};

} // namespace AOS
