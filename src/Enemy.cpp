#include "Enemy.hpp"
#include <iostream>

Enemy::Enemy(const sf::Color& color) : 
    velocity(0, 0), speed(2.0f), health(100), maxHealth(100), chaseRange(200.0f) {
    
    shape.setSize(sf::Vector2f(30, 30));
    shape.setFillColor(color);
    shape.setOrigin(15, 15);
}

void Enemy::update(const sf::Vector2f& playerPosition) {
    // Calculate direction to player
    sf::Vector2f direction = playerPosition - shape.getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Normalize direction
    if (distance > 0) {
        direction /= distance;
    }
    
    // Move towards player if in range
    if (distance < chaseRange) {
        velocity = direction * speed;
    } else {
        velocity = sf::Vector2f(0, 0);
    }
    
    // Update position
    shape.move(velocity);
    
    // Check wall collisions
    checkWallCollisions();
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Enemy::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

sf::Vector2f Enemy::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

int Enemy::getHealth() const {
    return health;
}

void Enemy::checkWallCollisions() {
    sf::FloatRect bounds = shape.getGlobalBounds();
    
    // Check screen boundaries
    if (bounds.left < 0) {
        shape.setPosition(bounds.width / 2, shape.getPosition().y);
        velocity.x = 0;
    }
    else if (bounds.left + bounds.width > 1280) {
        shape.setPosition(1280 - bounds.width / 2, shape.getPosition().y);
        velocity.x = 0;
    }
    
    if (bounds.top < 0) {
        shape.setPosition(shape.getPosition().x, bounds.height / 2);
        velocity.y = 0;
    }
    else if (bounds.top + bounds.height > 800) {
        shape.setPosition(shape.getPosition().x, 800 - bounds.height / 2);
        velocity.y = 0;
    }
} 