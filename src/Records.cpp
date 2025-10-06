#include "Records.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

const std::string Records::RECORDS_FILE = "records.txt";

void Records::addRecord(const std::string& username, int score, const std::string& difficulty, int mapNumber) {
    // Ищем существующую запись для этого игрока
    auto it = std::find_if(records.begin(), records.end(),
        [&username](const Record& r) { return r.username == username; });
    
    if (it != records.end()) {
        // Если нашли существующую запись, обновляем её только если новое время лучше (меньше)
        if (score < it->score) {
            it->score = score;
            it->difficulty = difficulty;
            it->mapNumber = mapNumber;
        }
    } else {
        // Если записи для этого игрока нет, добавляем новую
        records.push_back(Record(username, score, difficulty, mapNumber));
    }
    
    // Сортируем записи по возрастанию времени (лучшее время первым)
    std::sort(records.begin(), records.end(),
        [](const Record& a, const Record& b) { return a.score < b.score; });
    
    // Оставляем только топ-10 результатов
    if (records.size() > 10) {
        records.resize(10);
    }
    
    // Сохраняем изменения в файл
    saveToFile();
}

std::vector<Record> Records::getRecords() const {
    return records;
}

void Records::saveToFile() const {
    std::ofstream file(RECORDS_FILE);
    if (!file.is_open()) {
        std::cerr << "Failed to open records file for writing" << std::endl;
        return;
    }

    for (const auto& record : records) {
        file << record.username << ","
             << record.score << ","
             << record.difficulty << ","
             << record.mapNumber << "\n";
    }
}

void Records::loadFromFile() {
    std::ifstream file(RECORDS_FILE);
    if (!file.is_open()) {
        std::cerr << "Failed to open records file for reading" << std::endl;
        return;
    }

    records.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::string username, difficulty;
        int score, mapNumber;
        
        size_t pos1 = line.find(',');
        if (pos1 == std::string::npos) continue;
        username = line.substr(0, pos1);
        
        size_t pos2 = line.find(',', pos1 + 1);
        if (pos2 == std::string::npos) continue;
        score = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
        
        size_t pos3 = line.find(',', pos2 + 1);
        if (pos3 == std::string::npos) continue;
        difficulty = line.substr(pos2 + 1, pos3 - pos2 - 1);
        
        mapNumber = std::stoi(line.substr(pos3 + 1));
        
        records.emplace_back(username, score, difficulty, mapNumber);
    }
}

void Records::clearRecords() {
    records.clear();
    saveToFile();
} 