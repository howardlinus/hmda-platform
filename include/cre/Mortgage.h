#ifndef MORTGAGE_H
#define MORTGAGE_H

#include <string>

struct Mortgage {
    double balance;
    double rate;
    int start_year;
    int start_quarter;
    std::string product_type;
    double current_benchmark_rate;
    double sato;
    double two_s_ten_spread;
    double debt_yield;
    
    Mortgage();
};

#endif // MORTGAGE_H
