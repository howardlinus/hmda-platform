#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <map>

namespace utils {

class BenchmarkLookup {
public:
    BenchmarkLookup();
    
    // Add benchmark data
    void addBenchmark(int year, int quarter, const std::string& product_type, double rate);
    
    // Lookup benchmark rate - returns true if found, false otherwise
    // The value is written to *out if found
    bool get(int year, int quarter, const std::string& product_type, double* out) const;
    
    // Helper methods
    bool getLatestRate(const std::string& product_type, double* out) const;
    bool getRateForYear(int year, const std::string& product_type, double* out) const;
    
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
    
    // Helper to find latest benchmark - returns true if found, false otherwise
    bool findLatestKey(const std::string& product_type, BenchmarkKey* out) const;
};

} // namespace utils

#endif // BENCHMARK_H
