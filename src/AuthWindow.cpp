#include "AuthWindow.hpp"
#include "User.hpp"
#include "Game.hpp"
#include "Records.hpp"
#include <iostream>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

AuthWindow::AuthWindow() : window(sf::VideoMode::getFullscreenModes()[0], "Authorization", sf::Style::Fullscreen),
    currentState(WindowState::MAIN), isUsernameActive(false), isPasswordActive(false), isPasswordVisible(false),
    cursorBlinkTime(0), showCursor(true), selectedMap(1),
    isDraggingMusicSlider(false), isDraggingEffectsSlider(false), selectedCharacter(0) {
    
    try {
        // Initialize text rendering settings
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        window.create(sf::VideoMode::getFullscreenModes()[0], "Authorization", sf::Style::Fullscreen, settings);
        
        window.setFramerateLimit(60);
        
        // Load font with error handling
        string fontPath = "/System/Library/Fonts/Supplemental/Arial Black.ttf";
        if (!fs::exists(fontPath)) {
            fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
            if (!fs::exists(fontPath)) {
                throw runtime_error("Could not find DejaVu Sans font in standard locations");
            }
        }
        
        if (!font.loadFromFile(fontPath)) {
            throw runtime_error("Failed to load font file");
        }
        
        // Debug information
        std::cout << "Font loaded successfully from: " << fontPath << std::endl;
        std::cout << "Font info - Line spacing: " << font.getLineSpacing(30) << std::endl;
        std::cout << "Font info - Underline position: " << font.getUnderlinePosition(30) << std::endl;
        std::cout << "Font info - Underline thickness: " << font.getUnderlineThickness(30) << std::endl;
        
        // Test text rendering
        sf::Text testText;
        testText.setFont(font);
        testText.setString("Тест русского текста");
        testText.setCharacterSize(30);
        testText.setFillColor(sf::Color::White);
        std::cout << "Test text bounds - Width: " << testText.getGlobalBounds().width 
                  << ", Height: " << testText.getGlobalBounds().height << std::endl;
        
        // Load music tracks
        if (!musicTracks[0].openFromFile("/Users/misa/Desktop/курсовая основа/sounds/ganggang.ogg")) {
             cerr << "Failed to load music track 1!" << endl;
        } else { cout << "Successfully loaded music track 1." << endl; }
        if (!musicTracks[1].openFromFile("/Users/misa/Desktop/курсовая основа/sounds/Lil-Pump-Esskeetit.ogg")) {
             cerr << "Failed to load music track 2!" << endl;
        } else { cout << "Successfully loaded music track 2." << endl; }
        if (!musicTracks[2].openFromFile("/Users/misa/Desktop/курсовая основа/sounds/zeze.ogg")) {
             cerr << "Failed to load music track 3!" << endl;
        } else { cout << "Successfully loaded music track 3." << endl; }
        if (!musicTracks[3].openFromFile("/Users/misa/Desktop/курсовая основа/sounds/makanishe.ogg")) {
             cerr << "Failed to load music track 4!" << endl;
        } else { cout << "Successfully loaded music track 4." << endl; }

        // Set initial volume and looping for all tracks
        for (int i = 0; i < 4; ++i) {
            musicTracks[i].setVolume(musicVolume);
            musicTracks[i].setLoop(true);
        }

        // Восстанавливаем состояние музыки
        if (currentTrack >= 0 && currentTrack < 4) {
            // Останавливаем все треки
            for (int i = 0; i < 4; ++i) {
                musicTracks[i].stop();
            }
            // Устанавливаем громкость и запускаем текущий трек
            musicTracks[currentTrack].setVolume(musicVolume);
            if (isMusicPlaying) {
                musicTracks[currentTrack].play();
            }
        }
        
        // Get screen dimensions and calculate scaling
        sf::Vector2u screenSize = window.getSize();
        float scaleX = screenSize.x / static_cast<float>(WINDOW_WIDTH);
        float scaleY = screenSize.y / static_cast<float>(WINDOW_HEIGHT);
        
        // Setup background
        // Load background textures
        if (!backgroundTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/backgroung/back1.png")) {
            std::cerr << "Failed to load dark background texture for AuthWindow" << std::endl;
        }
        if (!lightBackgroundTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/backgroung/back2.png")) {
            std::cerr << "Failed to load light background texture for AuthWindow" << std::endl;
        }
        background.setSize(sf::Vector2f(screenSize.x, screenSize.y));
        background.setTexture(isDarkTheme ? &backgroundTexture : &lightBackgroundTexture);
        
        // Setup cursor
        cursor.setFont(font);
        cursor.setString("|");
        cursor.setCharacterSize(30 * scaleY);
        cursor.setFillColor(sf::Color::Black);
        cursor.setPosition(0, 0);
        
        // Initialize UI elements
        initializeUIElements();
        
        // Load button click sound
        if (!buttonClickBuffer.loadFromFile("/Users/misa/Desktop/курсовая основа/sounds effects/buttonSound.ogg")) {
            cerr << "Failed to load button click sound!" << endl;
        } else {
            cout << "Successfully loaded button click sound." << endl;
            buttonClickSound.setBuffer(buttonClickBuffer);
            buttonClickSound.setVolume(effectsVolume);
        }

        // Load key collect sound
        if (!keyCollectBuffer.loadFromFile("/Users/misa/Desktop/курсовая основа/sounds effects/sborKlucha.ogg")) {
            cerr << "Failed to load key collect sound!" << endl;
        } else {
            cout << "Successfully loaded key collect sound." << endl;
            keyCollectSound.setBuffer(keyCollectBuffer);
            keyCollectSound.setVolume(effectsVolume);
        }
        
        // Set text rendering quality
        for (auto& text : {&loginText, &registerText, &exitText, &characterText, &characterSelectText, 
                          &settingsText, &recordsText, &menuExitText, &settingsTitleText, 
                          &musicVolumeText, &effectsVolumeText, &currentTrackText, &themeColorText, 
                          &settingsBackText, &mapTitleText, &mapSelectText, &difficultyTitleText, 
                          &difficultySelectText, &startGameText, &mapBackText, &redCubeText, 
                          &greenCubeText, &blueCubeText, &usernameText, &passwordText, &submitText, 
                          &backText, &showPasswordText, &errorText, &highscoresBackText, 
                          &highscoresTitleText, &highscoresTableText}) {
            text->setStyle(sf::Text::Regular);
            text->setOutlineThickness(0);
        }
        
        // Load character textures
        if (!mikeLeftTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/cheracter/bear.png")) {
            cerr << "Failed to load Mike texture!" << endl;
        }
        if (!jesyLeftTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/cheracter/dino.png")) {
            cerr << "Failed to load Jesy texture!" << endl;
        }
        if (!gremLeftTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/cheracter/cat.png")) {
            cerr << "Failed to load Grem texture!" << endl;
        }

        // Setup character sprites
        mikeSprite.setTexture(mikeLeftTexture);
        mikeSprite.setOrigin(mikeLeftTexture.getSize().x / 2.0f, mikeLeftTexture.getSize().y / 2.0f);

        jesySprite.setTexture(jesyLeftTexture);
        jesySprite.setOrigin(jesyLeftTexture.getSize().x / 2.0f, jesyLeftTexture.getSize().y / 2.0f);

        gremSprite.setTexture(gremLeftTexture);
        gremSprite.setOrigin(gremLeftTexture.getSize().x / 2.0f, gremLeftTexture.getSize().y / 2.0f);

        // Scale sprites to appropriate size
        float spriteScale = 0.8f;
        mikeSprite.setScale(spriteScale, spriteScale);
        jesySprite.setScale(spriteScale, spriteScale);
        gremSprite.setScale(spriteScale, spriteScale);

        // Initialize character text
        mikeText.setFont(font);
        mikeText.setString(L"Медведь Мишаня");
        mikeText.setCharacterSize(24);
        mikeText.setFillColor(sf::Color::White);
        mikeText.setOrigin(mikeText.getLocalBounds().width / 2.0f, mikeText.getLocalBounds().height / 2.0f);

        jesyText.setFont(font);
        jesyText.setString(L"Динозавр Смола");
        jesyText.setCharacterSize(24);
        jesyText.setFillColor(sf::Color::White);
        jesyText.setOrigin(jesyText.getLocalBounds().width / 2.0f, jesyText.getLocalBounds().height / 2.0f);

        gremText.setFont(font);
        gremText.setString(L"Кот Зачет");
        gremText.setCharacterSize(24);
        gremText.setFillColor(sf::Color::White);
        gremText.setOrigin(gremText.getLocalBounds().width / 2.0f, gremText.getLocalBounds().height / 2.0f);

        // Initialize hover states
        isMikeHovered = false;
        isJesyHovered = false;
        isGremHovered = false;

        // Initialize highlight rectangle
        // Removed selectedCharacterHighlight initialization
        
    } catch (const exception& e) {
        cerr << "Error initializing AuthWindow: " << e.what() << endl;
        throw;
    }
}

void AuthWindow::initializeUIElements() {
    sf::Vector2u screenSize = window.getSize();
    float scaleX = screenSize.x / static_cast<float>(WINDOW_WIDTH);
    float scaleY = screenSize.y / static_cast<float>(WINDOW_HEIGHT);

    try {
        // Debug font information
        std::cout << "Initializing UI elements with font: " << (font.getInfo().family) << std::endl;
        
        float centerY = screenSize.y / 2.f;
        float centerX = screenSize.x / 2.f;
        float fieldWidth = 400 * scaleX;
        float fieldHeight = 50 * scaleY;
        float labelOffsetX = 160 * scaleX;
        float spacingY = 30 * scaleY;
        float buttonWidth = 300 * scaleX;
        float buttonHeight = 60 * scaleY;
        float menuButtonWidth = 450 * scaleX;
        float menuButtonHeight = 60 * scaleY;
        float menuSpacingY = 40 * scaleY;

        // --- MAIN WINDOW ---
        float mainStartY = centerY - (buttonHeight * 3 + menuSpacingY * 2) / 2;
        loginButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        loginButton.setPosition(centerX - buttonWidth/2, mainStartY);
        loginButton.setFillColor(sf::Color(70, 130, 180));
        loginButton.setOutlineThickness(3);
        loginButton.setOutlineColor(sf::Color::White);
        loginText.setFont(font);
        loginText.setString(L"Войти");
        loginText.setCharacterSize(36 * scaleY);
        loginText.setFillColor(sf::Color::White);
        
        // Debug login te
        
        loginText.setPosition(
            loginButton.getPosition().x + buttonWidth/2 - loginText.getGlobalBounds().width/2,
            loginButton.getPosition().y + buttonHeight/2 - loginText.getGlobalBounds().height/2
        );
        registerButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        registerButton.setPosition(centerX - buttonWidth/2, mainStartY + buttonHeight + menuSpacingY);
        registerButton.setFillColor(sf::Color(70, 130, 180));
        registerButton.setOutlineThickness(3);
        registerButton.setOutlineColor(sf::Color::White);
        registerText.setFont(font);
        registerText.setString(L"Регистрация");
        registerText.setCharacterSize(36 * scaleY);
        registerText.setFillColor(sf::Color::White);
        registerText.setPosition(
            registerButton.getPosition().x + buttonWidth/2 - registerText.getGlobalBounds().width/2,
            registerButton.getPosition().y + buttonHeight/2 - registerText.getGlobalBounds().height/2
        );
        exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        exitButton.setPosition(centerX - buttonWidth/2, mainStartY + (buttonHeight + menuSpacingY) * 2);
        exitButton.setFillColor(sf::Color(180, 70, 70));
        exitButton.setOutlineThickness(3);
        exitButton.setOutlineColor(sf::Color::White);
        exitText.setFont(font);
        exitText.setString(L"выход");
        exitText.setCharacterSize(36 * scaleY);
        exitText.setFillColor(sf::Color::White);
        exitText.setPosition(
            exitButton.getPosition().x + buttonWidth/2 - exitText.getGlobalBounds().width/2,
            exitButton.getPosition().y + buttonHeight/2 - exitText.getGlobalBounds().height/2
        );

        // --- MAIN MENU ---
        float menuStartY = centerY - (menuButtonHeight * 5 + menuSpacingY * 4) / 2;  // Adjusted for 5 buttons
        characterButton.setSize(sf::Vector2f(menuButtonWidth, menuButtonHeight));
        characterButton.setPosition(centerX - menuButtonWidth/2, menuStartY);
        characterButton.setFillColor(sf::Color(70, 130, 180));
        characterButton.setOutlineThickness(3);
        characterButton.setOutlineColor(sf::Color::White);
        characterText.setFont(font);
        characterText.setString(L"Играть");
        characterText.setCharacterSize(36 * scaleY);
        characterText.setFillColor(sf::Color::White);
        characterText.setPosition(
            characterButton.getPosition().x + menuButtonWidth/2 - characterText.getGlobalBounds().width/2,
            characterButton.getPosition().y + menuButtonHeight/2 - characterText.getGlobalBounds().height/2
        );

        // Add character selection button
        characterSelectButton.setSize(sf::Vector2f(menuButtonWidth, menuButtonHeight));
        characterSelectButton.setPosition(centerX - menuButtonWidth/2, menuStartY + menuButtonHeight + menuSpacingY);
        characterSelectButton.setFillColor(sf::Color(70, 130, 180));
        characterSelectButton.setOutlineThickness(3);
        characterSelectButton.setOutlineColor(sf::Color::White);
        characterSelectText.setFont(font);
        characterSelectText.setString(L"Выбрать персонажа");
        characterSelectText.setCharacterSize(36 * scaleY);
        characterSelectText.setFillColor(sf::Color::White);
        characterSelectText.setPosition(
            characterSelectButton.getPosition().x + menuButtonWidth/2 - characterSelectText.getGlobalBounds().width/2,
            characterSelectButton.getPosition().y + menuButtonHeight/2 - characterSelectText.getGlobalBounds().height/2
        );

        // Add settings button
        settingsButton.setSize(sf::Vector2f(menuButtonWidth, menuButtonHeight));
        settingsButton.setPosition(centerX - menuButtonWidth/2, menuStartY + (menuButtonHeight + menuSpacingY) * 2);
        settingsButton.setFillColor(sf::Color(70, 130, 180));
        settingsButton.setOutlineThickness(3);
        settingsButton.setOutlineColor(sf::Color::White);
        settingsText.setFont(font);
        settingsText.setString(L"Настройки");
        settingsText.setCharacterSize(36 * scaleY);
        settingsText.setFillColor(sf::Color::White);
        settingsText.setPosition(
            settingsButton.getPosition().x + menuButtonWidth/2 - settingsText.getGlobalBounds().width/2,
            settingsButton.getPosition().y + menuButtonHeight/2 - settingsText.getGlobalBounds().height/2
        );

        // Add records button
        recordsButton.setSize(sf::Vector2f(menuButtonWidth, menuButtonHeight));
        recordsButton.setPosition(centerX - menuButtonWidth/2, menuStartY + (menuButtonHeight + menuSpacingY) * 3);
        recordsButton.setFillColor(sf::Color(70, 130, 180));
        recordsButton.setOutlineThickness(3);
        recordsButton.setOutlineColor(sf::Color::White);
        recordsText.setFont(font);
        recordsText.setString(L"Таблица лидеров");
        recordsText.setCharacterSize(36 * scaleY);
        recordsText.setFillColor(sf::Color::White);
        recordsText.setPosition(
            recordsButton.getPosition().x + menuButtonWidth/2 - recordsText.getGlobalBounds().width/2,
            recordsButton.getPosition().y + menuButtonHeight/2 - recordsText.getGlobalBounds().height/2
        );

        // Add exit button
        menuExitButton.setSize(sf::Vector2f(menuButtonWidth, menuButtonHeight));
        menuExitButton.setPosition(centerX - menuButtonWidth/2, menuStartY + (menuButtonHeight + menuSpacingY) * 4);
        menuExitButton.setFillColor(sf::Color(180, 70, 70));
        menuExitButton.setOutlineThickness(3);
        menuExitButton.setOutlineColor(sf::Color::White);
        menuExitText.setFont(font);
        menuExitText.setString(L"Выход");
        menuExitText.setCharacterSize(36 * scaleY);
        menuExitText.setFillColor(sf::Color::White);
        menuExitText.setPosition(
            menuExitButton.getPosition().x + menuButtonWidth/2 - menuExitText.getGlobalBounds().width/2,
            menuExitButton.getPosition().y + menuButtonHeight/2 - menuExitText.getGlobalBounds().height/2
        );

        // Initialize settings menu elements
        float sliderWidth = 400 * scaleX;
        float sliderHeight = 20 * scaleY;
        float handleSize = 30 * scaleY;
        float settingsStartY = centerY - 200 * scaleY;

        // Settings title
        settingsTitleText.setFont(font);
        settingsTitleText.setString(L"Настройки");
        settingsTitleText.setCharacterSize(48 * scaleY);
        settingsTitleText.setFillColor(sf::Color::White);
        settingsTitleText.setPosition(centerX - settingsTitleText.getGlobalBounds().width/2, settingsStartY);

        // Music volume slider
        musicVolumeText.setFont(font);
        musicVolumeText.setString(L"Громкость музыки");
        musicVolumeText.setCharacterSize(30 * scaleY);
        musicVolumeText.setFillColor(sf::Color::White);
        musicVolumeText.setPosition(centerX - sliderWidth/2, settingsStartY + 80 * scaleY);

        musicSlider.setSize(sf::Vector2f(sliderWidth, sliderHeight));
        musicSlider.setPosition(centerX - sliderWidth/2, settingsStartY + 120 * scaleY);
        musicSlider.setFillColor(sf::Color(100, 100, 100));

        musicSliderHandle.setSize(sf::Vector2f(handleSize, handleSize));
        musicSliderHandle.setPosition(
            musicSlider.getPosition().x + (musicVolume / 100.0f) * (sliderWidth - handleSize),
            musicSlider.getPosition().y - (handleSize - sliderHeight) / 2
        );
        musicSliderHandle.setFillColor(sf::Color::White);

        // Effects volume slider
        effectsVolumeText.setFont(font);
        effectsVolumeText.setString(L"Громкость эффектов");
        effectsVolumeText.setCharacterSize(30 * scaleY);
        effectsVolumeText.setFillColor(sf::Color::White);
        effectsVolumeText.setPosition(centerX - sliderWidth/2, settingsStartY + 200 * scaleY);

        effectsSlider.setSize(sf::Vector2f(sliderWidth, sliderHeight));
        effectsSlider.setPosition(centerX - sliderWidth/2, settingsStartY + 240 * scaleY);
        effectsSlider.setFillColor(sf::Color(100, 100, 100));

        effectsSliderHandle.setSize(sf::Vector2f(handleSize, handleSize));
        effectsSliderHandle.setPosition(
            effectsSlider.getPosition().x + (effectsVolume / 100.0f) * (sliderWidth - handleSize),
            effectsSlider.getPosition().y - (handleSize - sliderHeight) / 2
        );
        effectsSliderHandle.setFillColor(sf::Color::White);

        // Track selection buttons
        float trackButtonWidth = 60 * scaleX;
        float trackButtonHeight = 60 * scaleY;
        float trackButtonSpacing = 20 * scaleX;

        prevTrackButton.setSize(sf::Vector2f(trackButtonWidth, trackButtonHeight));
        prevTrackButton.setPosition(centerX - trackButtonWidth - trackButtonSpacing - 150 * scaleX, settingsStartY + 280 * scaleY);
        prevTrackButton.setFillColor(sf::Color(70, 130, 180));
        prevTrackButton.setOutlineThickness(3);
        prevTrackButton.setOutlineColor(sf::Color::White);

        nextTrackButton.setSize(sf::Vector2f(trackButtonWidth, trackButtonHeight));
        nextTrackButton.setPosition(centerX + 150 * scaleX + trackButtonSpacing, settingsStartY + 280 * scaleY);
        nextTrackButton.setFillColor(sf::Color(70, 130, 180));
        nextTrackButton.setOutlineThickness(3);
        nextTrackButton.setOutlineColor(sf::Color::White);

        // Current track text
        currentTrackText.setFont(font);
        currentTrackText.setString(L"Трек " + std::to_wstring(currentTrack + 1));
        currentTrackText.setCharacterSize(30 * scaleY);
        currentTrackText.setFillColor(sf::Color::White);
        currentTrackText.setPosition(
            centerX - currentTrackText.getGlobalBounds().width/2,
            settingsStartY + 280 * scaleY + trackButtonHeight/2 - currentTrackText.getGlobalBounds().height/2
        );

        // Theme color button
        themeColorButton.setSize(sf::Vector2f(300 * scaleX, 60 * scaleY));
        themeColorButton.setPosition(centerX - 150 * scaleX, settingsStartY + 400 * scaleY);
        themeColorButton.setFillColor(sf::Color(70, 130, 180));
        themeColorButton.setOutlineThickness(3);
        themeColorButton.setOutlineColor(sf::Color::White);

        themeColorText.setFont(font);
        themeColorText.setString(L"Тёмная");  // Initial text for dark theme
        themeColorText.setCharacterSize(30 * scaleY);
        themeColorText.setFillColor(sf::Color::White);
        themeColorText.setPosition(
            themeColorButton.getPosition().x + themeColorButton.getSize().x/2 - themeColorText.getGlobalBounds().width/2,
            themeColorButton.getPosition().y + themeColorButton.getSize().y/2 - themeColorText.getGlobalBounds().height/2
        );

        // Back button
        settingsBackButton.setSize(sf::Vector2f(300 * scaleX, 60 * scaleY));
        settingsBackButton.setPosition(centerX - 150 * scaleX, settingsStartY + 480 * scaleY);
        settingsBackButton.setFillColor(sf::Color(180, 70, 70));
        settingsBackButton.setOutlineThickness(3);
        settingsBackButton.setOutlineColor(sf::Color::White);

        settingsBackText.setFont(font);
        settingsBackText.setString(L"Вернуться в меню");
        settingsBackText.setCharacterSize(30 * scaleY);
        settingsBackText.setFillColor(sf::Color::White);
        settingsBackText.setPosition(
            settingsBackButton.getPosition().x + settingsBackButton.getSize().x/2 - settingsBackText.getGlobalBounds().width/2,
            settingsBackButton.getPosition().y + settingsBackButton.getSize().y/2 - settingsBackText.getGlobalBounds().height/2
        );

        // --- MAP SELECTION ---
        mapTitleText.setFont(font);
        mapTitleText.setString(L"Выбор карты");
        mapTitleText.setCharacterSize(48 * scaleY);
        mapTitleText.setFillColor(sf::Color::White);
        mapTitleText.setPosition(centerX - mapTitleText.getGlobalBounds().width/2, centerY - 200 * scaleY);
        mapSelectButton.setSize(sf::Vector2f(400 * scaleX, 60 * scaleY));
        mapSelectButton.setPosition(centerX - 200 * scaleX, centerY - 50 * scaleY);
        mapSelectButton.setFillColor(sf::Color(70, 130, 180));
        mapSelectText.setFont(font);
        mapSelectText.setString(L"Карта 1");
        mapSelectText.setCharacterSize(30 * scaleY);
        mapSelectText.setFillColor(sf::Color::White);
        mapSelectText.setPosition(
            mapSelectButton.getPosition().x + 200 * scaleX - mapSelectText.getGlobalBounds().width/2,
            mapSelectButton.getPosition().y + 30 * scaleY - mapSelectText.getGlobalBounds().height/2
        );
        difficultyTitleText.setFont(font);
        difficultyTitleText.setString(L"Выбор сложности");
        difficultyTitleText.setCharacterSize(48 * scaleY);
        difficultyTitleText.setFillColor(sf::Color::White);
        difficultyTitleText.setPosition(
            centerX - difficultyTitleText.getGlobalBounds().width/2,
            centerY + 20 * scaleY
        );
        difficultySelectButton.setSize(sf::Vector2f(400 * scaleX, 60 * scaleY));
        difficultySelectButton.setPosition(centerX - 200 * scaleX, centerY + 100 * scaleY);
        difficultySelectButton.setFillColor(sf::Color(70, 130, 180));
        difficultySelectText.setFont(font);
        difficultySelectText.setString(L"Легкая");
        difficultySelectText.setCharacterSize(30 * scaleY);
        difficultySelectText.setFillColor(sf::Color::White);
        difficultySelectText.setPosition(
            difficultySelectButton.getPosition().x + 200 * scaleX - difficultySelectText.getGlobalBounds().width/2,
            difficultySelectButton.getPosition().y + 30 * scaleY - difficultySelectText.getGlobalBounds().height/2
        );
        startGameButton.setSize(sf::Vector2f(400 * scaleX, 60 * scaleY));
        startGameButton.setPosition(centerX - 200 * scaleX, centerY + 200 * scaleY);
        startGameButton.setFillColor(sf::Color(70, 180, 70));
        startGameText.setFont(font);
        startGameText.setString(L"Начать игру");
        startGameText.setCharacterSize(30 * scaleY);
        startGameText.setFillColor(sf::Color::White);
        startGameText.setPosition(
            startGameButton.getPosition().x + 200 * scaleX - startGameText.getGlobalBounds().width/2,
            startGameButton.getPosition().y + 30 * scaleY - startGameText.getGlobalBounds().height/2
        );
        mapBackButton.setSize(sf::Vector2f(400 * scaleX, 60 * scaleY));
        mapBackButton.setPosition(centerX - 200 * scaleX, centerY + 300 * scaleY);
        mapBackButton.setFillColor(sf::Color(180, 70, 70));
        mapBackText.setFont(font);
        mapBackText.setString(L"Вернуться в меню");
        mapBackText.setCharacterSize(30 * scaleY);
        mapBackText.setFillColor(sf::Color::White);
        mapBackText.setPosition(
            mapBackButton.getPosition().x + 200 * scaleX - mapBackText.getGlobalBounds().width/2,
            mapBackButton.getPosition().y + 30 * scaleY - mapBackText.getGlobalBounds().height/2
        );

        // --- CHARACTER SELECT ---
        redCubeButton.setSize(sf::Vector2f(200 * scaleX, 200 * scaleY));
        redCubeButton.setFillColor(sf::Color::Red);
        redCubeButton.setPosition(centerX - 350 * scaleX, centerY - 100 * scaleY);
        greenCubeButton.setSize(sf::Vector2f(200 * scaleX, 200 * scaleY));
        greenCubeButton.setFillColor(sf::Color::Green);
        greenCubeButton.setPosition(centerX - 100 * scaleX, centerY - 100 * scaleY);
        blueCubeButton.setSize(sf::Vector2f(200 * scaleX, 200 * scaleY));
        blueCubeButton.setFillColor(sf::Color::Blue);
        blueCubeButton.setPosition(centerX + 150 * scaleX, centerY - 100 * scaleY);
        redCubeText.setFont(font);
        redCubeText.setString(L"Красный");
        redCubeText.setCharacterSize(32 * scaleY);
        redCubeText.setFillColor(sf::Color::White);
        redCubeText.setPosition(redCubeButton.getPosition().x + 60 * scaleX, redCubeButton.getPosition().y + 210 * scaleY);
        greenCubeText.setFont(font);
        greenCubeText.setString(L"Зеленый");
        greenCubeText.setCharacterSize(32 * scaleY);
        greenCubeText.setFillColor(sf::Color::White);
        greenCubeText.setPosition(greenCubeButton.getPosition().x + 50 * scaleX, greenCubeButton.getPosition().y + 210 * scaleY);
        blueCubeText.setFont(font);
        blueCubeText.setString(L"Синий");
        blueCubeText.setCharacterSize(32 * scaleY);
        blueCubeText.setFillColor(sf::Color::White);
        blueCubeText.setPosition(blueCubeButton.getPosition().x + 60 * scaleX, blueCubeButton.getPosition().y + 210 * scaleY);

        // --- Остальные элементы (логин/регистрация/ошибки) ---
        float formY = centerY - fieldHeight - spacingY;
        usernameText.setFont(font);
        usernameText.setString(L"Имя пользователя:");
        usernameText.setCharacterSize(30 * scaleY);
        usernameText.setFillColor(sf::Color::White);
        usernameText.setPosition(centerX - fieldWidth/2 - labelOffsetX - 200 * scaleX, formY);
        usernameBox.setSize(sf::Vector2f(fieldWidth, fieldHeight));
        usernameBox.setPosition(centerX - fieldWidth/2, formY);
        usernameBox.setFillColor(sf::Color::White);
        inputText.setFont(font);
        inputText.setCharacterSize(30 * scaleY);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(usernameBox.getPosition().x + 10 * scaleX, usernameBox.getPosition().y + 8 * scaleY);
        passwordText.setFont(font);
        passwordText.setString(L"Пароль:");
        passwordText.setCharacterSize(30 * scaleY);
        passwordText.setFillColor(sf::Color::White);
        passwordText.setPosition(centerX - fieldWidth/2 - labelOffsetX - 200 * scaleX, formY + fieldHeight + spacingY);
        passwordBox.setSize(sf::Vector2f(fieldWidth, fieldHeight));
        passwordBox.setPosition(centerX - fieldWidth/2, formY + fieldHeight + spacingY);
        passwordBox.setFillColor(sf::Color::White);
        passwordInputText.setFont(font);
        passwordInputText.setCharacterSize(30 * scaleY);
        passwordInputText.setFillColor(sf::Color::Black);
        passwordInputText.setPosition(passwordBox.getPosition().x + 10 * scaleX, passwordBox.getPosition().y + 8 * scaleY);
        float showBtnSize = fieldHeight;
        showPasswordButton.setSize(sf::Vector2f(showBtnSize, showBtnSize));
        showPasswordButton.setPosition(passwordBox.getPosition().x + fieldWidth + 10 * scaleX, passwordBox.getPosition().y);
        showPasswordButton.setFillColor(sf::Color(70, 130, 180));
        showPasswordButton.setOutlineThickness(2);
        showPasswordButton.setOutlineColor(sf::Color::White);
        showPasswordText.setFont(font);
        showPasswordText.setString(isPasswordVisible ? L"1" : L"Х");
        showPasswordText.setCharacterSize(22 * scaleY);
        showPasswordText.setFillColor(sf::Color::White);
        showPasswordText.setPosition(
            showPasswordButton.getPosition().x + showBtnSize/2 - showPasswordText.getGlobalBounds().width/2,
            showPasswordButton.getPosition().y + showBtnSize/2 - showPasswordText.getGlobalBounds().height/1.5f
        );
        submitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        submitButton.setPosition(centerX - buttonWidth/2, passwordBox.getPosition().y + fieldHeight + spacingY * 2);
        submitButton.setFillColor(sf::Color(70, 130, 180));
        submitButton.setOutlineThickness(3);
        submitButton.setOutlineColor(sf::Color::White);
        submitText.setFont(font);
        submitText.setString(L"Регистрация");
        submitText.setCharacterSize(36 * scaleY);
        submitText.setFillColor(sf::Color::White);
        
        // Центрируем текст кнопки
        submitText.setPosition(
            submitButton.getPosition().x + (buttonWidth - submitText.getGlobalBounds().width) / 2,
            submitButton.getPosition().y + (buttonHeight - submitText.getGlobalBounds().height) / 2
        );
        backButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        backButton.setPosition(centerX - buttonWidth/2, submitButton.getPosition().y + buttonHeight + spacingY);
        backButton.setFillColor(sf::Color(180, 70, 70));
        backButton.setOutlineThickness(3);
        backButton.setOutlineColor(sf::Color::White);
        backText.setFont(font);
        backText.setString(L"Назад");
        backText.setCharacterSize(36 * scaleY);
        backText.setFillColor(sf::Color::White);
        backText.setPosition(
            backButton.getPosition().x + buttonWidth/2 - backText.getGlobalBounds().width/2,
            backButton.getPosition().y + buttonHeight/2 - backText.getGlobalBounds().height/2
        );
        errorText.setFont(font);
        errorText.setCharacterSize(20 * scaleY);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(centerX - buttonWidth/2, backButton.getPosition().y + buttonHeight + spacingY);

        // Initialize Highscores elements
        highscoresBackButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        highscoresBackButton.setPosition(centerX - buttonWidth/2, screenSize.y - buttonHeight - 20 * scaleY);
        highscoresBackButton.setFillColor(sf::Color(70, 130, 180));
        highscoresBackButton.setOutlineThickness(3);
        highscoresBackButton.setOutlineColor(sf::Color::White);

        highscoresBackText.setFont(font);
        highscoresBackText.setString(L"Назад");
        highscoresBackText.setCharacterSize(36 * scaleY);
        highscoresBackText.setFillColor(sf::Color::White);
        highscoresBackText.setPosition(
            highscoresBackButton.getPosition().x + buttonWidth/2 - highscoresBackText.getGlobalBounds().width/2,
            highscoresBackButton.getPosition().y + buttonHeight/2 - highscoresBackText.getGlobalBounds().height/2
        );

        highscoresTitleText.setFont(font);
        highscoresTitleText.setString(L"Таблица лидеров");
        highscoresTitleText.setCharacterSize(48 * scaleY);
        highscoresTitleText.setFillColor(sf::Color::White);
        highscoresTitleText.setPosition(
            centerX - highscoresTitleText.getGlobalBounds().width/2,
            50 * scaleY
        );

        highscoresTable.setSize(sf::Vector2f(800 * scaleX, 500 * scaleY));
        highscoresTable.setPosition(centerX - 400 * scaleX, 150 * scaleY);
        highscoresTable.setFillColor(sf::Color(0, 0, 0, 200));
        highscoresTable.setOutlineThickness(2);
        highscoresTable.setOutlineColor(sf::Color::White);

        highscoresTableText.setFont(font);
        highscoresTableText.setString(L"Пока нет записей");
        highscoresTableText.setCharacterSize(36 * scaleY);
        highscoresTableText.setFillColor(sf::Color::White);
        highscoresTableText.setPosition(
            centerX - highscoresTableText.getGlobalBounds().width/2,
            400 * scaleY
        );

        // Character select elements
        characterSelectTitle.setFont(font);
        characterSelectTitle.setString(L"Выберите персонажа");
        characterSelectTitle.setCharacterSize(40);
        characterSelectTitle.setFillColor(sf::Color::White);
        characterSelectTitle.setPosition(
            window.getSize().x / 2 - characterSelectTitle.getGlobalBounds().width / 2,
            50
        );
        
        characterBackButton.setSize(sf::Vector2f(200, 50));
        characterBackButton.setPosition(50, window.getSize().y - 70);
        characterBackButton.setFillColor(sf::Color(70, 130, 180));
        
        characterBackText.setFont(font);
        characterBackText.setString(L"Назад");
        characterBackText.setCharacterSize(24);
        characterBackText.setFillColor(sf::Color::White);
        characterBackText.setPosition(
            characterBackButton.getPosition().x + characterBackButton.getSize().x / 2 - characterBackText.getGlobalBounds().width / 2,
            characterBackButton.getPosition().y + characterBackButton.getSize().y / 2 - characterBackText.getGlobalBounds().height / 2
        );
    } catch (const std::exception& e) {
        std::cerr << "Error initializing UI elements: " << e.what() << std::endl;
        throw;
    }
}

void AuthWindow::run() {
    try {
        while (window.isOpen()) {
            handleEvents();
            draw();
        }
    } catch (const exception& e) {
        cerr << "Error in main loop: " << e.what() << endl;
        throw;
    }
}

void AuthWindow::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        switch (currentState) {
            case WindowState::MAIN:
                handleMainWindowEvents(event);
                break;
            case WindowState::LOGIN:
                handleLoginWindowEvents(event);
                break;
            case WindowState::REGISTER:
                handleRegisterWindowEvents(event);
                break;
            case WindowState::MAIN_MENU:
                handleMainMenuEvents(event);
                break;
            case WindowState::MAP_SELECTION:
                handleMapSelectionEvents(event);
                break;
            case WindowState::CHARACTER_SELECT:
                handleCharacterSelectEvents(event);
                break;
            case WindowState::SETTINGS:
                handleSettingsEvents(event);
                break;
            case WindowState::EXIT_CONFIRMATION:
                handleExitConfirmationEvents(event);
                break;
            case WindowState::HIGHSCORES:
                handleHighscoresEvents(event);
                break;
            case WindowState::PAUSE:
                // Handle pause state if needed
                break;
        }
    }
}

void AuthWindow::handleMainWindowEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (loginButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::LOGIN;
            submitText.setString(L"Вход");
            username = "";
            password = "";
            inputText.setString("");
            passwordInputText.setString("");
            errorText.setString("");
        }
        else if (registerButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::REGISTER;
            submitText.setString(L"Регистрация");
            username = "";
            password = "";
            inputText.setString("");
            passwordInputText.setString("");
            errorText.setString("");
        }
        else if (exitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            window.close();
        }
    }
}

void AuthWindow::handleLoginWindowEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        isUsernameActive = usernameBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        isPasswordActive = passwordBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        if (submitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            handleSubmit();
        }
        else if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::MAIN;
        }
        else if (showPasswordButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            togglePasswordVisibility();
        }
    }
    
    if (event.type == sf::Event::TextEntered) {
        if (isUsernameActive) {
            if (event.text.unicode == '\b' && !username.empty()) {
                username.pop_back();
            }
            else if (event.text.unicode < 128) {
                username += static_cast<char>(event.text.unicode);
            }
            inputText.setString(username);
            cursor.setPosition(inputText.getPosition().x + inputText.getGlobalBounds().width + 2, 
                             inputText.getPosition().y);
        }
        else if (isPasswordActive) {
            if (event.text.unicode == '\b' && !password.empty()) {
                password.pop_back();
            }
            else if (event.text.unicode < 128) {
                password += static_cast<char>(event.text.unicode);
            }
            passwordInputText.setString(isPasswordVisible ? password : string(password.length(), '*'));
            cursor.setPosition(passwordInputText.getPosition().x + passwordInputText.getGlobalBounds().width + 2,
                             passwordInputText.getPosition().y);
        }
    }
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        handleSubmit();
    }
}

void AuthWindow::handleRegisterWindowEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        isUsernameActive = usernameBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        isPasswordActive = passwordBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        if (submitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            handleSubmit();
        }
        else if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::MAIN;
        }
        else if (showPasswordButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            togglePasswordVisibility();
        }
    }
    
    if (event.type == sf::Event::TextEntered) {
        if (isUsernameActive) {
            if (event.text.unicode == '\b' && !username.empty()) {
                username.pop_back();
            }
            else if (event.text.unicode < 128) {
                username += static_cast<char>(event.text.unicode);
            }
            inputText.setString(username);
            cursor.setPosition(inputText.getPosition().x + inputText.getGlobalBounds().width + 2, 
                             inputText.getPosition().y);
        }
        else if (isPasswordActive) {
            if (event.text.unicode == '\b' && !password.empty()) {
                password.pop_back();
            }
            else if (event.text.unicode < 128) {
                password += static_cast<char>(event.text.unicode);
            }
            passwordInputText.setString(isPasswordVisible ? password : string(password.length(), '*'));
            cursor.setPosition(passwordInputText.getPosition().x + passwordInputText.getGlobalBounds().width + 2,
                             passwordInputText.getPosition().y);
        }
    }
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        handleSubmit();
    }
}

void AuthWindow::handleMainMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (characterButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::MAP_SELECTION;
        }
        else if (characterSelectButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::CHARACTER_SELECT;
        }
        else if (settingsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            cout << "Entering settings. Current track: " << currentTrack << ", Volume: " << musicVolume << ", isPlaying: " << isMusicPlaying << endl;
            // Сохраняем текущее состояние музыки перед входом в настройки
            if (currentTrack >= 0 && currentTrack < 4) {
                musicTracks[currentTrack].setVolume(musicVolume);
                // Обновляем текст текущего трека в настройках
               currentTrackText.setString(sf::String(L"Трек ") + std::to_wstring(currentTrack + 1));
                currentTrackText.setPosition(
                    currentTrackText.getPosition().x + currentTrackText.getGlobalBounds().width/2 - currentTrackText.getGlobalBounds().width/2,
                    currentTrackText.getPosition().y
                );
            }
            currentState = WindowState::SETTINGS;
        }
        else if (recordsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::HIGHSCORES;
        }
        else if (menuExitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            window.close();
        }
    }
}

void AuthWindow::handleMapSelectionEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (mapSelectButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            selectedMap = (selectedMap % 3) + 1;
            mapSelectText.setString(L"Карта " + std::to_wstring(selectedMap));
            
            mapSelectText.setPosition(
                mapSelectButton.getPosition().x + mapSelectButton.getSize().x/2 - mapSelectText.getGlobalBounds().width/2,
                mapSelectButton.getPosition().y + mapSelectButton.getSize().y/2 - mapSelectText.getGlobalBounds().height/2
            );
        }
        else if (difficultySelectButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            // Переключение между уровнями сложности
            if (difficultySelectText.getString() == L"Легкая") {
                difficultySelectText.setString(L"Средняя");
            } else if (difficultySelectText.getString() == L"Средняя") {
                difficultySelectText.setString(L"Сложная");
            } else {
                difficultySelectText.setString(L"Легкая");
            }
            
            // Центрирование текста после изменения
            difficultySelectText.setPosition(
                difficultySelectButton.getPosition().x + difficultySelectButton.getSize().x/2 - difficultySelectText.getGlobalBounds().width/2,
                difficultySelectButton.getPosition().y + difficultySelectButton.getSize().y/2 - difficultySelectText.getGlobalBounds().height/2
            );
        }
        else if (startGameButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            // Определяем выбранную сложность
            Difficulty selectedDifficulty = Difficulty::EASY; // По умолчанию
            if (difficultySelectText.getString() == L"Средняя") {
                selectedDifficulty = Difficulty::MEDIUM;
            } else if (difficultySelectText.getString() == L"Сложная") {
                selectedDifficulty = Difficulty::HARD;
            }

            // Создание и запуск игры с выбранной сложностью
            window.close();
            Game game(selectedDifficulty, selectedMap, username, selectedCharacter);
            game.run();
        }
        else if (mapBackButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            playButtonClickSound();
            currentState = WindowState::MAIN_MENU;
        }
    }
}

void AuthWindow::handleCharacterSelectEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // Check hover states for each character
        isMikeHovered = mikeSprite.getGlobalBounds().contains(mousePos.x, mousePos.y);
        isJesyHovered = jesySprite.getGlobalBounds().contains(mousePos.x, mousePos.y);
        isGremHovered = gremSprite.getGlobalBounds().contains(mousePos.x, mousePos.y);
        
        // Update sprite scale based on hover
        float normalScale = 0.8f;
        float hoverScale = 1.0f;
        
        if (isMikeHovered) {
            mikeSprite.setScale(hoverScale, hoverScale);
        } else if (selectedCharacter != 0) {
            mikeSprite.setScale(normalScale, normalScale);
        }
        
        if (isJesyHovered) {
            jesySprite.setScale(hoverScale, hoverScale);
        } else if (selectedCharacter != 1) {
            jesySprite.setScale(normalScale, normalScale);
        }
        
        if (isGremHovered) {
            gremSprite.setScale(hoverScale, hoverScale);
        } else if (selectedCharacter != 2) {
            gremSprite.setScale(normalScale, normalScale);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // Reset all characters to normal scale first
        float normalScale = 0.8f;
        mikeSprite.setScale(normalScale, normalScale);
        jesySprite.setScale(normalScale, normalScale);
        gremSprite.setScale(normalScale, normalScale);
        
        // Check if any character is clicked
        if (mikeSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            playButtonClickSound();
            selectedCharacter = 0;
            mikeSprite.setScale(1.0f, 1.0f); // Keep enlarged for selected character
        }
        else if (jesySprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            playButtonClickSound();
            selectedCharacter = 1;
            jesySprite.setScale(1.0f, 1.0f);
        }
        else if (gremSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            playButtonClickSound();
            selectedCharacter = 2;
            gremSprite.setScale(1.0f, 1.0f);
        }
        else if (characterBackButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            playButtonClickSound();
            currentState = WindowState::MAIN_MENU;
        }
    }
}

void AuthWindow::handleSettingsEvents(const sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    float sliderWidth = musicSlider.getSize().x;
    float handleSize = musicSliderHandle.getSize().x;

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if clicking on slider handles
            if (musicSliderHandle.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                isDraggingMusicSlider = true;
            }
            else if (effectsSliderHandle.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                isDraggingEffectsSlider = true;
            }
            // Check other buttons
            else if (prevTrackButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                playButtonClickSound();
                if (currentTrack >= 0 && currentTrack < 4) {
                    musicTracks[currentTrack].stop();
                }
                currentTrack = (currentTrack - 1 + 4) % 4;
                currentTrackText.setString(L"Трек " + std::to_wstring(currentTrack + 1));
                currentTrackText.setPosition(
                    currentTrackText.getPosition().x + currentTrackText.getGlobalBounds().width/2 - currentTrackText.getGlobalBounds().width/2,
                    currentTrackText.getPosition().y
                );
                if (currentTrack >= 0 && currentTrack < 4) {
                    musicTracks[currentTrack].setVolume(musicVolume);
                    musicTracks[currentTrack].play();
                    isMusicPlaying = true;
                }
            }
            else if (nextTrackButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                playButtonClickSound();
                if (currentTrack >= 0 && currentTrack < 4) {
                    musicTracks[currentTrack].stop();
                }
                currentTrack = (currentTrack + 1) % 4;
                currentTrackText.setString(L"Трек " + std::to_wstring(currentTrack + 1));
                currentTrackText.setPosition(
                    currentTrackText.getPosition().x + currentTrackText.getGlobalBounds().width/2 - currentTrackText.getGlobalBounds().width/2,
                    currentTrackText.getPosition().y
                );
                if (currentTrack >= 0 && currentTrack < 4) {
                    musicTracks[currentTrack].setVolume(musicVolume);
                    musicTracks[currentTrack].play();
                    isMusicPlaying = true;
                }
            }
            else if (themeColorButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                playButtonClickSound();
                // Toggle theme
                isDarkTheme = !isDarkTheme;
                if (isDarkTheme) {
                    background.setTexture(&backgroundTexture);
                    themeColorText.setString(L"Тёмная");
                } else {
                    background.setTexture(&lightBackgroundTexture);
                    themeColorText.setString(L"Светлая");
                }
                // Update text position after changing string
                themeColorText.setPosition(
                    themeColorButton.getPosition().x + themeColorButton.getSize().x/2 - themeColorText.getGlobalBounds().width/2,
                    themeColorButton.getPosition().y + themeColorButton.getSize().y/2 - themeColorText.getGlobalBounds().height/2
                );
            }
            else if (settingsBackButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                playButtonClickSound();
                cout << "Returning to menu. Current track: " << currentTrack << ", Volume: " << musicVolume << ", isPlaying: " << isMusicPlaying << endl;
                // Сохраняем текущее состояние перед возвратом в меню
                if (currentTrack >= 0 && currentTrack < 4) {
                    musicTracks[currentTrack].setVolume(musicVolume);
                }
                currentState = WindowState::MAIN_MENU;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isDraggingMusicSlider = false;
            isDraggingEffectsSlider = false;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (isDraggingMusicSlider) {
            float newX = std::max(musicSlider.getPosition().x,
                                std::min(musicSlider.getPosition().x + sliderWidth - handleSize,
                                        static_cast<float>(mousePos.x) - handleSize/2));
            musicSliderHandle.setPosition(newX, musicSliderHandle.getPosition().y);
            musicVolume = ((newX - musicSlider.getPosition().x) / (sliderWidth - handleSize)) * 100.0f;

            // Update volume of current music track only
            if (currentTrack >= 0 && currentTrack < 4) {
                musicTracks[currentTrack].setVolume(musicVolume);
            }
        }
        else if (isDraggingEffectsSlider) {
            float newX = std::max(effectsSlider.getPosition().x,
                                std::min(effectsSlider.getPosition().x + sliderWidth - handleSize,
                                        static_cast<float>(mousePos.x) - handleSize/2));
            effectsSliderHandle.setPosition(newX, effectsSliderHandle.getPosition().y);
            effectsVolume = ((newX - effectsSlider.getPosition().x) / (sliderWidth - handleSize)) * 100.0f;
        }
    }
}

void AuthWindow::handleSubmit() {
    try {
        std::cout << "HandleSubmit called with state: " << 
            (currentState == WindowState::LOGIN ? "LOGIN" : 
             currentState == WindowState::REGISTER ? "REGISTER" : "OTHER") << std::endl;
        
        std::cout << "Username: " << username << ", Password length: " << password.length() << std::endl;
        
        if (currentState == WindowState::LOGIN) {
            if (User::loginUser(username, password)) {
                std::cout << "Login successful, changing to MAIN_MENU" << std::endl;
                enterMainMenu();
                errorText.setString(L"");
            } else {
                std::cout << "Login failed" << std::endl;
                errorText.setString(L"Неверное имя\nпользователя\nили пароль");
            }
        } else if (currentState == WindowState::REGISTER) {
            if (User::registerUser(username, password)) {
                std::cout << "Registration successful, changing to MAIN_MENU" << std::endl;
                enterMainMenu();
                errorText.setString(L"");
            } else {
                std::cout << "Registration failed" << std::endl;
                errorText.setString(L"Ошибка регистрации.\nИмя пользователя\nуже существует.");
            }
        }
    } catch (const exception& e) {
        cerr << "Error during submit: " << e.what() << endl;
        errorText.setString("Произошла ошибка: " + string(e.what()));
    }
}

void AuthWindow::togglePasswordVisibility() {
    isPasswordVisible = !isPasswordVisible;
    passwordInputText.setString(isPasswordVisible ? password : string(password.length(), '*'));
}

void AuthWindow::enterMainMenu() {
    currentState = WindowState::MAIN_MENU;
    // Восстанавливаем состояние музыки
    if (currentTrack >= 0 && currentTrack < 4) {
        // Останавливаем все треки кроме текущего
        for (int i = 0; i < 4; ++i) {
            if (i != currentTrack) {
                musicTracks[i].stop();
            }
        }
        // Устанавливаем громкость и запускаем текущий трек только если он не играет
        musicTracks[currentTrack].setVolume(musicVolume);
        if (!isMusicPlaying) {
            musicTracks[currentTrack].play();
            isMusicPlaying = true;
        }
    }
}

void AuthWindow::handleExitConfirmationEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (confirmExitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            window.close();
        }
        else if (cancelExitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            currentState = WindowState::MAIN_MENU;
        }
    }
}

void AuthWindow::handleHighscoresEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
            if (highscoresBackButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                playButtonClickSound();
                currentState = WindowState::MAIN_MENU;
            }
        }
    }
}

void AuthWindow::draw() {
    window.clear();
    window.draw(background);
    
    switch (currentState) {
        case WindowState::MAIN:
            drawMainWindow();
            break;
        case WindowState::LOGIN:
            drawLoginWindow();
            break;
        case WindowState::REGISTER:
            drawRegisterWindow();
            break;
        case WindowState::MAIN_MENU:
            drawMainMenu();
            break;
        case WindowState::MAP_SELECTION:
            drawMapSelection();
            break;
        case WindowState::CHARACTER_SELECT:
            drawCharacterSelect();
            break;
        case WindowState::SETTINGS:
            drawSettings();
            break;
        case WindowState::EXIT_CONFIRMATION:
            drawExitConfirmation();
            break;
        case WindowState::HIGHSCORES:
            drawHighscores();
            break;
        case WindowState::PAUSE:
            // Draw pause state if needed
            break;
    }
    
    window.display();
}

void AuthWindow::drawMainWindow() {
    window.draw(loginButton);
    window.draw(loginText);
    window.draw(registerButton);
    window.draw(registerText);
    window.draw(exitButton);
    window.draw(exitText);
}

void AuthWindow::drawLoginWindow() {
    window.draw(usernameBox);
    window.draw(passwordBox);
    window.draw(submitButton);
    window.draw(backButton);
    window.draw(showPasswordButton);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(submitText);
    window.draw(backText);
    window.draw(showPasswordText);
    window.draw(inputText);
    window.draw(passwordInputText);
    window.draw(errorText);
    
    if ((isUsernameActive || isPasswordActive) && showCursor) {
        window.draw(cursor);
    }
}

void AuthWindow::drawRegisterWindow() {
    window.draw(usernameBox);
    window.draw(passwordBox);
    window.draw(submitButton);
    window.draw(backButton);
    window.draw(showPasswordButton);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(submitText);
    window.draw(backText);
    window.draw(showPasswordText);
    window.draw(inputText);
    window.draw(passwordInputText);
    window.draw(errorText);
    
    if ((isUsernameActive || isPasswordActive) && showCursor) {
        window.draw(cursor);
    }
}

void AuthWindow::drawMainMenu() {
    window.draw(characterButton);
    window.draw(characterText);
    window.draw(characterSelectButton);
    window.draw(characterSelectText);
    window.draw(settingsButton);
    window.draw(settingsText);
    window.draw(recordsButton);
    window.draw(recordsText);
    window.draw(menuExitButton);
    window.draw(menuExitText);
}

void AuthWindow::drawMapSelection() {
    window.draw(mapTitleText);
    window.draw(mapSelectButton);
    window.draw(mapSelectText);
    window.draw(difficultyTitleText);
    window.draw(difficultySelectButton);
    window.draw(difficultySelectText);
    window.draw(startGameButton);
    window.draw(startGameText);
    window.draw(mapBackButton);
    window.draw(mapBackText);
}

void AuthWindow::drawCharacterSelect() {
    // Draw background
    window.draw(background);
    
    // Draw title
    window.draw(characterSelectTitle);
    
    // Draw character sprites
    float centerX = window.getSize().x / 2.f;
    float windowWidth = window.getSize().x;
    float centerY = window.getSize().y / 2.f;

    // Calculate positions to place characters more distinctly: left, center, right
    float leftX = windowWidth * 0.25f;  // 25% от ширины экрана
    float middleX = centerX;            // Центр экрана
    float rightX = windowWidth * 0.75f; // 75% от ширины экрана

    // Fixed Y-coordinates for the center of sprites and texts
    float spriteCenterY = centerY - 120; // Y-координата для центров спрайтов
    float textCenterY = centerY + 70;   // Y-координата для центров текста

    // Position sprites (their centers are placed at these coordinates)
    mikeSprite.setPosition(leftX, spriteCenterY);
    jesySprite.setPosition(middleX, spriteCenterY);
    gremSprite.setPosition(rightX, spriteCenterY);
    
    // Position names (using their already set origins, so position is their center)
    mikeText.setPosition(leftX, textCenterY);
    jesyText.setPosition(middleX, textCenterY);
    gremText.setPosition(rightX, textCenterY);
    
    // Draw sprites
    window.draw(mikeSprite);
    window.draw(jesySprite);
    window.draw(gremSprite);
    
    // Draw character names
    window.draw(mikeText);
    window.draw(jesyText);
    window.draw(gremText);
    
    // Draw back button
    window.draw(characterBackButton);
    window.draw(characterBackText);
}

void AuthWindow::drawSettings() {
    window.draw(settingsTitleText);
    window.draw(musicVolumeText);
    window.draw(musicSlider);
    window.draw(musicSliderHandle);
    window.draw(effectsVolumeText);
    window.draw(effectsSlider);
    window.draw(effectsSliderHandle);
    window.draw(prevTrackButton);
    window.draw(nextTrackButton);
    window.draw(currentTrackText);
    window.draw(themeColorButton);
    window.draw(themeColorText);
    window.draw(settingsBackButton);
    window.draw(settingsBackText);
}

void AuthWindow::drawExitConfirmation() {
    window.draw(confirmExitButton);
    window.draw(confirmExitText);
    window.draw(cancelExitButton);
    window.draw(cancelExitText);
    window.draw(exitConfirmationText);
}

void AuthWindow::drawHighscores() {
    window.draw(highscoresTitleText);
    window.draw(highscoresTable);
    window.draw(highscoresBackButton);
    window.draw(highscoresBackText);

    // Получаем записи из таблицы рекордов
    auto records = Records::getInstance().getRecords();
    
    if (records.empty()) {
        window.draw(highscoresTableText);
    } else {
        // Отображаем записи
        float startY = highscoresTable.getPosition().y + 20;
        float rowHeight = 40;
        int index = 0;
        
        for (const auto& record : records) {
            sf::Text recordText;
            recordText.setFont(font);
            recordText.setCharacterSize(24);
            recordText.setFillColor(sf::Color::White);
            
            // Форматируем время в минуты и секунды
            int minutes = record.score / 60;
            int seconds = record.score % 60;
            std::string timeStr = std::to_string(minutes) + ":" + 
                                (seconds < 10 ? "0" : "") + std::to_string(seconds);
            
            // Форматируем строку записи
            std::string recordStr = std::to_string(index + 1) + ". " + 
                                  record.username + " - " + 
                                  timeStr + " (" + 
                                  record.difficulty + ", Map " + 
                                  std::to_string(record.mapNumber) + ")";
            
            recordText.setString(recordStr);
            recordText.setPosition(
                highscoresTable.getPosition().x + 20,
                startY + index * rowHeight
            );
            
            window.draw(recordText);
            index++;
        }
    }
}

void AuthWindow::setState(WindowState state) {
    if (state == WindowState::CHARACTER_SELECT) {
        // Reset character selection highlight when entering this state
        selectedCharacter = -1; // No character selected initially
        // Also reset all character scales to normal when entering this state
        float normalScale = 0.8f;
        mikeSprite.setScale(normalScale, normalScale);
        jesySprite.setScale(normalScale, normalScale);
        gremSprite.setScale(normalScale, normalScale);
    }
    currentState = state;
}

void AuthWindow::playButtonClickSound() {
    buttonClickSound.setVolume(effectsVolume);
    buttonClickSound.play();
}

void AuthWindow::playKeyCollectSound() {
    keyCollectSound.setVolume(effectsVolume);
    keyCollectSound.play();
}