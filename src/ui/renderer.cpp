#include "renderer.h"
#include <iostream>
#include <cmath>

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

void Renderer::drawGradientRect(const Rect& rect, const Color& colorTop, const Color& colorBottom) {
    // Draw vertical gradient line by line
    for (int y = 0; y < rect.h; ++y) {
        float t = (float)y / (float)rect.h;
        
        uint8_t r = (uint8_t)(colorTop.r + t * (colorBottom.r - colorTop.r));
        uint8_t g = (uint8_t)(colorTop.g + t * (colorBottom.g - colorTop.g));
        uint8_t b = (uint8_t)(colorTop.b + t * (colorBottom.b - colorTop.b));
        uint8_t a = (uint8_t)(colorTop.a + t * (colorBottom.a - colorTop.a));
        
        SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a);
        SDL_RenderDrawLine(sdlRenderer, rect.x, rect.y + y, rect.x + rect.w, rect.y + y);
    }
}

void Renderer::drawRoundedRect(const Rect& rect, const Color& color, int radius, bool filled) {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    
    if (radius <= 0 || radius > rect.w / 2 || radius > rect.h / 2) {
        // Fallback to regular rectangle
        drawRect(rect, color, filled);
        return;
    }
    
    // Draw rounded corners cleanly
    if (filled) {
        // Center rectangle (horizontal)
        SDL_Rect centerH = { rect.x, rect.y + radius, rect.w, rect.h - 2 * radius };
        SDL_RenderFillRect(sdlRenderer, &centerH);
        
        // Top and bottom rectangles
        SDL_Rect topRect = { rect.x + radius, rect.y, rect.w - 2 * radius, radius };
        SDL_RenderFillRect(sdlRenderer, &topRect);
        
        SDL_Rect bottomRect = { rect.x + radius, rect.y + rect.h - radius, rect.w - 2 * radius, radius };
        SDL_RenderFillRect(sdlRenderer, &bottomRect);
        
        // Four corner circles (filled)
        drawCircle(rect.x + radius, rect.y + radius, radius, color, true);
        drawCircle(rect.x + rect.w - radius - 1, rect.y + radius, radius, color, true);
        drawCircle(rect.x + radius, rect.y + rect.h - radius - 1, radius, color, true);
        drawCircle(rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius, color, true);
    } else {
        // Draw outline only - use SDL for clean lines
        for (int i = 0; i < 2; ++i) {
            // Top line
            SDL_RenderDrawLine(sdlRenderer, rect.x + radius, rect.y + i, rect.x + rect.w - radius, rect.y + i);
            // Bottom line
            SDL_RenderDrawLine(sdlRenderer, rect.x + radius, rect.y + rect.h - i, rect.x + rect.w - radius, rect.y + rect.h - i);
            // Left line
            SDL_RenderDrawLine(sdlRenderer, rect.x + i, rect.y + radius, rect.x + i, rect.y + rect.h - radius);
            // Right line
            SDL_RenderDrawLine(sdlRenderer, rect.x + rect.w - i, rect.y + radius, rect.x + rect.w - i, rect.y + rect.h - radius);
        }
        
        // Corner arcs using circle outline
        drawCircle(rect.x + radius, rect.y + radius, radius, color, false);
        drawCircle(rect.x + rect.w - radius - 1, rect.y + radius, radius, color, false);
        drawCircle(rect.x + radius, rect.y + rect.h - radius - 1, radius, color, false);
        drawCircle(rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius, color, false);
    }
}

void Renderer::drawShadow(const Rect& rect, int offset, int blur) {
    // Draw soft, realistic shadow with Gaussian-like falloff
    int steps = blur;
    if (steps < 4) steps = 4;
    if (steps > 20) steps = 20;
    
    for (int i = steps - 1; i >= 0; --i) {
        float t = (float)i / (float)steps;
        // Gaussian-like falloff for smoother shadows
        float gaussian = expf(-3.0f * t * t);
        uint8_t alpha = (uint8_t)(35 * gaussian);
        
        if (alpha < 2) continue;
        
        Color shadowColor(0, 0, 0, alpha);
        Rect shadowRect(
            rect.x + offset - blur/2 + (int)(t * blur),
            rect.y + offset - blur/2 + (int)(t * blur),
            rect.w + blur - (int)(t * blur * 2),
            rect.h + blur - (int)(t * blur * 2)
        );
        
        drawRoundedRect(shadowRect, shadowColor, 12 + i/2, true);
    }
}

void Renderer::drawCircle(int centerX, int centerY, int radius, const Color& color, bool filled) {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    
    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;
    
    while (x >= y) {
        if (filled) {
            // Draw horizontal lines to fill the circle
            SDL_RenderDrawLine(sdlRenderer, centerX - x, centerY + y, centerX + x, centerY + y);
            SDL_RenderDrawLine(sdlRenderer, centerX - x, centerY - y, centerX + x, centerY - y);
            SDL_RenderDrawLine(sdlRenderer, centerX - y, centerY + x, centerX + y, centerY + x);
            SDL_RenderDrawLine(sdlRenderer, centerX - y, centerY - x, centerX + y, centerY - x);
        } else {
            // Draw outline points
            SDL_RenderDrawPoint(sdlRenderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(sdlRenderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(sdlRenderer, centerX + x, centerY - y);
            SDL_RenderDrawPoint(sdlRenderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(sdlRenderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(sdlRenderer, centerX - y, centerY + x);
            SDL_RenderDrawPoint(sdlRenderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(sdlRenderer, centerX - y, centerY - x);
        }
        
        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness) {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    
    if (thickness <= 1) {
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
    } else {
        // Draw thick line by drawing multiple parallel lines
        int halfThick = thickness / 2;
        
        // Calculate perpendicular offset based on line direction
        float dx = (float)(x2 - x1);
        float dy = (float)(y2 - y1);
        float length = sqrtf(dx * dx + dy * dy);
        
        if (length > 0) {
            float perpX = -dy / length;
            float perpY = dx / length;
            
            for (int i = -halfThick; i <= halfThick; ++i) {
                int offsetX = (int)(perpX * i);
                int offsetY = (int)(perpY * i);
                SDL_RenderDrawLine(sdlRenderer, 
                    x1 + offsetX, y1 + offsetY, 
                    x2 + offsetX, y2 + offsetY);
            }
        }
    }
}

void Renderer::drawGlassCard(const Rect& rect, int radius, float opacity) {
    // Glassmorphism effect: semi-transparent with subtle gradient
    Color glassBase(255, 255, 255, (uint8_t)(opacity * 255));
    Color glassHighlight(255, 255, 255, (uint8_t)(opacity * 1.5f * 255));
    
    // Base glass layer
    drawRoundedRect(rect, glassBase, radius, true);
    
    // Top highlight for depth
    Rect highlightRect(rect.x, rect.y, rect.w, rect.h / 3);
    drawGradientRect(highlightRect, glassHighlight, Color(255, 255, 255, 0));
    
    // Subtle border
    Color borderColor(255, 255, 255, (uint8_t)(opacity * 2.0f * 255));
    drawRoundedRect(rect, borderColor, radius, false);
}

void Renderer::drawRadialGradient(int centerX, int centerY, int radius, const Color& centerColor, const Color& edgeColor) {
    // Draw concentric circles with color interpolation
    int steps = radius / 2;
    if (steps < 10) steps = 10;
    if (steps > 50) steps = 50;
    
    for (int i = steps; i > 0; --i) {
        float t = (float)i / (float)steps;
        int currentRadius = (int)(radius * t);
        
        uint8_t r = (uint8_t)(centerColor.r + (edgeColor.r - centerColor.r) * t);
        uint8_t g = (uint8_t)(centerColor.g + (edgeColor.g - centerColor.g) * t);
        uint8_t b = (uint8_t)(centerColor.b + (edgeColor.b - centerColor.b) * t);
        uint8_t a = (uint8_t)(centerColor.a + (edgeColor.a - centerColor.a) * t);
        
        drawCircle(centerX, centerY, currentRadius, Color(r, g, b, a), true);
    }
}

} // namespace AOS
