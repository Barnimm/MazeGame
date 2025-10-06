# Maze Game

A 2D maze game built with C++ and SFML.

## Requirements

- C++ compiler with C++17 support
- CMake 3.10 or higher
- SFML 2.5 or higher

## Installation

### macOS

1. Install SFML using Homebrew:
```bash
brew install sfml
```

2. Clone the repository:
```bash
git clone <repository-url>
cd maze-game
```

3. Create a build directory and build the project:
```bash
mkdir build
cd build
cmake ..
make
```

## Running the Game

After building, run the game from the build directory:
```bash
./MazeGame
```

## Game Features

- User authentication system with registration and login
- Binary file-based user data storage
- Beautiful UI with custom background
- Secure password handling

## Project Structure

- `src/` - Source files
- `include/` - Header files
- `assets/` - Game resources (images, fonts, etc.)
- `build/` - Build directory (created during build)

## Controls

- Mouse for navigation and interaction
- Keyboard for text input during registration and login 

enum class GameState {
    MAIN_MENU,
    CHARACTER_SELECT,
    MAP_SELECT,
    PLAYING,
    GAME_OVER,
    LEVEL_COMPLETE,
    SETTINGS
}; 

sf::RectangleShape redCubeButton, greenCubeButton, blueCubeButton;
sf::Text redCubeText, greenCubeText, blueCubeText;
void renderCharacterSelect();
void handleCharacterSelectEvents(const sf::Event& event); 