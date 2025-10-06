#pragma once
#include <SFML/Graphics.hpp>

class Enemy {
public:
    Enemy(const sf::Color& color);
    
    void update(const sf::Vector2f& playerPosition);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    void setPosition(float x, float y);
    
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    int getHealth() const;
    
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float speed;
    int health;
    int maxHealth;
    float chaseRange;
    
    void checkWallCollisions();
}; 