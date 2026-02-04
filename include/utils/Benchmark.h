#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <map>
#include <optional>

namespace utils {

class BenchmarkLookup {
public:
    BenchmarkLookup();
    
    // Add benchmark data
    void addBenchmark(int year, int quarter, const std::string& product_type, double rate);
    
    // Lookup benchmark rate - returns std::optional<double>
    std::optional<double> get(int year, int quarter, const std::string& product_type) const;
    
    // Helper methods
    std::optional<double> getLatestRate(const std::string& product_type) const;
    std::optional<double> getRateForYear(int year, const std::string& product_type) const;
    
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
    
    // Helper to find latest benchmark
    std::optional<BenchmarkKey> findLatestKey(const std::string& product_type) const;
};

} // namespace utils

#endif // BENCHMARK_H
