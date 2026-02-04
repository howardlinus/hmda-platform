#include "../include/utils/CSV.h"
#include <fstream>
#include <sstream>

std::vector<std::vector<std::string>> CSVReader::read(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return data;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        data.push_back(parseLine(line));
    }
    
    file.close();
    return data;
}

std::vector<std::string> CSVReader::parseLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string cell;
    
    while (std::getline(ss, cell, ',')) {
        // Trim whitespace
        size_t start = cell.find_first_not_of(" \t\r\n");
        size_t end = cell.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            result.push_back(cell.substr(start, end - start + 1));
        } else {
            result.push_back("");
        }
    }
    
    return result;
}
