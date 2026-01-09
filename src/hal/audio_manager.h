#pragma once

namespace AOS {

/**
 * AudioManager - Placeholder for audio system
 *
 * Future responsibilities:
 * - ALSA audio capture (microphone)
 * - ALSA audio playback (speaker)
 * - UI sound effects
 * - Media playback
 * - TTS output (v1)
 * - ASR input processing (v1)
 *
 * For MVP, this is a stub.
 */
class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Placeholder methods
    void initialize();
    void shutdown();
};

} // namespace AOS
