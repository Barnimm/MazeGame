#include "Game.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <string> // Добавим для std::to_string
#include "Key.hpp"
#include "Trap.hpp"
#include "Records.hpp"
#include "AuthWindow.hpp"

// Удаляем статические константы размера лабиринта
// static constexpr int MAZE_WIDTH = 15;
// static constexpr int MAZE_HEIGHT = 15;
static constexpr int CELL_SIZE = 32;

namespace {
    // Helper function to load texture with chroma key
    bool loadTextureWithChromaKey(sf::Texture& texture, const std::string& filename, sf::Color chromaKey) {
        sf::Image image;
        if (!image.loadFromFile(filename)) {
            std::cerr << "Failed to load image for chroma key: " << filename << std::endl;
            return false;
        }
        // Define the chroma key color (green)
        image.createMaskFromColor(chromaKey);
        texture.loadFromImage(image);
        return true;
    }
}

Game::Game(Difficulty difficulty, int mapNumber, const std::string& username, int selectedCharacter)
    : window(sf::VideoMode::getDesktopMode(), "Maze Game", sf::Style::Fullscreen),
      player(sf::Vector2f(30.f, 30.f)),
      gameDifficulty(difficulty),
      currentMap(mapNumber),
      username(username),
      isPaused(false),
      isDraggingMusicSlider(false),
      isDraggingEffectsSlider(false),
      musicVolume(50.0f),
      effectsVolume(50.0f),
      playerVelocity(0.f, 0.f),
      playerSpeed(8.0f),
      initialSelectedCharacter(selectedCharacter) {
    window.setFramerateLimit(60);
    
    // Load sound effects
    if (!keyBuffer.loadFromFile("/Users/misa/Desktop/курсовая основа/sounds effects/sborKlucha.ogg")) {
        std::cerr << "Failed to load key sound" << std::endl;
    }
    if (!doorBuffer.loadFromFile("/Users/misa/Desktop/курсовая основа/sounds effects/door.wav")) {
        std::cerr << "Failed to load door sound" << std::endl;
    }
    if (!finishBuffer.loadFromFile("sounds/finish.wav")) {
        std::cerr << "Failed to load finish sound" << std::endl;
    }
    if (!damageBuffer.loadFromFile("/Users/misa/Desktop/курсовая основа/sounds effects/die.ogg")) {
        std::cerr << "Failed to load damage sound" << std::endl;
    }
    
    keySound.setBuffer(keyBuffer);
    doorSound.setBuffer(doorBuffer);
    finishSound.setBuffer(finishBuffer);
    damageSound.setBuffer(damageBuffer);
    
    // Load background music
    if (!backgroundMusic.openFromFile("sounds/background.ogg")) {
        std::cerr << "Failed to load background music" << std::endl;
    }
    backgroundMusic.setLoop(true);
    backgroundMusic.play();
    
    // Set initial volumes
    backgroundMusic.setVolume(musicVolume);
    keySound.setVolume(effectsVolume);
    doorSound.setVolume(effectsVolume);
    finishSound.setVolume(effectsVolume);
    damageSound.setVolume(effectsVolume);
    
    // Устанавливаем размеры лабиринта в зависимости от сложности
    switch (difficulty) {
        case Difficulty::EASY:
            MAZE_WIDTH = 30;
            MAZE_HEIGHT = 30;
            break;
        case Difficulty::MEDIUM:
            MAZE_WIDTH = 45;
            MAZE_HEIGHT = 45;
            break;
        case Difficulty::HARD:
            MAZE_WIDTH = 60;
            MAZE_HEIGHT = 60;
            break;
    }

    // Initialize game state
    playerHealth = 100;
    invincibilityTime = 0;
    isInvincible = false;
    collectedKeys = 0;
    doorOpen = false;
    
    // Initialize damage flash
    damageFlash.setFillColor(sf::Color(255, 0, 0, 128)); // Semi-transparent red
    damageFlashTime = 0;
    isFlashing = false;
    
    // Load font
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    
    // Load floor texture based on map number
    std::string floorTexturePath;
    switch (currentMap) {
        case 1:
            floorTexturePath = "/Users/misa/Desktop/курсовая основа/texture/floor/floorLVL1.png";
            break;
        case 2:
            floorTexturePath = "/Users/misa/Desktop/курсовая основа/texture/floor/floorLVL2.png";
            break;
        case 3:
            floorTexturePath = "/Users/misa/Desktop/курсовая основа/texture/floor/floorLVL3.png";
            break;
    }

    if (!floorTexture.loadFromFile(floorTexturePath)) {
        std::cerr << "Failed to load floor texture from " << floorTexturePath << std::endl;
    } else {
        std::cout << "Successfully loaded floor texture from " << floorTexturePath << std::endl;
        floorTexture.setRepeated(true);
    }
    
    // Load wall texture based on map number
    std::string wallTexturePath;
    switch (currentMap) {
        case 1:
            wallTexturePath = "/Users/misa/Desktop/курсовая основа/texture/wall/wallLVL1.jpg";
            break;
        case 2:
            wallTexturePath = "/Users/misa/Desktop/курсовая основа/texture/wall/wallLVL2.png";
            break;
        case 3:
            wallTexturePath = "/Users/misa/Desktop/курсовая основа/texture/wall/wallLVL3.png";
            break;
    }
    
    if (!wallTexture.loadFromFile(wallTexturePath)) {
        std::cerr << "Failed to load wall texture from " << wallTexturePath << std::endl;
    } else {
        std::cout << "Successfully loaded wall texture from " << wallTexturePath << std::endl;
        wallTexture.setRepeated(true);
    }
    
    // Load door textures
    if (!closedDoorTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/door/clotheDoor.png")) {
        std::cerr << "Failed to load closed door texture" << std::endl;
    }
    if (!openDoorTexture.loadFromFile("/Users/misa/Desktop/курсовая основа/texture/door/openDoor.png")) {
        std::cerr << "Failed to load open door texture" << std::endl;
    }
    
    // Initialize door sprite
    doorSprite.setTexture(closedDoorTexture);
    // Position the door in the center of the maze
    float doorX = (MAZE_WIDTH / 2) * CELL_SIZE;
    float doorY = (MAZE_HEIGHT / 2) * CELL_SIZE;
    doorSprite.setPosition(doorX, doorY);
    
    // Scale the door to be larger than cell size
    float doorScale = static_cast<float>(CELL_SIZE * 2.25f) / closedDoorTexture.getSize().x; // Увеличили в 1.5 раза
    doorSprite.setScale(doorScale, doorScale);
    
    // Set the origin to center of the sprite
    doorSprite.setOrigin(closedDoorTexture.getSize().x / 2.0f, closedDoorTexture.getSize().y / 2.0f);
    
    // Load key texture with chroma key (green)
    if (!loadTextureWithChromaKey(keyTexture, "/Users/misa/Desktop/курсовая основа/texture/key/keyem.png", sf::Color(0, 255, 0))) {
        std::cerr << "Failed to load key texture with chroma key" << std::endl;
    }
    
    // Load trap texture with chroma key (black)
    if (!loadTextureWithChromaKey(trapTexture, "/Users/misa/Desktop/курсовая основа/texture/enemy/lovushka.png", sf::Color::Black)) {
        std::cerr << "Failed to load trap texture with chroma key" << std::endl;
    }
    
    // Initialize health bar
    healthBarBackground.setSize(sf::Vector2f(200, 20));
    healthBarBackground.setFillColor(sf::Color(50, 50, 50));
    healthBarBackground.setPosition(20, 20);
    
    healthBar.setSize(sf::Vector2f(200, 20));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(20, 20);
    
    healthText.setFont(font);
    healthText.setCharacterSize(20);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(230, 20);
    
    // Initialize timer text
    timerText.setFont(font);
    timerText.setCharacterSize(20);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(450, 20);
    
    loadMaze();
    placeTraps();
    placeKeys();
    
    // Player setup
    float playerSizeMultiplier = 0.5f; // Единый множитель для размера игрока. Увеличено для более стабильной работы хитбокса. Регулируйте это значение для изменения размера.
    
    float playerHitboxDimension = CELL_SIZE * playerSizeMultiplier;
    player.setSize(sf::Vector2f(playerHitboxDimension, playerHitboxDimension));
    player.setFillColor(sf::Color::Transparent); // Делаем хитбокс невидимым
    player.setOrigin(playerHitboxDimension / 2, playerHitboxDimension / 2); // Центр хитбокса
    player.setPosition(CELL_SIZE + CELL_SIZE/2, CELL_SIZE + CELL_SIZE/2); // Начальная позиция хитбокса

    // Load player texture based on selectedCharacter
    std::string playerTexturePath;
    // sf::Color chromaKeyColor = sf::Color::Black; // Удалено использование chromaKeyColor

    switch (selectedCharacter) {
        case 0: // Mike (Медведь)
            playerTexturePath = "/Users/misa/Desktop/курсовая основа/texture/cheracter/bear.png";
            break;
        case 1: // Jesy (Динозавр)
            playerTexturePath = "/Users/misa/Desktop/курсовая основа/texture/cheracter/dino.png";
            break;
        case 2: // Grem (Кот)
            playerTexturePath = "/Users/misa/Desktop/курсовая основа/texture/cheracter/cat.png";
            break;
        default:
            // Default texture if something goes wrong or no selection
            playerTexturePath = "/Users/misa/Desktop/курсовая основа/texture/cheracter/bear.png";
            break;
    }

    if (!playerTexture.loadFromFile(playerTexturePath)) { // Прямая загрузка файла без chroma key
        std::cerr << "Failed to load player texture from " << playerTexturePath << std::endl;
    } else {
        std::cout << "Successfully loaded player texture from " << playerTexturePath << std::endl;
        playerSprite.setTexture(playerTexture); // Применяем загруженную текстуру к спрайту
        
        // Масштабируем текстуру относительно ее оригинального размера, чтобы она соответствовала hitboxDimension
        // и затем применяем visualAdjustmentFactor для визуального увеличения
        float textureScaleX = playerHitboxDimension / playerTexture.getSize().x;
        float textureScaleY = playerHitboxDimension / playerTexture.getSize().y;
        
        float visualAdjustmentFactor = 1.2f; // Коэффициент для визуального увеличения текстуры по сравнению с хитбоксом
        playerSprite.setScale(textureScaleX * visualAdjustmentFactor, textureScaleY * visualAdjustmentFactor);
        
        playerSprite.setOrigin(playerTexture.getSize().x / 2.0f, playerTexture.getSize().y / 2.0f); // Центр спрайта
        playerSprite.setPosition(player.getPosition()); // Располагаем спрайт по позиции хитбокса
    }
    
    // Finish setup
    float finishSize = CELL_SIZE / 2.0f; // Match player size
    finish.setSize(sf::Vector2f(finishSize, finishSize));
    finish.setFillColor(sf::Color::Yellow);
    finish.setOrigin(finishSize / 2, finishSize / 2); // Set origin to center
    // Position the finish in the center of the maze
    finish.setPosition((MAZE_WIDTH / 2) * CELL_SIZE + CELL_SIZE/2, (MAZE_HEIGHT / 2) * CELL_SIZE + CELL_SIZE/2);
    
    // Camera setup
    camera.setSize(window.getSize().x, window.getSize().y);
    camera.setCenter(player.getPosition());
    
    // Set initial camera bounds
    float minX = camera.getSize().x / 2;
    float minY = camera.getSize().y / 2;
    float maxX = MAZE_WIDTH * CELL_SIZE - camera.getSize().x / 2;
    float maxY = MAZE_HEIGHT * CELL_SIZE - camera.getSize().y / 2;
    camera.setCenter(sf::Vector2f(
        std::max(minX, std::min(player.getPosition().x, maxX)),
        std::max(minY, std::min(player.getPosition().y, maxY))
    ));

    initializePauseMenu();
    initializeGameOverWindow();
    showGameOver = false;
}

void Game::placeTraps() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::pair<int, int>> potentialTrapPositions;
    
    // Determine trap damage based on difficulty
    int trapDamage = 0;
    switch (gameDifficulty) {
        case Difficulty::EASY:
            trapDamage = 5;
            break;
        case Difficulty::MEDIUM:
            trapDamage = 10;
            break;
        case Difficulty::HARD:
            trapDamage = 20;
            break;
    }

    // Collect potential trap positions that are path cells with a wall directly above them
    for (int y = 1; y < MAZE_HEIGHT - 1; ++y) { // Start from y=1 to check for wall at y-1
        for (int x = 1; x < MAZE_WIDTH - 1; ++x) {
            if (maze[y][x] == 0 && maze[y-1][x] == 1) { // If it's a path cell AND there's a wall directly above
                 potentialTrapPositions.push_back({x, y});
            }
        }
    }
    
    // Shuffle the potential positions and select 15
    std::shuffle(potentialTrapPositions.begin(), potentialTrapPositions.end(), gen);
    
    traps.clear(); // Clear existing traps
    for (int i = 0; i < 15 && i < potentialTrapPositions.size(); ++i) { // Limit to 15 traps
        int x = potentialTrapPositions[i].first;
        int y = potentialTrapPositions[i].second;
        
        Trap trap(trapTexture, trapDamage); // Pass the determined damage
        
        // Scale the trap sprite to a fixed size relative to CELL_SIZE
        float targetSize = static_cast<float>(CELL_SIZE) * 0.8f; // Make trap 80% of cell size
        float trapOriginalWidth = trapTexture.getSize().x;
        float trapOriginalHeight = trapTexture.getSize().y;
        float scaleFactor = targetSize / std::max(trapOriginalWidth, trapOriginalHeight);
        trap.sprite.setScale(scaleFactor, scaleFactor);
        
        // Get the scaled dimensions for positioning
        float trapWidth = trap.sprite.getGlobalBounds().width;
        float trapHeight = trap.sprite.getGlobalBounds().height;
        
        // Position the trap sprite horizontally centered in the cell
        float finalTrapX = x * CELL_SIZE + CELL_SIZE / 2;
        // Position the trap sprite vertically so its top edge aligns with the top edge of the cell
        // The center of the sprite will be at y*CELL_SIZE + trapHeight / 2
        float finalTrapY = y * CELL_SIZE + trapHeight / 2;
        
        trap.setPosition(finalTrapX, finalTrapY);
        
        traps.push_back(trap);
    }
}

void Game::placeKeys() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::pair<int, int>> potentialKeyPositions;
    
    // Collect positions near the edges of the maze
    for (int y = 1; y < MAZE_HEIGHT - 1; ++y) {
        for (int x = 1; x < MAZE_WIDTH - 1; ++x) {
            if (maze[y][x] == 0) { // If it's a path
                // Check if it's near the edge (within 3 cells)
                if (x <= 3 || x >= MAZE_WIDTH - 4 || y <= 3 || y >= MAZE_HEIGHT - 4) {
                    potentialKeyPositions.push_back({x, y});
                }
            }
        }
    }
    
    // Shuffle and select 3 positions
    std::shuffle(potentialKeyPositions.begin(), potentialKeyPositions.end(), gen);
    
    keys.clear(); // Clear existing keys
    for (int i = 0; i < 3 && i < potentialKeyPositions.size(); ++i) {
        int x = potentialKeyPositions[i].first;
        int y = potentialKeyPositions[i].second;
        
        Key key(keyTexture); // Create Key object with texture
        // Position the key sprite in the center of the cell
        key.setPosition(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2);
        
        // Scale the key sprite to be slightly larger than the player/finish
        // Assuming player/finish size is around CELL_SIZE - 50 (from player initialization)
        // The key sprite's original size is based on the image.
        // We need to scale it relative to its original size.
        float keyOriginalWidth = keyTexture.getSize().x;
        float keyOriginalHeight = keyTexture.getSize().y;
        float targetSize = static_cast<float>(CELL_SIZE) - 30; // Увеличиваем targetSize для увеличения ключа
        float scaleFactor = targetSize / std::max(keyOriginalWidth, keyOriginalHeight);
        key.sprite.setScale(scaleFactor, scaleFactor);
        
        keys.push_back(key);
    }
}

void Game::checkTrapCollisions() {
    if (isInvincible) {
        invincibilityTime -= 1.0f/60.0f; // Assuming 60 FPS
        if (invincibilityTime <= 0) {
            isInvincible = false;
        }
        return;
    }
    
    for (const auto& trap : traps) {
        if (player.getGlobalBounds().intersects(trap.getGlobalBounds())) {
            playerHealth -= trap.getDamage();
            if (playerHealth < 0) playerHealth = 0;
            isInvincible = true;
            invincibilityTime = 1.0f; // 1 second of invincibility
            
            // Start damage flash effect
            isFlashing = true;
            damageFlashTime = 0.5f; // 0.5 seconds flash duration
            
            // Set damage flash size to cover the player
            damageFlash.setSize(player.getSize());
            damageFlash.setPosition(player.getPosition());
            damageFlash.setOrigin(player.getSize().x / 2, player.getSize().y / 2);

            // Воспроизводим звук смерти только когда здоровье достигает 0
            if (playerHealth == 0) {
                damageSound.play();
            }
            break;
        }
    }
}

void Game::checkKeyCollection() {
    for (auto it = keys.begin(); it != keys.end();) {
        if (player.getGlobalBounds().intersects(it->getGlobalBounds())) {
            it->collect(); // Mark key as collected
            collectedKeys++;
            keySound.play(); // Воспроизводим звук сбора ключа
            it = keys.erase(it); // Remove collected key from the list
        } else {
            ++it;
        }
    }
    
    // Update door state
    doorOpen = (collectedKeys == 3);
    if (doorOpen) {
        doorSprite.setTexture(openDoorTexture);
        doorSound.play();
    }
}

void Game::updateDamageFlash() {
    if (isFlashing) {
        damageFlashTime -= 1.0f/60.0f; // Assuming 60 FPS
        
        // Update flash opacity based on remaining time
        float opacity = (damageFlashTime / 0.5f) * 128; // Fade from 128 to 0
        damageFlash.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(opacity)));
        
        // Make flash cover the sides of the screen
        damageFlash.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
        damageFlash.setPosition(0, 0);
        
        if (damageFlashTime <= 0) {
            isFlashing = false;
        }
    }
}

void Game::update() {
    // Update timer
    sf::Time elapsed = gameClock.getElapsedTime();
    
    // Update player sprite position to match hitbox position
    playerSprite.setPosition(player.getPosition());

    // Update door state
    if (collectedKeys == 3 && !doorOpen) {
        doorOpen = true;
        doorSprite.setTexture(openDoorTexture);
        doorSound.play();
    }
    
    if (showGameOver) {
        return;
    }
    
    // Update player input
    handlePlayerInput();
    
    // Update camera
    updateCamera();
    
    // Check for finish
    if (isAtFinish() && doorOpen) {
        showGameOver = true;
        gameOverTitle.setString(L"Вы выиграли!");
        gameOverTitle.setPosition(
            gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTitle.getGlobalBounds().width/2,
            gameOverWindow.getPosition().y + 20
        );
        float finalTime = elapsed.asSeconds();
        int finalMinutes = static_cast<int>(finalTime) / 60;
        int finalSeconds = static_cast<int>(finalTime) % 60;
        gameOverTime.setString(
    L"Время: " + 
    std::to_wstring(finalMinutes) + 
    L":" + 
    (finalSeconds < 10 ? L"0" : L"") + 
    std::to_wstring(finalSeconds)
);
        gameOverTime.setPosition(
            gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTime.getGlobalBounds().width/2,
            gameOverWindow.getPosition().y + 100
        );
        
        // Save the score
        std::string difficultyStr;
        switch (gameDifficulty) {
            case Difficulty::EASY:
                difficultyStr = "Easy";
                break;
            case Difficulty::MEDIUM:
                difficultyStr = "Medium";
                break;
            case Difficulty::HARD:
                difficultyStr = "Hard";
                break;
        }
        Records::getInstance().addRecord(username, static_cast<int>(finalTime), difficultyStr, currentMap);
    }
    
    // Check trap collisions
    checkTrapCollisions();
    
    // Update damage flash
    updateDamageFlash();
    
    // Check key collection
    checkKeyCollection();
    
    // Update health bar
    updateHealthBar();
    
    // Update keys
    for (auto& key : keys) {
        key.update();
    }
    
    // Check for death
    if (playerHealth <= 0) {
        showGameOver = true;
        gameOverTitle.setString(L"Игра окончена!");
        gameOverTitle.setPosition(
            gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTitle.getGlobalBounds().width/2,
            gameOverWindow.getPosition().y + 20
        );
        gameOverTime.setString(L"Вы умерли!");
        gameOverTime.setPosition(
            gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTime.getGlobalBounds().width/2,
            gameOverWindow.getPosition().y + 100
        );
    }
    
    // Update timer text
    int seconds = static_cast<int>(elapsed.asSeconds());
    int minutes = seconds / 60;
    seconds %= 60;
    timerText.setString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
}

void Game::draw() {
    window.clear(sf::Color::Black);
    
    // Set the view for the game world
    window.setView(camera);
    
    // Draw floor tiles
    for (const auto& tile : floorTiles) {
        window.draw(tile);
    }
    
    // Draw walls
    for (const auto& wall : walls) {
        window.draw(wall);
    }
    
    // Draw door (без прозрачности)
    doorSprite.setColor(sf::Color::White); // Устанавливаем полную непрозрачность
    window.draw(doorSprite);
    
    // Draw keys
    for (const auto& key : keys) {
        key.draw(window);
    }
    
    // Draw traps
    for (const auto& trap : traps) {
        trap.draw(window);
    }
    
    // Draw the player sprite
    window.draw(playerSprite);
    
    // Draw finish (only if door is open)
    if (doorOpen) {
        finish.setFillColor(sf::Color::Transparent);
        window.draw(finish);
    }
    
    // Reset view for UI elements
    window.setView(window.getDefaultView());
    
    // Draw health bar
    window.draw(healthBarBackground);
    window.draw(healthBar);
    window.draw(healthText);
    
    // Draw timer
    window.draw(timerText);
    
    // Draw damage flash if active
    if (isFlashing) {
        window.draw(damageFlash);
    }
    
    // Draw pause menu if game is paused
    if (isPaused) {
        drawPauseMenu();
    }
    
    // Draw game over window if game is over
    if (showGameOver) {
        drawGameOverWindow();
    }
    
    window.display();
}

// Вспомогательная функция для генерации лабиринта с помощью DFS
void generateMazeDFS(std::vector<std::vector<int>>& maze, int width, int height, int x, int y, std::mt19937& rng) {
    static const int DX[4] = {0, 0, 1, -1};
    static const int DY[4] = {1, -1, 0, 0};
    std::vector<int> dirs = {0, 1, 2, 3};
    std::shuffle(dirs.begin(), dirs.end(), rng);
    for (int dir : dirs) {
        int nx = x + DX[dir]*2;
        int ny = y + DY[dir]*2;
        if (nx > 0 && nx < width-1 && ny > 0 && ny < height-1 && maze[ny][nx] == 1) {
            maze[y + DY[dir]][x + DX[dir]] = 0; // Убираем стену между
            maze[ny][nx] = 0; // Открываем клетку
            generateMazeDFS(maze, width, height, nx, ny, rng);
        }
    }
}

void Game::loadMaze() {
    // Инициализируем вектор лабиринта с заданными размерами
    maze.assign(MAZE_HEIGHT, std::vector<int>(MAZE_WIDTH, 1));

    // Генерируем лабиринт с помощью DFS
    std::random_device rd;
    std::mt19937 rng(rd());
    maze[1][1] = 0;
    generateMazeDFS(maze, MAZE_WIDTH, MAZE_HEIGHT, 1, 1, rng);
    
    // Гарантируем финиш и область вокруг него
    int centerX = MAZE_WIDTH/2;
    int centerY = MAZE_HEIGHT/2;
    for(int y = centerY-1; y <= centerY+1; y++) {
        for(int x = centerX-1; x <= centerX+1; x++) {
            if(x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT) {
                maze[y][x] = 0;
            }
        }
    }
    
    // Генерируем стены
    walls.clear();
    floorTiles.clear(); // Clear previous floor tiles
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            if (maze[y][x] == 1) {
                sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                wall.setPosition(x*CELL_SIZE, y*CELL_SIZE);
                wall.setTexture(&wallTexture); // Применяем текстуру
                wall.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE)); // Устанавливаем область текстуры для повторения
                walls.push_back(wall);
            } else { // It's a path, create a floor tile
                sf::Sprite floorTile(floorTexture);
                floorTile.setPosition(x*CELL_SIZE, y*CELL_SIZE);
                floorTile.setScale(static_cast<float>(CELL_SIZE) / floorTexture.getSize().x, static_cast<float>(CELL_SIZE) / floorTexture.getSize().y);
                floorTiles.push_back(floorTile);
            }
        }
    }
    // Reset player size and position
    float playerSize = CELL_SIZE / 2.0f;
    player.setSize(sf::Vector2f(playerSize, playerSize));
    player.setOrigin(playerSize / 2, playerSize / 2);
    player.setPosition(CELL_SIZE + CELL_SIZE/2, CELL_SIZE + CELL_SIZE/2);
    
    // Reset finish size and position
    float finishSize = CELL_SIZE / 2.0f;
    finish.setSize(sf::Vector2f(finishSize, finishSize));
    finish.setOrigin(finishSize / 2, finishSize / 2);
    finish.setPosition((MAZE_WIDTH / 2) * CELL_SIZE + CELL_SIZE/2, (MAZE_HEIGHT / 2) * CELL_SIZE + CELL_SIZE/2);
}

void Game::handlePlayerInput() {
    sf::Vector2f move(0, 0);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) move.y -= playerSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) move.y += playerSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) move.x -= playerSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) move.x += playerSpeed;
    
    if (move.x != 0 || move.y != 0) {
        // Normalize diagonal movement
        if (move.x != 0 && move.y != 0) {
            move.x *= 0.7071f; // 1/sqrt(2)
            move.y *= 0.7071f;
        }
        
        // Check collisions separately for X and Y movement
        sf::FloatRect nextPos = player.getGlobalBounds();
        nextPos.left += move.x;
        nextPos.top += move.y;
        
        bool canMoveX = true;
        bool canMoveY = true;
        
        // Test X movement
        if (move.x != 0) {
            sf::FloatRect testX = player.getGlobalBounds();
            testX.left += move.x;
            canMoveX = !checkCollision(testX);
        }
        
        // Test Y movement
        if (move.y != 0) {
            sf::FloatRect testY = player.getGlobalBounds();
            testY.top += move.y;
            canMoveY = !checkCollision(testY);
        }
        
        // Apply movement
        if (canMoveX) player.move(move.x, 0);
        if (canMoveY) player.move(0, move.y);
    }
}

bool Game::checkCollision(const sf::FloatRect& rect) {
    // Check wall collisions
    for (const auto& wall : walls) {
        if (wall.getGlobalBounds().intersects(rect)) return true;
    }
    
    // Check door collision only if door is closed
    if (!doorOpen) {
        // Create a smaller hitbox for the door
        sf::FloatRect doorHitbox = doorSprite.getGlobalBounds();
        float hitboxReduction = doorHitbox.width * 0.35f; // Увеличили хитбокс (было 0.4, стало 0.35)
        doorHitbox.left += hitboxReduction;
        doorHitbox.top += hitboxReduction;
        doorHitbox.width -= hitboxReduction * 2;
        doorHitbox.height -= hitboxReduction * 2;
        
        if (doorHitbox.intersects(rect)) return true;
    }
    
    return false;
}

void Game::updateCamera() {
    sf::Vector2f playerPos = player.getPosition();
    float minX = camera.getSize().x / 2;
    float minY = camera.getSize().y / 2;
    float maxX = MAZE_WIDTH * CELL_SIZE - camera.getSize().x / 2;
    float maxY = MAZE_HEIGHT * CELL_SIZE - camera.getSize().y / 2;
    playerPos.x = std::max(minX, std::min(playerPos.x, maxX));
    playerPos.y = std::max(minY, std::min(playerPos.y, maxY));
    sf::Vector2f currentCenter = camera.getCenter();
    sf::Vector2f newCenter = currentCenter + (playerPos - currentCenter) * 0.1f;
    camera.setCenter(newCenter);
}

bool Game::isAtFinish() const {
    // Проверяем пересечение с финишем И состояние двери
    return player.getGlobalBounds().intersects(finish.getGlobalBounds()) && doorOpen;
}

bool Game::canOpenDoor() const {
    return doorOpen;
}

void Game::updateHealthBar() {
    float healthPercentage = static_cast<float>(playerHealth) / 100.0f;
    healthBar.setSize(sf::Vector2f(200 * healthPercentage, 20));
    healthText.setString(std::to_string(playerHealth));
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        
        if (!isPaused) {
            update();
        }
        
        draw();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::Resized) {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
            camera.setSize(event.size.width, event.size.height);
        }
        
        if (showGameOver) {
            handleGameOverEvents(event);
            continue;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                togglePause();
            }
        }
        
        if (isPaused) {
            handlePauseEvents(event);
        } else {
            // Обработка движения игрока
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::W:
                        playerVelocity.y = -playerSpeed;
                        break;
                    case sf::Keyboard::S:
                        playerVelocity.y = playerSpeed;
                        break;
                    case sf::Keyboard::A:
                        playerVelocity.x = -playerSpeed;
                        break;
                    case sf::Keyboard::D:
                        playerVelocity.x = playerSpeed;
                        break;
                    default:
                        break; // Ignore other keys
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    case sf::Keyboard::W:
                    case sf::Keyboard::S:
                        playerVelocity.y = 0;
                        break;
                    case sf::Keyboard::A:
                    case sf::Keyboard::D:
                        playerVelocity.x = 0;
                        break;
                    default:
                        break; // Ignore other keys
                }
            }
        }
    }
}

void Game::initializePauseMenu() {
    // Pause menu background
    pauseMenu.setSize(sf::Vector2f(400, 500));
    pauseMenu.setPosition(200, 50);
    pauseMenu.setFillColor(sf::Color(0, 0, 0, 200));
    pauseMenu.setOutlineThickness(2);
    pauseMenu.setOutlineColor(sf::Color::White);

    // Title
    pauseTitleText.setFont(font);
    pauseTitleText.setString(L"пауза");
    pauseTitleText.setCharacterSize(40);
    pauseTitleText.setFillColor(sf::Color::White);
    pauseTitleText.setPosition(
        pauseMenu.getPosition().x + pauseMenu.getSize().x/2 - pauseTitleText.getGlobalBounds().width/2,
        pauseMenu.getPosition().y + 20
    );

    // Buttons
    float buttonWidth = 300;
    float buttonHeight = 50;
    float buttonSpacing = 20;
    float startY = pauseMenu.getPosition().y + 100;

    // Resume button
    resumeButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    resumeButton.setPosition(pauseMenu.getPosition().x + 50, startY);
    resumeButton.setFillColor(sf::Color(70, 130, 180));
    resumeButton.setOutlineThickness(2);
    resumeButton.setOutlineColor(sf::Color::White);

    resumeText.setFont(font);
    resumeText.setString(L"продолжить");
    resumeText.setCharacterSize(30);
    resumeText.setFillColor(sf::Color::White);
    resumeText.setPosition(
        resumeButton.getPosition().x + buttonWidth/2 - resumeText.getGlobalBounds().width/2,
        resumeButton.getPosition().y + buttonHeight/2 - resumeText.getGlobalBounds().height/2
    );

    // Restart button
    restartButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    restartButton.setPosition(pauseMenu.getPosition().x + 50, startY + buttonHeight + buttonSpacing);
    restartButton.setFillColor(sf::Color(70, 130, 180));
    restartButton.setOutlineThickness(2);
    restartButton.setOutlineColor(sf::Color::White);

    restartText.setFont(font);
    restartText.setString(L"Заново");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(
        restartButton.getPosition().x + buttonWidth/2 - restartText.getGlobalBounds().width/2,
        restartButton.getPosition().y + buttonHeight/2 - restartText.getGlobalBounds().height/2
    );

    // Main menu button
    mainMenuButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    mainMenuButton.setPosition(pauseMenu.getPosition().x + 50, startY + (buttonHeight + buttonSpacing) * 2);
    mainMenuButton.setFillColor(sf::Color(70, 130, 180));
    mainMenuButton.setOutlineThickness(2);
    mainMenuButton.setOutlineColor(sf::Color::White);

    mainMenuText.setFont(font);
    mainMenuText.setString(L"выход");
    mainMenuText.setCharacterSize(30);
    mainMenuText.setFillColor(sf::Color::White);
    mainMenuText.setPosition(
        mainMenuButton.getPosition().x + buttonWidth/2 - mainMenuText.getGlobalBounds().width/2,
        mainMenuButton.getPosition().y + buttonHeight/2 - mainMenuText.getGlobalBounds().height/2
    );
}

void Game::handlePauseEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (resumeButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            togglePause();
        }
        else if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            // Перезапуск игры
            window.close();
            Game game(gameDifficulty, currentMap, username, initialSelectedCharacter);
            game.run();
        }
        else if (mainMenuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            // Сохраняем текущие настройки перед возвратом в главное меню
            AuthWindow::musicVolume = 0.0f; // Устанавливаем громкость на 0
            AuthWindow::effectsVolume = effectsVolume;
            AuthWindow::currentTrack = AuthWindow::currentTrack; // Сохраняем текущий трек
            AuthWindow::isMusicPlaying = true;
            AuthWindow::isDarkTheme = AuthWindow::isDarkTheme; // Сохраняем текущую тему
            
            window.close();
            AuthWindow mainMenu;
            mainMenu.setState(WindowState::MAIN_MENU);
            mainMenu.run();
        }
    }
}

void Game::drawPauseMenu() {
    window.draw(pauseMenu);
    window.draw(pauseTitleText);
    window.draw(resumeButton);
    window.draw(resumeText);
    window.draw(restartButton);
    window.draw(restartText);
    window.draw(mainMenuButton);
    window.draw(mainMenuText);
}

void Game::togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        backgroundMusic.pause();
    } else {
        backgroundMusic.play();
    }
}

void Game::initializeGameOverWindow() {
    // Game over window background
    gameOverWindow.setSize(sf::Vector2f(400, 300));
    gameOverWindow.setPosition(window.getSize().x/2 - 200, window.getSize().y/2 - 150);
    gameOverWindow.setFillColor(sf::Color(0, 0, 0, 230));
    gameOverWindow.setOutlineThickness(2);
    gameOverWindow.setOutlineColor(sf::Color::White);

    // Title
    gameOverTitle.setFont(font);
    gameOverTitle.setCharacterSize(40);
    gameOverTitle.setFillColor(sf::Color::White);
    gameOverTitle.setPosition(
        gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTitle.getGlobalBounds().width/2,
        gameOverWindow.getPosition().y + 20
    );

    // Time text
    gameOverTime.setFont(font);
    gameOverTime.setCharacterSize(30);
    gameOverTime.setFillColor(sf::Color::White);
    gameOverTime.setPosition(
        gameOverWindow.getPosition().x + gameOverWindow.getSize().x/2 - gameOverTime.getGlobalBounds().width/2,
        gameOverWindow.getPosition().y + 100
    );

    // Restart button
    restartGameButton.setSize(sf::Vector2f(300, 50));
    restartGameButton.setPosition(gameOverWindow.getPosition().x + 50, gameOverWindow.getPosition().y + 160);
    restartGameButton.setFillColor(sf::Color(70, 130, 180));
    restartGameButton.setOutlineThickness(2);
    restartGameButton.setOutlineColor(sf::Color::White);

    restartGameText.setFont(font);
    restartGameText.setString(L"Начать заново");
    restartGameText.setCharacterSize(30);
    restartGameText.setFillColor(sf::Color::White);
    restartGameText.setPosition(
        restartGameButton.getPosition().x + restartGameButton.getSize().x/2 - restartGameText.getGlobalBounds().width/2,
        restartGameButton.getPosition().y + restartGameButton.getSize().y/2 - restartGameText.getGlobalBounds().height/2
    );

    // Main menu button
    gameOverMainMenuButton.setSize(sf::Vector2f(300, 50));
    gameOverMainMenuButton.setPosition(gameOverWindow.getPosition().x + 50, gameOverWindow.getPosition().y + 220);
    gameOverMainMenuButton.setFillColor(sf::Color(70, 130, 180));
    gameOverMainMenuButton.setOutlineThickness(2);
    gameOverMainMenuButton.setOutlineColor(sf::Color::White);

    gameOverMainMenuText.setFont(font);
    gameOverMainMenuText.setString(L"Главное меню");
    gameOverMainMenuText.setCharacterSize(30);
    gameOverMainMenuText.setFillColor(sf::Color::White);
    gameOverMainMenuText.setPosition(
        gameOverMainMenuButton.getPosition().x + gameOverMainMenuButton.getSize().x/2 - gameOverMainMenuText.getGlobalBounds().width/2,
        gameOverMainMenuButton.getPosition().y + gameOverMainMenuButton.getSize().y/2 - gameOverMainMenuText.getGlobalBounds().height/2
    );
}

void Game::drawGameOverWindow() {
    window.draw(gameOverWindow);
    window.draw(gameOverTitle);
    window.draw(gameOverTime);
    window.draw(restartGameButton);
    window.draw(restartGameText);
    window.draw(gameOverMainMenuButton);
    window.draw(gameOverMainMenuText);
}

void Game::handleGameOverEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (restartGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            window.close();
            Game game(gameDifficulty, currentMap, username, initialSelectedCharacter);
            game.run();
        }
        else if (gameOverMainMenuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            // Сохраняем текущие настройки перед возвратом в главное меню
            AuthWindow::musicVolume = 0.0f; // Устанавливаем громкость на 0
            AuthWindow::effectsVolume = effectsVolume;
            AuthWindow::currentTrack = AuthWindow::currentTrack; // Сохраняем текущий трек
            AuthWindow::isMusicPlaying = true;
            AuthWindow::isDarkTheme = AuthWindow::isDarkTheme; // Сохраняем текущую тему
            
            window.close();
            AuthWindow mainMenu;
            mainMenu.setState(WindowState::MAIN_MENU);
            mainMenu.run();
        }
    }
}