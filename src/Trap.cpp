#include "Trap.hpp"
#include <iostream>

Trap::Trap(const sf::Texture& texture, int damageAmount) :
    active(false), damage(damageAmount), animationTime(0), currentFrame(0) {
    
    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2); // Set origin to center
    
    // Initial calculation of collision bounds (can be refined later)
    // Assuming the visible part of the monster is a central portion of the texture
    float monsterWidth = sprite.getLocalBounds().width * 0.017f; // Further reduce factor
    float monsterHeight = sprite.getLocalBounds().height * 0.01f; // Further reduce factor
    
    collisionBounds.width = monsterWidth;
    collisionBounds.height = monsterHeight;
}  

void Trap::update() {
    // Traps are static for now, no animation/movement needed.
    // If any trap-specific animation is added later, it will go here.
}

void Trap::draw(sf::RenderWindow& window) const {
    // Draw trap regardless of 'active' status, active is for dealing damage
    window.draw(sprite);
}

void Trap::activate() {
    active = true;
}

void Trap::deactivate() {
    active = false;
}

void Trap::setPosition(float x, float y) {
    sprite.setPosition(x, y);
    
    // Update collision bounds position relative to sprite position
    // Center the collision bounds around the sprite's origin
    collisionBounds.left = sprite.getPosition().x - collisionBounds.width / 2;
    collisionBounds.top = sprite.getPosition().y - collisionBounds.height / 2;
}

bool Trap::isActive() const {
    return active;
}

sf::FloatRect Trap::getGlobalBounds() const {
    return collisionBounds; // Return the custom collision bounds
}

int Trap::getDamage() const {
    return damage;
}

// Removed updateAnimation function as it's no longer used for size pulsing
/*
void Trap::updateAnimation() {
    animationTime += 0.016f;
    if (animationTime >= 0.1f) {
        animationTime = 0;
        currentFrame = (currentFrame + 1) % 4;
        
        float size = 30.0f * (1.0f + 0.1f * std::sin(animationTime * 10.0f));
        shape.setSize(sf::Vector2f(size, size));
        shape.setOrigin(size/2, size/2);
    }
}
*/ 