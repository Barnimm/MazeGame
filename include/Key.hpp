#pragma once
#include <SFML/Graphics.hpp>

class Key {
public:
    Key(const sf::Texture& texture);
    
    void update();
    void draw(sf::RenderWindow& window) const;
    void collect();
    void setPosition(float x, float y);
    
    bool isCollected() const;
    sf::FloatRect getGlobalBounds() const;
    
    // Make sprite public for direct access from Game class
    sf::Sprite sprite;
    
private:
    bool collected;
    float rotation;
    float animationTime;
    int currentFrame;
    sf::Vector2f initialPosition;
    
    void updateAnimation();
}; 