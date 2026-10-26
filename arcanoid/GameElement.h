#ifndef GAME_ELEMENT_H
#define GAME_ELEMENT_H

#include <SFML/Graphics.hpp>

class Ball;
class Paddle;
class Game;

class GameElement {
protected:
    bool destroyed = false;

public:
    virtual ~GameElement() = default;
    
    virtual void update(float deltaTime) {}
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    
    virtual void onBallCollision(Ball& ball, Game& game) {}
    virtual void onPaddleCollision(Paddle& paddle, Game& game) {}
    
    bool isDestroyed() const { return destroyed; }

    virtual bool isBreakable() const { return false; }
};

#endif