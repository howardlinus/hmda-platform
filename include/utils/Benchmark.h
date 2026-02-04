#ifndef UTILS_BENCHMARK_H
#define UTILS_BENCHMARK_H

#include <map>
#include <tuple>
#include <string>
#include <stdexcept>

namespace util {

// Key: (year, quarter, product_type)
using BenchmarkKey = std::tuple<int, int, std::string>;

class BenchmarkLookup {
public:
    BenchmarkLookup() = default;

    // Set benchmark rate for specific key
    void setBenchmark(int year, int quarter, const std::string& product_type, double rate) {
        benchmarks_[std::make_tuple(year, quarter, product_type)] = rate;
    }

    // Get benchmark rate, throws if not found
    double getBenchmark(int year, int quarter, const std::string& product_type) const {
        auto key = std::make_tuple(year, quarter, product_type);
        auto it = benchmarks_.find(key);
        if (it == benchmarks_.end()) {
            throw std::runtime_error("Benchmark not found for year=" + std::to_string(year) +
                                   ", quarter=" + std::to_string(quarter) +
                                   ", product_type=" + product_type);
        }
        return it->second;
    }

    // Check if benchmark exists
    bool hasBenchmark(int year, int quarter, const std::string& product_type) const {
        auto key = std::make_tuple(year, quarter, product_type);
        return benchmarks_.find(key) != benchmarks_.end();
    }

    // Get number of benchmarks stored
    size_t size() const {
        return benchmarks_.size();
    }

private:
    std::map<BenchmarkKey, double> benchmarks_;
};

} // namespace util

#endif // UTILS_BENCHMARK_H
