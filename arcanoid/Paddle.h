#pragma once
#ifndef PADDLE_H
#define PADDLE_H

#include <SFML/Graphics.hpp>

class Paddle {
private:
    sf::RectangleShape shape;
    float speed;
    float width;

public:
    Paddle();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setWidth(float newWidth);
    void shrink();
    sf::FloatRect getBounds() const;
    void reset();
};

#endif
