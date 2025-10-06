#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

class User {
public:
    static bool registerUser(const std::string& username, const std::string& password);
    static bool loginUser(const std::string& username, const std::string& password);
    static void saveUserData(const std::string& username, const std::string& password);
    static bool loadUserData(const std::string& username, const std::string& password);
    static std::string hashPassword(const std::string& password);
    
private:
    static const std::string USERS_FILE;
    static const std::string SALT;
}; 