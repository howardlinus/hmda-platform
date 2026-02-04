#ifndef PREPAYMENT_MODEL_H
#define PREPAYMENT_MODEL_H

#include "Mortgage.h"
#include "FeatureBuilder.h"
#include "../utils/CPR.h"
#include "../utils/Benchmark.h"

struct PrepaymentModel {
    FeatureBuilder feature_builder;
    LogisticCPRModel cpr_model;
    BenchmarkLookup benchmark_lookup;
    
    double predictCPR(const Mortgage& mortgage) const;
};

#endif // PREPAYMENT_MODEL_H
