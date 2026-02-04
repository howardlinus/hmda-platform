#include "Benchmark.h"
#include <algorithm>
#include <limits>

namespace utils {

BenchmarkLookup::BenchmarkLookup() = default;

void BenchmarkLookup::addBenchmark(int year, int quarter, const std::string& product_type, double rate) {
    BenchmarkKey key{year, quarter, product_type};
    benchmarks_[key] = rate;
}

bool BenchmarkLookup::get(int year, int quarter, const std::string& product_type, double* out) const {
    BenchmarkKey key{year, quarter, product_type};
    std::map<BenchmarkKey, double>::const_iterator it = benchmarks_.find(key);
    
    if (it != benchmarks_.end()) {
        if (out != nullptr) {
            *out = it->second;
        }
        return true;
    }
    
    return false;
}

bool BenchmarkLookup::getLatestRate(const std::string& product_type, double* out) const {
    BenchmarkKey latest_key{0, 0, ""};
    
    if (findLatestKey(product_type, &latest_key)) {
        return get(latest_key.year, latest_key.quarter, product_type, out);
    }
    
    return false;
}

bool BenchmarkLookup::getRateForYear(int year, const std::string& product_type, double* out) const {
    // Find the latest quarter for the given year
    bool found = false;
    int latest_quarter = -1;
    double result_value = 0.0;
    
    for (std::map<BenchmarkKey, double>::const_iterator it = benchmarks_.begin(); 
         it != benchmarks_.end(); ++it) {
        const BenchmarkKey& key = it->first;
        const double& value = it->second;
        
        if (key.year == year && key.product_type == product_type) {
            if (key.quarter > latest_quarter) {
                latest_quarter = key.quarter;
                result_value = value;
                found = true;
            }
        }
    }
    
    if (found && out != nullptr) {
        *out = result_value;
    }
    
    return found;
}

bool BenchmarkLookup::findLatestKey(const std::string& product_type, BenchmarkKey* out) const {
    bool found = false;
    BenchmarkKey latest_key{0, 0, ""};
    
    for (std::map<BenchmarkKey, double>::const_iterator it = benchmarks_.begin(); 
         it != benchmarks_.end(); ++it) {
        const BenchmarkKey& key = it->first;
        
        if (key.product_type == product_type) {
            if (!found || 
                key.year > latest_key.year || 
                (key.year == latest_key.year && key.quarter > latest_key.quarter)) {
                latest_key = key;
                found = true;
            }
        }
    }
    
    if (found && out != nullptr) {
        *out = latest_key;
    }
    
    return found;
}

} // namespace utils
