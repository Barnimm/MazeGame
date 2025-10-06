#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Registration {
public:
    Registration();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::Text usernameText;
    sf::Text passwordText;
    sf::Text registerButtonText;
    sf::Text backButtonText;
    
    sf::RectangleShape usernameBox;
    sf::RectangleShape passwordBox;
    sf::RectangleShape registerButton;
    sf::RectangleShape backButton;
    
    std::string username;
    std::string password;
    bool isUsernameSelected;
    bool isPasswordSelected;
    
    void processEvents();
    void update();
    void render();
    void handleTextInput(sf::Event& event);
    void handleButtonClick(const sf::Vector2f& mousePos);
}; 