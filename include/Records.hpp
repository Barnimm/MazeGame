#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <SFML/Graphics.hpp>

struct Record {
    std::string username;
    int score;
    std::string difficulty;
    int mapNumber;

    // Конструктор по умолчанию
    Record() : score(0), mapNumber(0) {}

    // Существующий конструктор
    Record(const std::string& name, int s, const std::string& diff, int map)
        : username(name), score(s), difficulty(diff), mapNumber(map) {}
};

class Records {
public:
    static Records& getInstance() {
        static Records instance;
        return instance;
    }

    void addRecord(const std::string& username, int score, const std::string& difficulty, int mapNumber);
    std::vector<Record> getRecords() const;
    void saveToFile() const;
    void loadFromFile();
    void clearRecords();

private:
    Records() { loadFromFile(); }
    ~Records() { saveToFile(); }

    Records(const Records&) = delete;
    Records& operator=(const Records&) = delete;

    std::vector<Record> records;
    static const std::string RECORDS_FILE;
}; 