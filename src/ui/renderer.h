#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>

namespace AOS {

/**
 * Color structure
 */
struct Color {
    uint8_t r, g, b, a;

    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    static Color Black() { return Color(0, 0, 0); }
    static Color White() { return Color(255, 255, 255); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    static Color Gray() { return Color(128, 128, 128); }
};

/**
 * Rectangle structure
 */
struct Rect {
    int x, y, w, h;

    Rect(int xPos, int yPos, int width, int height)
        : x(xPos), y(yPos), w(width), h(height) {}
};

/**
 * Renderer - Abstraction over SDL2 rendering
 *
 * Provides simple drawing primitives for apps.
 * On desktop: renders to SDL window
 * On Pi: renders to framebuffer via SDL
 *
 * This keeps apps independent of the underlying rendering system.
 */
class Renderer {
public:
    Renderer(SDL_Window* window, SDL_Renderer* sdlRenderer);
    ~Renderer();

    // Frame management
    void clear(const Color& color = Color::Black());
    void present();

    // Drawing primitives
    void drawRect(const Rect& rect, const Color& color, bool filled = false);
    void drawText(const std::string& text, int x, int y, const Color& color, int fontSize = 24);
    
    // Advanced drawing
    void drawGradientRect(const Rect& rect, const Color& colorTop, const Color& colorBottom);
    void drawRoundedRect(const Rect& rect, const Color& color, int radius, bool filled = true);
    void drawShadow(const Rect& rect, int offset = 4, int blur = 8);
    void drawCircle(int centerX, int centerY, int radius, const Color& color, bool filled = true);
    void drawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness = 1);
    void drawGlassCard(const Rect& rect, int radius, float opacity = 0.15f);
    void drawRadialGradient(int centerX, int centerY, int radius, const Color& centerColor, const Color& edgeColor);

    // Font management
    bool loadFont(const std::string& path, int size);
    TTF_Font* getFont(int size);

    // Get screen dimensions
    int getWidth() const { return screenWidth; }
    int getHeight() const { return screenHeight; }

    // Get SDL renderer (for advanced operations like screenshots)
    SDL_Renderer* getSDLRenderer() { return sdlRenderer; }

private:
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;
    int screenWidth;
    int screenHeight;

    // Font cache: size -> TTF_Font
    std::map<int, TTF_Font*> fontCache;
    std::string defaultFontPath;
};

} // namespace AOS
