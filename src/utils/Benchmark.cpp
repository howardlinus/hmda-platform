#include "utils/Benchmark.h"

namespace utils {

BenchmarkLookup::BenchmarkLookup() {}

void BenchmarkLookup::setBenchmark(int year, int quarter, const std::string& product_type, double rate) {
    BenchmarkKey key{year, quarter, product_type};
    benchmarks_[key] = rate;
}

double BenchmarkLookup::getBenchmark(int year, int quarter, const std::string& product_type) const {
    BenchmarkKey key{year, quarter, product_type};
    auto it = benchmarks_.find(key);
    if (it != benchmarks_.end()) {
        return it->second;
    }
    return 0.0; // Default return if not found
}

bool BenchmarkLookup::hasBenchmark(int year, int quarter, const std::string& product_type) const {
    BenchmarkKey key{year, quarter, product_type};
    return benchmarks_.find(key) != benchmarks_.end();
}

} // namespace utils
