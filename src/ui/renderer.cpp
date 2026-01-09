#include "renderer.h"
#include <iostream>

namespace AOS {

Renderer::Renderer(SDL_Window* win, SDL_Renderer* sdlRend)
    : window(win)
    , sdlRenderer(sdlRend)
    , defaultFontPath("")
{
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    std::cout << "Renderer initialized: " << screenWidth << "x" << screenHeight << std::endl;

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    } else {
        std::cout << "SDL_ttf initialized successfully" << std::endl;
    }
}

Renderer::~Renderer() {
    // Free all cached fonts
    for (auto& pair : fontCache) {
        if (pair.second) {
            TTF_CloseFont(pair.second);
        }
    }
    fontCache.clear();

    // Quit SDL_ttf
    TTF_Quit();
}

void Renderer::clear(const Color& color) {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(sdlRenderer);
}

void Renderer::present() {
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::drawRect(const Rect& rect, const Color& color, bool filled) {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);

    SDL_Rect sdlRect = { rect.x, rect.y, rect.w, rect.h };

    if (filled) {
        SDL_RenderFillRect(sdlRenderer, &sdlRect);
    } else {
        SDL_RenderDrawRect(sdlRenderer, &sdlRect);
    }
}

void Renderer::drawText(const std::string& text, int x, int y, const Color& color, int fontSize) {
    if (text.empty()) {
        return;
    }

    // Get or load font
    TTF_Font* font = getFont(fontSize);
    if (!font) {
        // Fallback: draw a rectangle as placeholder
        drawRect(Rect(x, y, (int)text.length() * fontSize / 2, fontSize), color, true);
        return;
    }

    // Render text to surface
    SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    // Draw texture
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(sdlRenderer, texture, nullptr, &destRect);

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

bool Renderer::loadFont(const std::string& path, int size) {
    // Check if already loaded
    if (fontCache.find(size) != fontCache.end()) {
        return true;
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cerr << "Failed to load font: " << path << " - " << TTF_GetError() << std::endl;
        return false;
    }

    fontCache[size] = font;
    if (defaultFontPath.empty()) {
        defaultFontPath = path;
    }

    std::cout << "Font loaded: " << path << " (size " << size << ")" << std::endl;
    return true;
}

TTF_Font* Renderer::getFont(int size) {
    // Check cache first
    auto it = fontCache.find(size);
    if (it != fontCache.end()) {
        return it->second;
    }

    // Try to load from default font path
    if (!defaultFontPath.empty()) {
        if (loadFont(defaultFontPath, size)) {
            return fontCache[size];
        }
    }

    // Try common system font paths
    const char* commonFonts[] = {
        // Windows
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        // macOS
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf"
    };

    for (const char* fontPath : commonFonts) {
        if (loadFont(fontPath, size)) {
            return fontCache[size];
        }
    }

    std::cerr << "Could not load any system font for size " << size << std::endl;
    return nullptr;
}

} // namespace AOS
