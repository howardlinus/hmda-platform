#include "utils/CSV.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace utils {

CSVReader::CSVReader() {}

std::vector<std::string> CSVReader::parseLine(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool in_quotes = false;
    
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    
    result.push_back(cell);
    return result;
}

std::vector<std::vector<std::string>> CSVReader::read(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::vector<std::vector<std::string>> rows;
    std::string line;
    
    while (std::getline(file, line)) {
        if (!line.empty()) {
            rows.push_back(parseLine(line));
        }
    }
    
    return rows;
}

std::vector<std::map<std::string, std::string>> CSVReader::readWithHeader(const std::string& filename) {
    std::vector<std::vector<std::string>> rows = read(filename);
    
    if (rows.empty()) {
        return {};
    }
    
    std::vector<std::string> header = rows[0];
    std::vector<std::map<std::string, std::string>> result;
    
    for (size_t i = 1; i < rows.size(); ++i) {
        std::map<std::string, std::string> row_map;
        for (size_t j = 0; j < header.size() && j < rows[i].size(); ++j) {
            row_map[header[j]] = rows[i][j];
        }
        result.push_back(row_map);
    }
    
    return result;
}

} // namespace utils
