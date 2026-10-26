#include "Game.h"
#include "Config.h"
#include <Block.h>
#include <Bonus.h>
#include <string>
#include <ctime>

Game::Game() :
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid"),
    score(0), lives(3), safetyFloorActive(false), stickyModifierActive(false)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    balls.push_back(std::make_unique<Ball>(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f - BALL_RADIUS, true));
    initBlocks();
    
    safetyFloorVisual.setSize(sf::Vector2f(WINDOW_WIDTH, 8.0f));
    safetyFloorVisual.setFillColor(sf::Color::Blue);
    safetyFloorVisual.setPosition(0, WINDOW_HEIGHT - 8.0f);
}

void Game::initBlocks() {
    float blockWidth = (WINDOW_WIDTH - (BLOCK_COLS + 1) * BLOCK_PADDING) / BLOCK_COLS;

    for (int r = 0; r < BLOCK_ROWS; ++r) {
        for (int c = 0; c < BLOCK_COLS; ++c) {
            float x = BLOCK_PADDING + c * (blockWidth + BLOCK_PADDING);
            float y = BLOCK_PADDING + r * (BLOCK_HEIGHT + BLOCK_PADDING) + 60.0f;

            int randType = std::rand() % 5;
            if (randType == 0) elements.push_back(std::make_unique<IndestructibleBlock>(x, y, blockWidth));
            else if (randType == 1) elements.push_back(std::make_unique<SpeedUpBlock>(x, y, blockWidth));
            else if (randType == 2) elements.push_back(std::make_unique<DurableBlock>(x, y, blockWidth));
            else if (randType == 3) elements.push_back(std::make_unique<BonusBlock>(x, y, blockWidth));
            else elements.push_back(std::make_unique<NormalBlock>(x, y, blockWidth));
        }
    }
}

void Game::spawnRandomBonus(float x, float y) {
    int r = std::rand() % 5;
    if (r == 0) elements.push_back(std::make_unique<PaddleSizeBonus>(x, y));
    else if (r == 1) elements.push_back(std::make_unique<BallSpeedBonus>(x, y));
    else if (r == 2) elements.push_back(std::make_unique<StickyPaddleBonus>(x, y));
    else if (r == 3) elements.push_back(std::make_unique<SafetyFloorBonus>(x, y));
    else elements.push_back(std::make_unique<SecondBallBonus>(x, y));
}

void Game::modifyBallsSpeed(float factor) {
    for (auto& ball : balls) ball->multiplySpeed(factor);
}

void Game::spawnAdditionalBall() {
    balls.push_back(std::make_unique<Ball>(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, false));
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            for (auto& ball : balls) {
                if (ball->isStuck()) ball->launch();
            }
        }
    }
}

void Game::update(float deltaTime) {
    paddle.update(deltaTime);
    
    for (auto& ball : balls) {
        ball->update(deltaTime, paddle.getBounds());
    }
    for (auto& element : elements) {
        element->update(deltaTime);
    }

    checkCollisions();
    updateTitle();
}

void Game::checkCollisions() {
    sf::FloatRect paddleBounds = paddle.getBounds();

    for (auto& ball : balls) {
        if (ball->isStuck()) continue;

        if (ball->getBounds().intersects(paddleBounds)) {
            if (stickyModifierActive) {
                ball->stick(paddleBounds.left);
                stickyModifierActive = false;
            } else {
                ball->resolveCollision(paddleBounds);
            }
        }

        for (auto& element : elements) {
            if (!element->isDestroyed() && ball->getBounds().intersects(element->getBounds())) {
                element->onBallCollision(*ball, *this);
            }
        }
    }

    for (auto& element : elements) {
        if (!element->isDestroyed() && paddleBounds.intersects(element->getBounds())) {
            element->onPaddleCollision(paddle, *this);
        }
    }

    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            sf::Vector2f p1 = balls[i]->getPosition();
            sf::Vector2f p2 = balls[j]->getPosition();
            float distance = std::sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
            if (distance < BALL_RADIUS * 2) {
                balls[i]->bounceX(); balls[i]->bounceY();
                balls[j]->bounceX(); balls[j]->bounceY();
            }
        }
    }

    elements.erase(std::remove_if(elements.begin(), elements.end(),
        [](const std::unique_ptr<GameElement>& e) { return e->isDestroyed(); }), elements.end());

    for (auto it = balls.begin(); it != balls.end();) {
        if ((*it)->getPosition().y + BALL_RADIUS > WINDOW_HEIGHT) {
            if (safetyFloorActive) {
                (*it)->bounceY();
                safetyFloorActive = false;
                ++it;
            } else {
                it = balls.erase(it);
            }
        } else {
            ++it;
        }
    }

    if (balls.empty()) {
        lives--;
        paddle.shrink();
        stickyModifierActive = false;
        safetyFloorActive = false;
        balls.push_back(std::make_unique<Ball>(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f - BALL_RADIUS, true));
        
        if (lives <= 0) {
            lives = 3;
            score = 0;
            elements.clear();
            initBlocks();
        }
    }

    bool breakableRemaining = false;
    for (const auto& element : elements) {
        if (element->isBreakable() && !element->isDestroyed()) {
            breakableRemaining = true;
            break;
        }
    }

    if (!breakableRemaining) {
        score += 500;
        elements.clear();
        
        balls.clear();
        balls.push_back(std::make_unique<Ball>(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f - BALL_RADIUS, true));
        
        stickyModifierActive = false;
        safetyFloorActive = false;
        paddle.reset();
        
        initBlocks();
    }
}

void Game::updateTitle() {
    std::string title = "Arkanoid Polymorphic | Score: " + std::to_string(score) + 
                        " | Lives: " + std::to_string(lives) + " | Balls: " + std::to_string(balls.size()) +
                        (safetyFloorActive ? " | [FLOOR SHIELD]" : "") + 
                        (stickyModifierActive ? " | [STICKY READY]" : "");
    window.setTitle(title);
}

void Game::render() {
    window.clear(sf::Color::Black);
    paddle.draw(window);
    for (auto& ball : balls) ball->draw(window);
    for (auto& element : elements) element->draw(window);
    if (safetyFloorActive) window.draw(safetyFloorVisual);
    window.display();
}
