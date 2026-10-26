#pragma once
#ifndef BALL_H
#define BALL_H

#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool stuck;
    float stuckOffsetX;

public:
    Ball(float x, float y, bool startStuck = false);
    void update(float deltaTime, const sf::FloatRect& paddleBounds);
    void draw(sf::RenderWindow& window);
    
    void bounceX();
    void bounceY();
    void multiplySpeed(float factor);
    void setVelocity(sf::Vector2f vel);
    sf::Vector2f getVelocity() const { return velocity; }
    
    bool isStuck() const { return stuck; }
    void stick(float paddleLeft);
    void launch();

    void resolveCollision(const sf::FloatRect& objectBounds);
    
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
};

#endif
