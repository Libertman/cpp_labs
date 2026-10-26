#include "Ball.h"
#include "Config.h"
#include <cmath>

Ball::Ball(float x, float y, bool startStuck) : stuck(startStuck), stuckOffsetX(0.0f) {
    shape.setRadius(BALL_RADIUS);
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(BALL_RADIUS, BALL_RADIUS);
    shape.setPosition(x, y);
    velocity = sf::Vector2f(BALL_START_SPEED, -BALL_START_SPEED);
}

void Ball::update(float deltaTime, const sf::FloatRect& paddleBounds) {
    if (stuck) {
        shape.setPosition(paddleBounds.left + stuckOffsetX, paddleBounds.top - BALL_RADIUS);
        return;
    }

    shape.move(velocity * deltaTime);

    if (shape.getPosition().x - BALL_RADIUS < 0) {
        shape.setPosition(BALL_RADIUS, shape.getPosition().y);
        bounceX();
    }
    if (shape.getPosition().x + BALL_RADIUS > WINDOW_WIDTH) {
        shape.setPosition(WINDOW_WIDTH - BALL_RADIUS, shape.getPosition().y);
        bounceX();
    }
    if (shape.getPosition().y - BALL_RADIUS < 0) {
        shape.setPosition(shape.getPosition().x, BALL_RADIUS);
        bounceY();
    }
}

void Ball::stick(float paddleLeft) {
    stuck = true;
    stuckOffsetX = shape.getPosition().x - paddleLeft;
}

void Ball::launch() {
    if (stuck) {
        stuck = false;
        velocity = sf::Vector2f(BALL_START_SPEED, -BALL_START_SPEED);
    }
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Ball::bounceX() { velocity.x = -velocity.x; }
void Ball::bounceY() { velocity.y = -velocity.y; }

void Ball::multiplySpeed(float factor) { velocity *= factor; }
void Ball::setVelocity(sf::Vector2f vel) { velocity = vel; }

sf::FloatRect Ball::getBounds() const {
    return shape.getGlobalBounds();
}

sf::Vector2f Ball::getPosition() const {
    return shape.getPosition();
}

void Ball::resolveCollision(const sf::FloatRect& objectBounds) {
    sf::FloatRect ballBounds = getBounds();
    sf::FloatRect overlap;

    if (ballBounds.intersects(objectBounds, overlap)) {
        if (overlap.width < overlap.height) {
            bounceX();
            
            if (shape.getPosition().x < objectBounds.left + objectBounds.width / 2.0f) {
                shape.move(-overlap.width, 0);
            } else {
                shape.move(overlap.width, 0);
            }
        } else {
            bounceY();
            
            if (shape.getPosition().y < objectBounds.top + objectBounds.height / 2.0f) {
                shape.move(0, -overlap.height);
            } else {
                shape.move(0, overlap.height);
            }
        }
    }
}
