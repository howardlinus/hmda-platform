#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <map>
#include <tuple>

// BenchmarkLookup: key = (year, quarter, product_type)
class BenchmarkLookup {
public:
    using Key = std::tuple<int, int, std::string>;
    
    void addRate(int year, int quarter, const std::string& product_type, double rate);
    double getRate(int year, int quarter, const std::string& product_type) const;
    bool hasRate(int year, int quarter, const std::string& product_type) const;
    
private:
    std::map<Key, double> rates_;
};

#endif // BENCHMARK_H
