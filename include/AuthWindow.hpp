#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include "User.hpp"
#include "Game.hpp"

using namespace std;

enum class WindowState {
    MAIN,
    LOGIN,
    REGISTER,
    MAIN_MENU,
    MAP_SELECTION,
    CHARACTER_SELECT,
    SETTINGS,
    EXIT_CONFIRMATION,
    HIGHSCORES,
    PAUSE
};

class AuthWindow {
public:
    AuthWindow();
    void run();
    void setState(WindowState state);
    
    // Static variables to store settings
    static inline bool isDarkTheme = true;
    static inline int currentTrack = 0;
    static inline float musicVolume = 0.0f;
    static inline float effectsVolume = 50.0f;
    static inline bool isMusicPlaying = false;
    
private:
    // State management
    WindowState currentState;
    string username;
    string password;
    bool isUsernameActive;
    bool isPasswordActive;
    int selectedMap;
    int selectedCharacter; // 0: Warrior, 1: Mage, 2: Archer
    
    sf::RenderWindow window;
    sf::Font font;
    
    // Background
    sf::RectangleShape background;
    sf::Texture backgroundTexture;
    sf::Texture lightBackgroundTexture;
    
    // Main window elements
    sf::RectangleShape loginButton;
    sf::RectangleShape registerButton;
    sf::RectangleShape exitButton;
    sf::Text loginText;
    sf::Text registerText;
    sf::Text exitText;
    sf::Text titleText;
    
    // Login/Register window elements
    sf::RectangleShape usernameBox;
    sf::RectangleShape passwordBox;
    sf::RectangleShape submitButton;
    sf::RectangleShape backButton;
    sf::RectangleShape showPasswordButton;
    sf::Text usernameText;
    sf::Text passwordText;
    sf::Text submitText;
    sf::Text backText;
    sf::Text showPasswordText;
    sf::Text inputText;
    sf::Text passwordInputText;
    sf::Text errorText;
    bool isPasswordVisible;
    
    // Main menu elements
    sf::RectangleShape characterButton;
    sf::RectangleShape characterSelectButton;
    sf::RectangleShape settingsButton;
    sf::RectangleShape recordsButton;
    sf::RectangleShape menuExitButton;
    sf::Text characterText;
    sf::Text characterSelectText;
    sf::Text settingsText;
    sf::Text recordsText;
    sf::Text menuExitText;
    
    // Map selection elements
    sf::RectangleShape mapSelectButton;
    sf::RectangleShape difficultySelectButton;
    sf::RectangleShape startGameButton;
    sf::RectangleShape mapBackButton;
    sf::Text mapSelectText;
    sf::Text difficultySelectText;
    sf::Text startGameText;
    sf::Text mapBackText;
    sf::Text mapTitleText;
    sf::Text difficultyTitleText;
    
    // Character select elements
    sf::RectangleShape redCubeButton;
    sf::RectangleShape greenCubeButton;
    sf::RectangleShape blueCubeButton;
    sf::Text redCubeText;
    sf::Text greenCubeText;
    sf::Text blueCubeText;
    sf::Text characterSelectTitle;
    sf::Text characterBackText;
    sf::RectangleShape characterBackButton;

    // Character textures and sprites
    sf::Texture mikeLeftTexture;
    sf::Texture jesyLeftTexture;
    sf::Texture gremLeftTexture;
    sf::Sprite mikeSprite;
    sf::Sprite jesySprite;
    sf::Sprite gremSprite;
    sf::Text mikeText;
    sf::Text jesyText;
    sf::Text gremText;
    
    // Character hover states
    bool isMikeHovered;
    bool isJesyHovered;
    bool isGremHovered;
    
    // Exit confirmation elements
    sf::RectangleShape confirmExitButton;
    sf::RectangleShape cancelExitButton;
    sf::Text confirmExitText;
    sf::Text cancelExitText;
    sf::Text exitConfirmationText;
    
    // Cursor elements
    sf::Text cursor;
    sf::Clock cursorBlinkClock;
    float cursorBlinkTime;
    bool showCursor;
    
    // Sound effects
    sf::SoundBuffer buttonClickBuffer;
    sf::Sound buttonClickSound;
    sf::SoundBuffer keyCollectBuffer;
    sf::Sound keyCollectSound;
    
    // Settings elements
    sf::RectangleShape musicSlider;
    sf::RectangleShape musicSliderHandle;
    sf::RectangleShape effectsSlider;
    sf::RectangleShape effectsSliderHandle;
    sf::RectangleShape prevTrackButton;
    sf::RectangleShape nextTrackButton;
    sf::RectangleShape themeColorButton;
    sf::RectangleShape settingsBackButton;
    sf::Text musicVolumeText;
    sf::Text effectsVolumeText;
    sf::Text currentTrackText;
    sf::Text themeColorText;
    sf::Text settingsBackText;
    sf::Text settingsTitleText;
    bool isDraggingMusicSlider;
    bool isDraggingEffectsSlider;
    
    // Highscores elements
    sf::RectangleShape highscoresBackButton;
    sf::Text highscoresBackText;
    sf::Text highscoresTitleText;
    sf::RectangleShape highscoresTable;
    sf::Text highscoresTableText;
    sf::RectangleShape clearRecordsButton;
    sf::Text clearRecordsText;
    
    // Music
    sf::Music musicTracks[4];
    
    // Methods
    void initializeUIElements();
    void handleEvents();
    void handleMainWindowEvents(const sf::Event& event);
    void handleLoginWindowEvents(const sf::Event& event);
    void handleRegisterWindowEvents(const sf::Event& event);
    void handleMainMenuEvents(const sf::Event& event);
    void handleMapSelectionEvents(const sf::Event& event);
    void handleCharacterSelectEvents(const sf::Event& event);
    void handleSettingsEvents(const sf::Event& event);
    void handleExitConfirmationEvents(const sf::Event& event);
    void handleHighscoresEvents(const sf::Event& event);
    void handleSubmit();
    void togglePasswordVisibility();
    void enterMainMenu();
    void draw();
    void drawMainWindow();
    void drawLoginWindow();
    void drawRegisterWindow();
    void drawMainMenu();
    void drawMapSelection();
    void drawCharacterSelect();
    void drawSettings();
    void drawExitConfirmation();
    void drawHighscores();
    void playButtonClickSound();
    void playKeyCollectSound();
    
    // Constants
    static constexpr float WINDOW_WIDTH = 1200.0f;
    static constexpr float WINDOW_HEIGHT = 800.0f;
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float MENU_BUTTON_WIDTH = 250.0f;
    static constexpr float BUTTON_SPACING = 20.0f;
}; 