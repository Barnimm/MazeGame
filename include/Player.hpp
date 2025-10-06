#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player(const sf::Color& color = sf::Color::Cyan);
    void update();
    void handleInput(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    void heal(int amount);
    sf::Vector2f getPosition() const;
    int getHealth() const;
    void setPosition(float x, float y);
    void setColor(const sf::Color& color);
    sf::FloatRect getGlobalBounds() const;
    void setSize(const sf::Vector2f& size);
    void setFillColor(const sf::Color& color);
    void move(float x, float y);
    void move(const sf::Vector2f& offset);

private:
    void checkWallCollisions();
    void updateAnimation();

    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float speed;
    float jumpForce;
    float gravity;
    float friction;
    bool isJumping;
    int health;
    int maxHealth;
    float invincibilityTime;
    bool isInvincible;
    float animationTime;
    int currentFrame;
    bool isMoving;
}; 