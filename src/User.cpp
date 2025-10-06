#include "User.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

const std::string User::USERS_FILE = "users.txt";
const std::string User::SALT = "game_salt";

std::string User::hashPassword(const std::string& password) {
    std::string salted = password + SALT;
    std::string hashed;
    for (char c : salted) {
        hashed += std::to_string(static_cast<int>(c * 31));
    }
    return hashed;
}

void User::saveUserData(const std::string& username, const std::string& password) {
    std::ofstream file(USERS_FILE, std::ios::app);
    if (file.is_open()) {
        file << username << ":" << hashPassword(password) << "\n";
        file.close();
        std::cout << "User data saved: " << username << std::endl;
    } else {
        std::cerr << "Failed to open users file for writing: " << USERS_FILE << std::endl;
    }
}

bool User::loadUserData(const std::string& username, const std::string& password) {
    std::ifstream file(USERS_FILE);
    if (!file.is_open()) {
        std::cerr << "Failed to open users file for reading: " << USERS_FILE << std::endl;
        return false;
    }

    std::string line;
    std::string hashedPassword = hashPassword(password);
    
    std::cout << "Looking for user: " << username << std::endl;
    
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string storedUsername = line.substr(0, pos);
            std::string storedPassword = line.substr(pos + 1);
            
            std::cout << "Found user in file: " << storedUsername << std::endl;
            
            if (storedUsername == username && storedPassword == hashedPassword) {
                file.close();
                std::cout << "Login successful for: " << username << std::endl;
                return true;
            }
        }
    }
    
    file.close();
    std::cout << "Login failed for: " << username << std::endl;
    return false;
}

bool User::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cout << "Registration failed: Empty username or password" << std::endl;
        return false;
    }
    
    std::ifstream file(USERS_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos && line.substr(0, pos) == username) {
                file.close();
                std::cout << "Registration failed: Username already exists" << std::endl;
                return false;
            }
        }
        file.close();
    } else {
        std::cout << "Users file does not exist, will create new file" << std::endl;
    }
    
    saveUserData(username, password);
    std::cout << "Registration successful for: " << username << std::endl;
    return true;
}

bool User::loginUser(const std::string& username, const std::string& password) {
    return loadUserData(username, password);
} 