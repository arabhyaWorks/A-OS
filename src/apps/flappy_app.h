#pragma once

#include "os/app.h"
#include "ui/renderer.h"
#include <vector>
#include <random>

namespace AOS {

/**
 * FlappyApp - Flappy Bird Clone
 *
 * Complete game implementation with:
 * - Physics simulation (gravity, velocity)
 * - Collision detection
 * - Procedural pipe generation
 * - Scoring system
 * - High score tracking
 * - Multiple game states
 *
 * Controls:
 * - SPACE/ENTER: Flap (jump)
 * - ESC: Return to home
 */
class FlappyApp : public App {
public:
    FlappyApp();
    ~FlappyApp() override = default;

    void onStart() override;
    void onStop() override;
    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
    void onEvent(const Event& event) override;

    std::string getName() const override { return "Flappy Bird"; }

private:
    // Game states
    enum GameState {
        MENU,
        PLAYING,
        GAME_OVER
    };

    // Bird structure
    struct Bird {
        float x, y;           // Position
        float velocity;       // Vertical velocity
        float rotation;       // Visual rotation angle
        int width, height;    // Collision box

        Bird() : x(200), y(300), velocity(0), rotation(0), width(34), height(24) {}
    };

    // Pipe structure
    struct Pipe {
        float x;              // Horizontal position
        float gapY;           // Center Y of the gap
        int gapSize;          // Size of the gap
        int width;            // Pipe width
        bool scored;          // Has the player scored from this pipe?

        Pipe(float posX, float gapYPos, int gap)
            : x(posX), gapY(gapYPos), gapSize(gap), width(80), scored(false) {}

        // Get top pipe bounds
        int getTopHeight() const { return (int)(gapY - gapSize / 2); }

        // Get bottom pipe top position
        int getBottomY() const { return (int)(gapY + gapSize / 2); }
    };

    // Game state
    GameState state;
    Bird bird;
    std::vector<Pipe> pipes;

    // Physics constants
    const float GRAVITY = 1200.0f;          // Pixels per second^2
    const float FLAP_VELOCITY = -400.0f;    // Initial jump velocity
    const float MAX_VELOCITY = 800.0f;      // Terminal velocity
    const float PIPE_SPEED = 200.0f;        // Pixels per second

    // Game constants
    const int PIPE_GAP = 180;               // Gap between pipes
    const int PIPE_SPACING = 300;           // Horizontal spacing
    const int GROUND_HEIGHT = 100;          // Ground level

    // Scoring
    int score;
    int highScore;

    // Random number generation
    std::mt19937 rng;
    std::uniform_int_distribution<int> gapDistribution;

    // Animation
    float gameTime;
    float groundOffset;

    // Game methods
    void resetGame();
    void flap();
    void updatePhysics(float deltaTime);
    void updatePipes(float deltaTime);
    void checkCollisions();
    void checkScoring();
    void spawnPipe();

    // Collision detection
    bool checkBirdPipeCollision(const Pipe& pipe) const;
    bool checkBirdGroundCollision() const;
    bool checkBirdCeilingCollision() const;

    // Rendering methods
    void renderMenu(Renderer& renderer);
    void renderGame(Renderer& renderer);
    void renderGameOver(Renderer& renderer);
    void renderBird(Renderer& renderer);
    void renderPipes(Renderer& renderer);
    void renderGround(Renderer& renderer);
    void renderScore(Renderer& renderer);
};

} // namespace AOS
