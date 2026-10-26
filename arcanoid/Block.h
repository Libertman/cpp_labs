#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "GameElement.h"

class Block : public GameElement {
protected:
    sf::RectangleShape shape;

public:
    Block(float x, float y, float width, sf::Color color) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, BLOCK_HEIGHT));
        shape.setFillColor(color);
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
    sf::FloatRect getBounds() const override { return shape.getGlobalBounds(); }
    virtual bool isBreakable() const override { return true; }

    void onBallCollision(Ball& ball, Game& game) override {
        ball.resolveCollision(shape.getGlobalBounds());
    }
};

class NormalBlock : public Block {
public:
    NormalBlock(float x, float y, float width) : Block(x, y, width, sf::Color::Red) {}
    void onBallCollision(Ball& ball, Game& game) override {
        Block::onBallCollision(ball, game);
        destroyed = true;
        game.addScore(1);
    }
};

class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y, float width) : Block(x, y, width, sf::Color(128, 128, 128)) {}
    bool isBreakable() const override { return false; }
};

class SpeedUpBlock : public Block {
public:
    SpeedUpBlock(float x, float y, float width) : Block(x, y, width, sf::Color::Magenta) {}
    void onBallCollision(Ball& ball, Game& game) override {
        Block::onBallCollision(ball, game);
        ball.multiplySpeed(1.2f);
        destroyed = true;
        game.addScore(1);
    }
};

class DurableBlock : public Block {
private:
    int health = 3;
public:
    DurableBlock(float x, float y, float width) : Block(x, y, width, sf::Color::Blue) {}
    void onBallCollision(Ball& ball, Game& game) override {
        Block::onBallCollision(ball, game);
        health--;
        game.addScore(1);
        if (health == 2) shape.setFillColor(sf::Color(0, 0, 180));
        if (health == 1) shape.setFillColor(sf::Color(100, 100, 255));
        if (health <= 0) destroyed = true;
    }
};

class BonusBlock : public Block {
public:
    BonusBlock(float x, float y, float width) : Block(x, y, width, sf::Color::Yellow) {}
    void onBallCollision(Ball& ball, Game& game) override {
        Block::onBallCollision(ball, game);
        destroyed = true;
        game.addScore(1);
        game.spawnRandomBonus(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y);
    }
};


#endif
