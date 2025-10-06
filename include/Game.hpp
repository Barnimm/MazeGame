#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include "Key.hpp"
#include "Trap.hpp"

enum class Difficulty { EASY, MEDIUM, HARD };

class Game {
public:
    Game(Difficulty difficulty, int mapNumber = 1, const std::string& username = "", int selectedCharacter = 0);
    void run();
     static const int CELL_SIZE = 100;
    std::vector<std::vector<int>> maze;
private:
    sf::RenderWindow window;
    sf::View camera;
    sf::RectangleShape player;
    sf::RectangleShape finish;
    std::vector<sf::RectangleShape> walls;
    std::vector<Trap> traps;
    std::vector<Key> keys;
    int collectedKeys;
    bool doorOpen;
    std::string username;
    
    // Player movement
    sf::Vector2f playerVelocity;
    float playerSpeed;
    
    // Sound effects
    sf::Music backgroundMusic;
    sf::Sound keySound;
    sf::Sound doorSound;
    sf::Sound finishSound;
    sf::Sound damageSound;
    sf::SoundBuffer keyBuffer;
    sf::SoundBuffer doorBuffer;
    sf::SoundBuffer finishBuffer;
    sf::SoundBuffer damageBuffer;
    
    // Floor texture and sprites
    sf::Texture floorTexture;
    std::vector<sf::Sprite> floorTiles;
    
    // Wall texture
    sf::Texture wallTexture;
    
    // Door textures
    sf::Texture closedDoorTexture;
    sf::Texture openDoorTexture;
    sf::Sprite doorSprite;
    
    // Key texture
    sf::Texture keyTexture;
    
    // Trap texture
    sf::Texture trapTexture;
    
    // Health bar elements
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBar;
    sf::Font font;
    sf::Text healthText;
    
    // Timer elements
    sf::Clock gameClock;
    sf::Text timerText;
    
    // Damage flash effect
    sf::RectangleShape damageFlash;
    float damageFlashTime;
    bool isFlashing;
    
    // Game state
    int playerHealth;
    float invincibilityTime;
    bool isInvincible;

    int MAZE_WIDTH;
    int MAZE_HEIGHT;
    Difficulty gameDifficulty;
    int currentMap;
    int initialSelectedCharacter;

    // Pause menu elements
    sf::RectangleShape pauseMenu;
    sf::RectangleShape resumeButton;
    sf::RectangleShape restartButton;
    sf::RectangleShape mainMenuButton;
    sf::RectangleShape musicSlider;
    sf::RectangleShape musicSliderHandle;
    sf::RectangleShape effectsSlider;
    sf::RectangleShape effectsSliderHandle;
    sf::Text pauseTitleText;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text mainMenuText;
    sf::Text musicVolumeText;
    sf::Text effectsVolumeText;
    bool isPaused;
    bool isDraggingMusicSlider;
    bool isDraggingEffectsSlider;
    float musicVolume;
    float effectsVolume;

    // Game over window elements
    bool showGameOver;
    sf::RectangleShape gameOverWindow;
    sf::Text gameOverTitle;
    sf::Text gameOverTime;
    sf::RectangleShape restartGameButton;
    sf::Text restartGameText;
    sf::RectangleShape gameOverMainMenuButton;
    sf::Text gameOverMainMenuText;

    // Player texture
    sf::Texture playerTexture;
    sf::Sprite playerSprite;

    void processEvents();
    void update();
    void render();
    void draw();
    void loadMaze();
    void handlePlayerInput();
    bool checkCollision(const sf::FloatRect& rect);
    void updateCamera();
    bool isAtFinish() const;
    void updateHealthBar();
    void placeTraps();
    void checkTrapCollisions();
    void updateDamageFlash();
    void placeKeys();
    void checkKeyCollection();
    bool canOpenDoor() const;
    void initializePauseMenu();
    void handlePauseEvents(const sf::Event& event);
    void drawPauseMenu();
    void togglePause();
    void initializeGameOverWindow();
    void drawGameOverWindow();
    void handleGameOverEvents(const sf::Event& event);
}; 