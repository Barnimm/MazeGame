#include "Player.hpp"
#include <iostream>

Player::Player(const sf::Color& color) : 
    velocity(0, 0), speed(100.0f), jumpForce(5.0f), gravity(0.2f), friction(0.7f),
    isJumping(false), health(100), maxHealth(100), invincibilityTime(0), isInvincible(false),
    animationTime(0), currentFrame(0), isMoving(false) {
    
    shape.setSize(sf::Vector2f(15, 15));
    shape.setFillColor(color);
    shape.setOrigin(7.5f, 7.5f);
}

void Player::update() {
    // Apply gravity
    velocity.y += gravity;
    
    // Apply friction
    velocity.x *= friction;
    
    // Update position
    shape.move(velocity);
    
    // Check wall collisions
    checkWallCollisions();
}

void Player::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space && !isJumping) {
            velocity.y = -jumpForce;
            isJumping = true;
        }
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -speed;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = speed;
        isMoving = true;
    }
    else {
        isMoving = false;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Player::takeDamage(int damage) {
    if (!isInvincible) {
        health -= damage;
        if (health < 0) health = 0;
        isInvincible = true;
        invincibilityTime = 1.0f;
    }
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

int Player::getHealth() const {
    return health;
}

void Player::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

void Player::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

sf::FloatRect Player::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

void Player::checkWallCollisions() {
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
        isJumping = false;
    }
}

void Player::updateAnimation() {
    if (isMoving) {
        animationTime += 0.016f;
        if (animationTime >= 0.1f) {
            animationTime = 0;
            currentFrame = (currentFrame + 1) % 4;
            
            int frameWidth = shape.getSize().x / 4;
            shape.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, 
                                            frameWidth, shape.getSize().y));
        }
    } else {
        currentFrame = 0;
        int frameWidth = shape.getSize().x / 4;
        shape.setTextureRect(sf::IntRect(0, 0, frameWidth, shape.getSize().y));
    }
}

void Player::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}

void Player::setFillColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Player::move(float x, float y) {
    shape.move(x, y);
}

void Player::move(const sf::Vector2f& offset) {
    shape.move(offset);
} 
