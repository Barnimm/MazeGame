#pragma once
#include <SFML/Graphics.hpp>

class Trap {
public:
    Trap(const sf::Texture& texture, int damageAmount);
    
    void update();
    void draw(sf::RenderWindow& window) const;
    void activate();
    void deactivate();
    void setPosition(float x, float y);
    
    bool isActive() const;
    sf::FloatRect getGlobalBounds() const;
    int getDamage() const;
    
    sf::Sprite sprite;
    
private:
    bool active;
    int damage;
    float animationTime;
    int currentFrame;
    sf::FloatRect collisionBounds;
}; 