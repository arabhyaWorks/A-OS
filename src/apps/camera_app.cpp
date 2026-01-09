#include "camera_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <cmath>

extern AOS::AppManager* g_appManager;

namespace AOS {

CameraApp::CameraApp()
    : currentMode(PREVIEW)
    , previewTime(0.0f)
    , capturing(false)
    , captureFlashTime(0.0f)
    , photoCount(0)
    , galleryIndex(0)
{
}

void CameraApp::onStart() {
    std::cout << "CameraApp: Started" << std::endl;
    previewTime = 0.0f;
    capturing = false;
    captureFlashTime = 0.0f;
    currentMode = PREVIEW;
}

void CameraApp::onStop() {
    std::cout << "CameraApp: Stopped" << std::endl;

    // Free all captured photos
    for (auto& photo : photos) {
        if (photo.surface) {
            SDL_FreeSurface(photo.surface);
        }
    }
    photos.clear();
}

void CameraApp::update(float deltaTime) {
    if (currentMode == PREVIEW) {
        previewTime += deltaTime;

        if (capturing) {
            captureFlashTime += deltaTime;
            if (captureFlashTime > 0.3f) {
                capturing = false;
                captureFlashTime = 0.0f;
            }
        }
    }
}

void CameraApp::render(Renderer& renderer) {
    // Draw header background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), 80), Color(40, 60, 40), true);

    if (currentMode == PREVIEW) {
        renderer.drawText("Camera - Preview", 20, 25, Color::White(), 28);

        int centerX = renderer.getWidth() / 2;
        int centerY = renderer.getHeight() / 2;

        // Simulate camera preview with animated pattern
        renderer.drawRect(
            Rect(centerX - 300, centerY - 200, 600, 400),
            Color(20, 20, 30),
            true
        );

        // Animated "video feed" effect
        for (int i = 0; i < 8; i++) {
            float phase = previewTime * 2.0f + i * 0.5f;
            float wave = std::sin(phase) * 0.5f + 0.5f;
            int y = centerY - 200 + i * 50;
            int brightness = 40 + (int)(wave * 60);

            renderer.drawRect(
                Rect(centerX - 300, y, 600, 45),
                Color(brightness, brightness, brightness + 20),
                true
            );
        }

        // Camera frame border
        renderer.drawRect(
            Rect(centerX - 300, centerY - 200, 600, 400),
            Color(100, 200, 100),
            false
        );

        // Crosshair
        renderer.drawRect(
            Rect(centerX - 20, centerY - 1, 40, 2),
            Color(0, 255, 0),
            true
        );
        renderer.drawRect(
            Rect(centerX - 1, centerY - 20, 2, 40),
            Color(0, 255, 0),
            true
        );

        // Capture flash effect
        if (capturing) {
            int alpha = (int)((1.0f - captureFlashTime / 0.3f) * 200);
            renderer.drawRect(
                Rect(0, 0, renderer.getWidth(), renderer.getHeight()),
                Color(255, 255, 255, alpha),
                true
            );
        }

        // Info overlay
        renderer.drawText("LIVE", centerX - 280, centerY - 180, Color(255, 0, 0), 20);

        char photoText[32];
        snprintf(photoText, sizeof(photoText), "Photos: %d", photoCount);
        renderer.drawText(photoText, centerX + 200, centerY - 180, Color(200, 200, 200), 18);

        // Instructions
        renderer.drawText("ENTER: Capture | UP: Gallery", centerX - 140, renderer.getHeight() - 80, Color(150, 200, 150), 20);
        renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);

    } else { // GALLERY mode
        char headerText[64];
        snprintf(headerText, sizeof(headerText), "Camera - Gallery (%d photos)", (int)photos.size());
        renderer.drawText(headerText, 20, 25, Color::White(), 28);

        int centerX = renderer.getWidth() / 2;
        int centerY = renderer.getHeight() / 2;

        if (photos.empty()) {
            renderer.drawText("No photos captured yet", centerX - 120, centerY - 20, Color(150, 150, 150), 24);
            renderer.drawText("Press UP to return to camera", centerX - 140, centerY + 20, Color(120, 120, 120), 18);
        } else {
            // Display current photo
            Photo& currentPhoto = photos[galleryIndex];

            // Draw photo (convert surface to texture and display)
            SDL_Renderer* sdlRenderer = renderer.getSDLRenderer();
            if (currentPhoto.surface && sdlRenderer) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, currentPhoto.surface);
                if (texture) {
                    // Scale to fit window while maintaining aspect ratio
                    int photoW = currentPhoto.surface->w;
                    int photoH = currentPhoto.surface->h;
                    int maxW = renderer.getWidth() - 100;
                    int maxH = renderer.getHeight() - 250;

                    float scale = std::min((float)maxW / photoW, (float)maxH / photoH);
                    int displayW = (int)(photoW * scale);
                    int displayH = (int)(photoH * scale);

                    SDL_Rect destRect = {
                        centerX - displayW / 2,
                        centerY - displayH / 2,
                        displayW,
                        displayH
                    };

                    SDL_RenderCopy(sdlRenderer, texture, nullptr, &destRect);

                    // Draw border
                    renderer.drawRect(
                        Rect(destRect.x - 2, destRect.y - 2, destRect.w + 4, destRect.h + 4),
                        Color(100, 200, 100),
                        false
                    );

                    SDL_DestroyTexture(texture);
                }
            }

            // Photo info
            char photoInfo[64];
            snprintf(photoInfo, sizeof(photoInfo), "Photo #%d (%d / %d)",
                     currentPhoto.number, galleryIndex + 1, (int)photos.size());
            renderer.drawText(photoInfo, centerX - 100, 110, Color(200, 200, 200), 20);

            // Navigation instructions
            renderer.drawText("LEFT/RIGHT: Navigate | UP: Camera", centerX - 180, renderer.getHeight() - 80, Color(150, 200, 150), 20);
            renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
        }
    }
}

void CameraApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        std::cout << "CameraApp: Returning to home" << std::endl;
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    } else if (currentMode == PREVIEW) {
        if (event.type == EventType::KEY_SELECT) {
            // Capture photo
            capturing = true;
            captureFlashTime = 0.0f;
            photoCount++;
            std::cout << "CameraApp: Photo captured (#" << photoCount << ")" << std::endl;

            // Create a simulated photo with a unique color pattern
            SDL_Surface* surface = SDL_CreateRGBSurface(0, 600, 400, 32, 0, 0, 0, 0);
            if (surface) {
                // Fill with a gradient based on photo number
                SDL_FillRect(surface, nullptr,
                    SDL_MapRGB(surface->format,
                               40 + (photoCount * 30) % 180,
                               60 + (photoCount * 45) % 180,
                               40 + (photoCount * 60) % 180));

                photos.push_back({surface, photoCount});
            }
        } else if (event.type == EventType::KEY_UP) {
            // Switch to gallery
            switchToGallery();
        }
    } else { // GALLERY mode
        if (event.type == EventType::KEY_LEFT) {
            if (!photos.empty()) {
                galleryIndex = (galleryIndex - 1 + photos.size()) % photos.size();
                std::cout << "CameraApp: Viewing photo " << (galleryIndex + 1) << "/" << photos.size() << std::endl;
            }
        } else if (event.type == EventType::KEY_RIGHT) {
            if (!photos.empty()) {
                galleryIndex = (galleryIndex + 1) % photos.size();
                std::cout << "CameraApp: Viewing photo " << (galleryIndex + 1) << "/" << photos.size() << std::endl;
            }
        } else if (event.type == EventType::KEY_UP) {
            // Switch to preview
            switchToPreview();
        }
    }
}

void CameraApp::switchToGallery() {
    currentMode = GALLERY;
    galleryIndex = photos.empty() ? 0 : photos.size() - 1; // Show latest photo
    std::cout << "CameraApp: Switched to gallery mode (" << photos.size() << " photos)" << std::endl;
}

void CameraApp::switchToPreview() {
    currentMode = PREVIEW;
    std::cout << "CameraApp: Switched to preview mode" << std::endl;
}

void CameraApp::capturePhoto(SDL_Renderer* sdlRenderer) {
    // This would capture actual screenshot in production
    // For now, we create a simulated photo in onEvent
}

} // namespace AOS
