#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class MainMenu {
public:
    MainMenu();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::Text playButtonText;
    sf::Text settingsButtonText;
    sf::Text exitButtonText;
    
    sf::RectangleShape playButton;
    sf::RectangleShape settingsButton;
    sf::RectangleShape exitButton;
    
    void processEvents();
    void update();
    void render();
    void handleButtonClick(const sf::Vector2f& mousePos);
}; 