#ifndef UTILS_CSV_H
#define UTILS_CSV_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace util {

class CSVReader {
public:
    CSVReader() = default;

    // Read CSV file and return rows
    static std::vector<std::vector<std::string>> read(const std::string& filename, char delimiter = ',') {
        std::vector<std::vector<std::string>> data;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open CSV file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            data.push_back(parseLine(line, delimiter));
        }

        return data;
    }

    // Parse a single CSV line
    static std::vector<std::string> parseLine(const std::string& line, char delimiter = ',') {
        std::vector<std::string> result;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, delimiter)) {
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
};

} // namespace util

#endif // UTILS_CSV_H
