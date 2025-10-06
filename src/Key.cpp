#include "Key.hpp"
#include <iostream>

Key::Key(const sf::Texture& texture) :
    collected(false), rotation(0), animationTime(0), currentFrame(0), initialPosition(0, 0) {
    
    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Key::update() {
    if (!collected) {
        // Rotate the key
        // rotation += 2.0f; // Increased rotation speed slightly for better visibility
        // sprite.setRotation(rotation);
        
        // Add floating animation
        animationTime += 0.07f; // Adjust speed of floating
        float floatingHeight = 10.0f; // Adjust height of floating
        float yOffset = std::sin(animationTime) * floatingHeight;
        
        // Set sprite position relative to initial position with offset
        sprite.setPosition(initialPosition.x, initialPosition.y + yOffset);
    }
}

void Key::draw(sf::RenderWindow& window) const {
    if (!collected) {
        window.draw(sprite);
    }
}

void Key::collect() {
    collected = true;
}

void Key::setPosition(float x, float y) {
    initialPosition.x = x; // Store initial position
    initialPosition.y = y;
    sprite.setPosition(initialPosition); // Set initial sprite position
}

bool Key::isCollected() const {
    return collected;
}

sf::FloatRect Key::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Key::updateAnimation() {
    // This function is now only used for rotation, size animation removed.
    // The rotation logic is moved to the update() method.
    // Keeping the function structure for potential future use or if needed for other animations.
} 