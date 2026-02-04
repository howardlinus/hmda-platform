#include "Benchmark.h"
#include <iostream>
#include <cassert>

int main() {
    utils::BenchmarkLookup lookup;
    
    // Add some benchmark data
    lookup.addBenchmark(2020, 1, "fixed30", 3.25);
    lookup.addBenchmark(2020, 2, "fixed30", 3.15);
    lookup.addBenchmark(2020, 3, "fixed30", 3.10);
    lookup.addBenchmark(2021, 1, "fixed30", 3.05);
    lookup.addBenchmark(2020, 1, "fixed15", 2.75);
    lookup.addBenchmark(2020, 2, "fixed15", 2.65);
    
    // Test basic get
    double rate;
    bool found = lookup.get(2020, 1, "fixed30", &rate);
    assert(found);
    assert(rate == 3.25);
    std::cout << "Test 1 passed: get(2020, 1, fixed30) = " << rate << std::endl;
    
    // Test get with non-existent data
    found = lookup.get(2020, 4, "fixed30", &rate);
    assert(!found);
    std::cout << "Test 2 passed: get(2020, 4, fixed30) not found" << std::endl;
    
    // Test getLatestRate
    found = lookup.getLatestRate("fixed30", &rate);
    assert(found);
    assert(rate == 3.05);  // Latest is 2021 Q1
    std::cout << "Test 3 passed: getLatestRate(fixed30) = " << rate << std::endl;
    
    // Test getRateForYear
    found = lookup.getRateForYear(2020, "fixed30", &rate);
    assert(found);
    assert(rate == 3.10);  // Latest quarter in 2020 is Q3
    std::cout << "Test 4 passed: getRateForYear(2020, fixed30) = " << rate << std::endl;
    
    // Test getRateForYear with different product
    found = lookup.getRateForYear(2020, "fixed15", &rate);
    assert(found);
    assert(rate == 2.65);  // Latest quarter in 2020 for fixed15 is Q2
    std::cout << "Test 5 passed: getRateForYear(2020, fixed15) = " << rate << std::endl;
    
    // Test with non-existent product
    found = lookup.getLatestRate("adjustable", &rate);
    assert(!found);
    std::cout << "Test 6 passed: getLatestRate(adjustable) not found" << std::endl;
    
    // Test nullptr safety
    found = lookup.get(2020, 1, "fixed30", nullptr);
    assert(found);
    std::cout << "Test 7 passed: nullptr safety check" << std::endl;
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
