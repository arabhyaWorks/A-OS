#pragma once

#include "os/app.h"
#include "ui/renderer.h"
#include <vector>
#include <SDL2/SDL.h>

namespace AOS {

/**
 * CameraApp - Camera preview and capture
 *
 * Demonstrates:
 * - Real-time preview simulation
 * - Button-based capture
 * - Visual feedback
 *
 * In production:
 * - V4L2 camera integration
 * - Image capture to storage
 * - Gallery integration
 */
class CameraApp : public App {
public:
    CameraApp();
    ~CameraApp() override = default;

    void onStart() override;
    void onStop() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "Camera"; }

private:
    enum Mode {
        PREVIEW,
        GALLERY
    };

    struct Photo {
        SDL_Surface* surface;
        int number;
    };

    Mode currentMode;
    float previewTime;
    bool capturing;
    float captureFlashTime;
    int photoCount;
    std::vector<Photo> photos;
    int galleryIndex;

    void capturePhoto(SDL_Renderer* sdlRenderer);
    void switchToGallery();
    void switchToPreview();
};

} // namespace AOS
