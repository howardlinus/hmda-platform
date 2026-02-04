#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <map>

namespace utils {

struct BenchmarkRate {
    double rate;
};

class BenchmarkLookup {
public:
    BenchmarkLookup();
    
    // Set benchmark rate for a given year, quarter, and product_type
    void setBenchmark(int year, int quarter, const std::string& product_type, double rate);
    
    // Get benchmark rate for a given year, quarter, and product_type
    double getBenchmark(int year, int quarter, const std::string& product_type) const;
    
    // Check if benchmark exists
    bool hasBenchmark(int year, int quarter, const std::string& product_type) const;
    
private:
    struct BenchmarkKey {
        int year;
        int quarter;
        std::string product_type;
        
        bool operator<(const BenchmarkKey& other) const {
            if (year != other.year) return year < other.year;
            if (quarter != other.quarter) return quarter < other.quarter;
            return product_type < other.product_type;
        }
    };
    
    std::map<BenchmarkKey, double> benchmarks_;
};

} // namespace utils

#endif // BENCHMARK_H
