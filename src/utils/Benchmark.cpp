#include "../include/utils/Benchmark.h"

void BenchmarkLookup::addRate(int year, int quarter, const std::string& product_type, double rate) {
    Key key = std::make_tuple(year, quarter, product_type);
    rates_[key] = rate;
}

double BenchmarkLookup::getRate(int year, int quarter, const std::string& product_type) const {
    Key key = std::make_tuple(year, quarter, product_type);
    auto it = rates_.find(key);
    if (it != rates_.end()) {
        return it->second;
    }
    return 0.0;  // default rate if not found
}

bool BenchmarkLookup::hasRate(int year, int quarter, const std::string& product_type) const {
    Key key = std::make_tuple(year, quarter, product_type);
    return rates_.find(key) != rates_.end();
}
