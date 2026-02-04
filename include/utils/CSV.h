#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>

class CSVReader {
public:
    static std::vector<std::vector<std::string>> read(const std::string& filename);
    static std::vector<std::string> parseLine(const std::string& line);
};

#endif // CSV_H
