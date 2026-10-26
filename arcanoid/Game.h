#pragma once
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Paddle.h"
#include "Ball.h"
#include "GameElement.h"

class Game {
private:
    sf::RenderWindow window;
    Paddle paddle;

    std::vector<std::unique_ptr<Ball>> balls;
    std::vector<std::unique_ptr<GameElement>> elements;

    int score;
    int lives;

    bool safetyFloorActive;
    sf::RectangleShape safetyFloorVisual;

    bool stickyModifierActive;

    void initBlocks();
    void processEvents();
    void update(float deltaTime);
    void render();
    void checkCollisions();
    void updateTitle();

public:
    Game();
    void run();
    void addScore(int points) { score += points; }
    void spawnRandomBonus(float x, float y);
    void modifyBallsSpeed(float factor);
    void activateStickyModifier() { stickyModifierActive = true; }
    void activateSafetyFloor() { safetyFloorActive = true; }
    void spawnAdditionalBall();
};

#endif
