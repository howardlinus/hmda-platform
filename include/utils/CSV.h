#ifndef CSV_H
#define CSV_H

#include <vector>
#include <string>
#include <map>

namespace utils {

class CSVReader {
public:
    CSVReader();
    
    // Read CSV file and return rows
    std::vector<std::vector<std::string>> read(const std::string& filename);
    
    // Read CSV with header and return as map
    std::vector<std::map<std::string, std::string>> readWithHeader(const std::string& filename);
    
private:
    std::vector<std::string> parseLine(const std::string& line);
};

} // namespace utils

#endif // CSV_H
