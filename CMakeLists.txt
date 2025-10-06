cmake_minimum_required(VERSION 3.10)
project(MazeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force libc++ for macOS to avoid ABI issues with SFML
set(CMAKE_CXX_STANDARD_LIB "libc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# macOS specific settings
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/AuthWindow.cpp
    src/Game.cpp
    src/Player.cpp
    src/User.cpp
    src/Key.cpp
    src/Trap.cpp
    src/Enemy.cpp
    src/Records.cpp
)

# Add header files
set(HEADERS
    include/AuthWindow.hpp
    include/Game.hpp
    include/Player.hpp
    include/User.hpp
    include/Key.hpp
    include/Trap.hpp
    include/Enemy.hpp
    include/Records.hpp
)

# Create executable
add_executable(MazeGame ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(MazeGame PRIVATE include)

# Link SFML libraries
target_link_libraries(MazeGame sfml-graphics sfml-window sfml-system sfml-audio)

# Copy assets folder to build directory for runtime
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}) 