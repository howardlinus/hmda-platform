#ifndef CRE_PREPAYMENT_MODEL_H
#define CRE_PREPAYMENT_MODEL_H

#include "cre/FeatureBuilder.h"
#include "utils/CPR.h"
#include "utils/Benchmark.h"
#include <memory>

namespace cre {

// PrepaymentModel: container for prepayment modeling components
struct PrepaymentModel {
    // Feature builder for constructing input features
    FeatureBuilder feature_builder;

    // Logistic CPR model for prepayment prediction
    util::LogisticCPRModel cpr_model;

    // Benchmark lookup for rate retrieval
    std::shared_ptr<util::BenchmarkLookup> benchmark_lookup;

    // Product type for benchmark lookup
    std::string product_type;

    PrepaymentModel() : benchmark_lookup(std::make_shared<util::BenchmarkLookup>()) {}
};

} // namespace cre

#endif // CRE_PREPAYMENT_MODEL_H
