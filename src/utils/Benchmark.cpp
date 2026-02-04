#include "Benchmark.h"
#include <algorithm>
#include <limits>

namespace utils {

BenchmarkLookup::BenchmarkLookup() = default;

void BenchmarkLookup::addBenchmark(int year, int quarter, const std::string& product_type, double rate) {
    BenchmarkKey key{year, quarter, product_type};
    benchmarks_[key] = rate;
}

std::optional<double> BenchmarkLookup::get(int year, int quarter, const std::string& product_type) const {
    BenchmarkKey key{year, quarter, product_type};
    auto it = benchmarks_.find(key);
    
    if (it != benchmarks_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::optional<double> BenchmarkLookup::getLatestRate(const std::string& product_type) const {
    auto latest_key = findLatestKey(product_type);
    
    if (latest_key.has_value()) {
        return get(latest_key->year, latest_key->quarter, product_type);
    }
    
    return std::nullopt;
}

std::optional<double> BenchmarkLookup::getRateForYear(int year, const std::string& product_type) const {
    // Find the latest quarter for the given year
    std::optional<double> result;
    int latest_quarter = -1;
    
    for (const auto& [key, value] : benchmarks_) {
        if (key.year == year && key.product_type == product_type) {
            if (key.quarter > latest_quarter) {
                latest_quarter = key.quarter;
                result = value;
            }
        }
    }
    
    return result;
}

std::optional<BenchmarkLookup::BenchmarkKey> BenchmarkLookup::findLatestKey(const std::string& product_type) const {
    std::optional<BenchmarkKey> latest;
    
    for (const auto& [key, value] : benchmarks_) {
        if (key.product_type == product_type) {
            if (!latest.has_value() || 
                key.year > latest->year || 
                (key.year == latest->year && key.quarter > latest->quarter)) {
                latest = key;
            }
        }
    }
    
    return latest;
}

} // namespace utils
