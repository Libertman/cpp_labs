#pragma once
#ifndef BONUS_H
#define BONUS_H

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "GameElement.h"

class Bonus : public GameElement {
private:
    sf::CircleShape shape;
    float speed = 150.0f;
public:
    Bonus(float x, float y, sf::Color color) {
        shape.setRadius(8.0f);
        shape.setOrigin(8.0f, 8.0f);
        shape.setPosition(x, y);
        shape.setFillColor(color);
    }
    void update(float deltaTime) override {
        shape.move(0, speed * deltaTime);
        if (shape.getPosition().y > WINDOW_HEIGHT) destroyed = true;
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }

    sf::FloatRect getBounds() const override { return shape.getGlobalBounds(); }
    void onBallCollision(Ball& ball, Game& game) override {}
};

class PaddleSizeBonus : public Bonus {
public:
    PaddleSizeBonus(float x, float y) : Bonus(x, y, sf::Color::Green) {}
    void onPaddleCollision(Paddle& paddle, Game& game) override {
        destroyed = true;
        paddle.setWidth(180.0f);
    }
};

class BallSpeedBonus : public Bonus {
public:
    BallSpeedBonus(float x, float y) : Bonus(x, y, sf::Color::Magenta) {}
    void onPaddleCollision(Paddle& paddle, Game& game) override {
        destroyed = true;
        game.modifyBallsSpeed(0.6f);
    }
};

class StickyPaddleBonus : public Bonus {
public:
    StickyPaddleBonus(float x, float y) : Bonus(x, y, sf::Color(255, 128, 0)) {}
    void onPaddleCollision(Paddle& paddle, Game& game) override {
        destroyed = true;
        game.activateStickyModifier();
    }
};

class SafetyFloorBonus : public Bonus {
public:
    SafetyFloorBonus(float x, float y) : Bonus(x, y, sf::Color::Blue) {}
    void onPaddleCollision(Paddle& paddle, Game& game) override {
        destroyed = true;
        game.activateSafetyFloor();
    }
};

class SecondBallBonus : public Bonus {
public:
    SecondBallBonus(float x, float y) : Bonus(x, y, sf::Color::White) {}
    void onPaddleCollision(Paddle& paddle, Game& game) override {
        destroyed = true;
        game.spawnAdditionalBall();
    }
};


#endif
