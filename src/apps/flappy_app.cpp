#include "flappy_app.h"
#include "os/app_manager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

extern AOS::AppManager* g_appManager;

namespace AOS {

FlappyApp::FlappyApp()
    : state(MENU)
    , score(0)
    , highScore(0)
    , rng(std::random_device{}())
    , gapDistribution(150, 500)
    , gameTime(0.0f)
    , groundOffset(0.0f)
{
}

void FlappyApp::onStart() {
    std::cout << "FlappyApp: Started" << std::endl;
    state = MENU;
    score = 0;
    gameTime = 0.0f;
}

void FlappyApp::onStop() {
    std::cout << "FlappyApp: Stopped (High Score: " << highScore << ")" << std::endl;
}

void FlappyApp::update(float deltaTime) {
    gameTime += deltaTime;

    if (state == PLAYING) {
        updatePhysics(deltaTime);
        updatePipes(deltaTime);
        checkScoring();
        checkCollisions();

        // Animate ground scrolling
        groundOffset += PIPE_SPEED * deltaTime;
        if (groundOffset > 50) groundOffset = 0;
    }
}

void FlappyApp::render(Renderer& renderer) {
    // Sky background
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), renderer.getHeight()),
                      Color(135, 206, 235), true);

    switch (state) {
        case MENU:
            renderMenu(renderer);
            break;
        case PLAYING:
            renderGame(renderer);
            break;
        case GAME_OVER:
            renderGameOver(renderer);
            break;
    }
}

void FlappyApp::onEvent(const Event& event) {
    if (event.type == EventType::KEY_BACK) {
        std::cout << "FlappyApp: Returning to home" << std::endl;
        if (g_appManager) {
            g_appManager->returnToHome();
        }
    } else if (event.type == EventType::KEY_SELECT) {
        if (state == MENU) {
            resetGame();
            state = PLAYING;
            std::cout << "FlappyApp: Game started!" << std::endl;
        } else if (state == PLAYING) {
            flap();
        } else if (state == GAME_OVER) {
            state = MENU;
        }
    } else if (event.type == EventType::KEY_UP) {
        // Also allow UP arrow for flapping
        if (state == PLAYING) {
            flap();
        }
    }
}

// ===== GAME LOGIC =====

void FlappyApp::resetGame() {
    bird = Bird();
    pipes.clear();
    score = 0;
    groundOffset = 0.0f;

    // Spawn initial pipes
    for (int i = 0; i < 4; i++) {
        spawnPipe();
    }
}

void FlappyApp::flap() {
    bird.velocity = FLAP_VELOCITY;
    bird.rotation = -30.0f; // Tilt up
    std::cout << "FlappyApp: Flap!" << std::endl;
}

void FlappyApp::updatePhysics(float deltaTime) {
    // Apply gravity
    bird.velocity += GRAVITY * deltaTime;

    // Clamp velocity
    if (bird.velocity > MAX_VELOCITY) {
        bird.velocity = MAX_VELOCITY;
    }

    // Update position
    bird.y += bird.velocity * deltaTime;

    // Update rotation based on velocity
    bird.rotation = std::min(90.0f, (bird.velocity / MAX_VELOCITY) * 90.0f);
}

void FlappyApp::updatePipes(float deltaTime) {
    // Move pipes left
    for (auto& pipe : pipes) {
        pipe.x -= PIPE_SPEED * deltaTime;
    }

    // Remove off-screen pipes
    pipes.erase(
        std::remove_if(pipes.begin(), pipes.end(),
                      [](const Pipe& p) { return p.x < -100; }),
        pipes.end()
    );

    // Spawn new pipes if needed
    if (pipes.empty() || pipes.back().x < 1280 - PIPE_SPACING) {
        spawnPipe();
    }
}

void FlappyApp::checkCollisions() {
    // Check ground collision
    if (checkBirdGroundCollision()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
            std::cout << "FlappyApp: New high score! " << highScore << std::endl;
        }
        std::cout << "FlappyApp: Game Over! Score: " << score << std::endl;
        return;
    }

    // Check ceiling collision
    if (checkBirdCeilingCollision()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
        }
        std::cout << "FlappyApp: Game Over! Score: " << score << std::endl;
        return;
    }

    // Check pipe collisions
    for (const auto& pipe : pipes) {
        if (checkBirdPipeCollision(pipe)) {
            state = GAME_OVER;
            if (score > highScore) {
                highScore = score;
            }
            std::cout << "FlappyApp: Game Over! Score: " << score << std::endl;
            return;
        }
    }
}

void FlappyApp::checkScoring() {
    for (auto& pipe : pipes) {
        // Check if bird passed the pipe
        if (!pipe.scored && bird.x > pipe.x + pipe.width) {
            pipe.scored = true;
            score++;
            std::cout << "FlappyApp: Score! " << score << std::endl;
        }
    }
}

void FlappyApp::spawnPipe() {
    float x = pipes.empty() ? 1280.0f : pipes.back().x + PIPE_SPACING;
    float gapY = (float)gapDistribution(rng);
    pipes.emplace_back(x, gapY, PIPE_GAP);
}

// ===== COLLISION DETECTION =====

bool FlappyApp::checkBirdPipeCollision(const Pipe& pipe) const {
    // Bird bounding box
    float birdLeft = bird.x;
    float birdRight = bird.x + bird.width;
    float birdTop = bird.y;
    float birdBottom = bird.y + bird.height;

    // Pipe bounding boxes
    float pipeLeft = pipe.x;
    float pipeRight = pipe.x + pipe.width;

    // Check if bird is horizontally aligned with pipe
    if (birdRight > pipeLeft && birdLeft < pipeRight) {
        // Check if bird hits top pipe
        if (birdTop < pipe.getTopHeight()) {
            return true;
        }

        // Check if bird hits bottom pipe
        if (birdBottom > pipe.getBottomY()) {
            return true;
        }
    }

    return false;
}

bool FlappyApp::checkBirdGroundCollision() const {
    return bird.y + bird.height >= 720 - GROUND_HEIGHT;
}

bool FlappyApp::checkBirdCeilingCollision() const {
    return bird.y <= 0;
}

// ===== RENDERING =====

void FlappyApp::renderMenu(Renderer& renderer) {
    int centerX = renderer.getWidth() / 2;
    int centerY = renderer.getHeight() / 2;

    // Title
    renderer.drawText("FLAPPY BIRD", centerX - 120, centerY - 150, Color(255, 255, 0), 32);

    // Bird preview (animated)
    float bobY = std::sin(gameTime * 3.0f) * 10.0f;
    int birdX = centerX - 17;
    int birdY = centerY - 60 + (int)bobY;

    // Bird body
    renderer.drawRect(Rect(birdX, birdY, 34, 24), Color(255, 200, 0), true);
    // Bird wing
    renderer.drawRect(Rect(birdX + 5, birdY + 8, 15, 8), Color(255, 150, 0), true);
    // Bird eye
    renderer.drawRect(Rect(birdX + 24, birdY + 6, 6, 6), Color(255, 255, 255), true);
    renderer.drawRect(Rect(birdX + 26, birdY + 8, 3, 3), Color(0, 0, 0), true);
    // Bird beak
    renderer.drawRect(Rect(birdX + 30, birdY + 12, 8, 4), Color(255, 100, 0), true);

    // Instructions
    renderer.drawText("Press ENTER or UP to start", centerX - 160, centerY + 40, Color::White(), 20);
    renderer.drawText("Press ENTER/UP to flap", centerX - 130, centerY + 80, Color(200, 200, 200), 18);
    renderer.drawText("Avoid the pipes!", centerX - 90, centerY + 110, Color(200, 200, 200), 18);

    // High score
    if (highScore > 0) {
        char highScoreText[32];
        snprintf(highScoreText, sizeof(highScoreText), "High Score: %d", highScore);
        renderer.drawText(highScoreText, centerX - 80, centerY + 160, Color(255, 255, 100), 20);
    }

    // Ground
    renderGround(renderer);

    // Controls
    renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void FlappyApp::renderGame(Renderer& renderer) {
    // Render pipes
    renderPipes(renderer);

    // Render ground
    renderGround(renderer);

    // Render bird
    renderBird(renderer);

    // Render score
    renderScore(renderer);

    // Instructions
    renderer.drawText("ENTER/UP: Flap | ESC: Exit", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void FlappyApp::renderGameOver(Renderer& renderer) {
    // Render game elements (frozen state)
    renderPipes(renderer);
    renderGround(renderer);
    renderBird(renderer);

    // Semi-transparent overlay
    renderer.drawRect(Rect(0, 0, renderer.getWidth(), renderer.getHeight()),
                      Color(0, 0, 0, 150), true);

    int centerX = renderer.getWidth() / 2;
    int centerY = renderer.getHeight() / 2;

    // Game Over box
    renderer.drawRect(Rect(centerX - 200, centerY - 150, 400, 250),
                      Color(40, 40, 40), true);
    renderer.drawRect(Rect(centerX - 200, centerY - 150, 400, 250),
                      Color(200, 200, 200), false);

    // Game Over text
    renderer.drawText("GAME OVER", centerX - 100, centerY - 120, Color(255, 100, 100), 28);

    // Score
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    renderer.drawText(scoreText, centerX - 60, centerY - 60, Color::White(), 24);

    // High score
    char highScoreText[32];
    snprintf(highScoreText, sizeof(highScoreText), "High Score: %d", highScore);
    renderer.drawText(highScoreText, centerX - 80, centerY - 20, Color(255, 255, 100), 20);

    // New high score indicator
    if (score == highScore && score > 0) {
        renderer.drawText("NEW HIGH SCORE!", centerX - 100, centerY + 20, Color(255, 215, 0), 20);
    }

    // Restart instruction
    renderer.drawText("Press ENTER to return to menu", centerX - 150, centerY + 60, Color(200, 200, 200), 18);

    // Controls
    renderer.drawText("Press ESC to return to Home", 20, renderer.getHeight() - 50, Color(150, 150, 150), 18);
}

void FlappyApp::renderBird(Renderer& renderer) {
    int birdX = (int)bird.x;
    int birdY = (int)bird.y;

    // Bird body (yellow circle approximation with rectangles)
    renderer.drawRect(Rect(birdX, birdY, bird.width, bird.height), Color(255, 200, 0), true);

    // Bird outline
    renderer.drawRect(Rect(birdX, birdY, bird.width, bird.height), Color(200, 150, 0), false);

    // Bird wing
    renderer.drawRect(Rect(birdX + 5, birdY + 8, 15, 8), Color(255, 150, 0), true);

    // Bird eye
    renderer.drawRect(Rect(birdX + 24, birdY + 6, 6, 6), Color(255, 255, 255), true);
    renderer.drawRect(Rect(birdX + 26, birdY + 8, 3, 3), Color(0, 0, 0), true);

    // Bird beak
    renderer.drawRect(Rect(birdX + 30, birdY + 12, 8, 4), Color(255, 100, 0), true);

    // Debug collision box (optional)
    // renderer.drawRect(Rect(birdX, birdY, bird.width, bird.height), Color(255, 0, 0), false);
}

void FlappyApp::renderPipes(Renderer& renderer) {
    for (const auto& pipe : pipes) {
        int pipeX = (int)pipe.x;

        // Top pipe
        int topHeight = pipe.getTopHeight();
        renderer.drawRect(Rect(pipeX, 0, pipe.width, topHeight),
                         Color(50, 200, 50), true);
        renderer.drawRect(Rect(pipeX, 0, pipe.width, topHeight),
                         Color(40, 160, 40), false);

        // Top pipe cap
        renderer.drawRect(Rect(pipeX - 5, topHeight - 30, pipe.width + 10, 30),
                         Color(60, 220, 60), true);
        renderer.drawRect(Rect(pipeX - 5, topHeight - 30, pipe.width + 10, 30),
                         Color(40, 160, 40), false);

        // Bottom pipe
        int bottomY = pipe.getBottomY();
        int bottomHeight = 720 - GROUND_HEIGHT - bottomY;
        renderer.drawRect(Rect(pipeX, bottomY, pipe.width, bottomHeight),
                         Color(50, 200, 50), true);
        renderer.drawRect(Rect(pipeX, bottomY, pipe.width, bottomHeight),
                         Color(40, 160, 40), false);

        // Bottom pipe cap
        renderer.drawRect(Rect(pipeX - 5, bottomY, pipe.width + 10, 30),
                         Color(60, 220, 60), true);
        renderer.drawRect(Rect(pipeX - 5, bottomY, pipe.width + 10, 30),
                         Color(40, 160, 40), false);

        // Debug gap indicator (optional)
        // renderer.drawRect(Rect(pipeX, (int)pipe.gapY - 2, pipe.width, 4), Color(255, 0, 0), true);
    }
}

void FlappyApp::renderGround(Renderer& renderer) {
    int groundY = renderer.getHeight() - GROUND_HEIGHT;

    // Ground base
    renderer.drawRect(Rect(0, groundY, renderer.getWidth(), GROUND_HEIGHT),
                      Color(210, 180, 140), true);

    // Ground pattern (animated scrolling)
    int patternWidth = 50;
    int offset = (int)groundOffset;
    for (int x = -offset; x < renderer.getWidth(); x += patternWidth) {
        renderer.drawRect(Rect(x, groundY, 2, GROUND_HEIGHT),
                         Color(180, 150, 110), true);
    }

    // Grass on top
    renderer.drawRect(Rect(0, groundY, renderer.getWidth(), 5),
                      Color(100, 180, 50), true);
}

void FlappyApp::renderScore(Renderer& renderer) {
    int centerX = renderer.getWidth() / 2;

    char scoreText[16];
    snprintf(scoreText, sizeof(scoreText), "%d", score);

    // Score shadow
    renderer.drawText(scoreText, centerX - 12, 52, Color(0, 0, 0), 48);

    // Score text
    renderer.drawText(scoreText, centerX - 15, 50, Color(255, 255, 255), 48);
}

} // namespace AOS
