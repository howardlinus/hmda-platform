#include "cre/FeatureBuilder.h"
#include <algorithm>
#include <cmath>

namespace cre {

std::vector<double> computeFeatureVector(const FeatureContext& ctx) {
    std::vector<double> features;
    
    // Calculate refi_incentive feature
    // Formula: base_incentive - penalty_points[period-1]
    // where base_incentive is:
    //   - Primary: (benchmark_running - benchmark_start)
    //   - Fallback: (annual_rate - forecast_rate_or_exog) if benchmarks are equal
    
    // Compute the base incentive value
    // Using benchmark difference as the primary calculation method
    double base_incentive = ctx.benchmark_running - ctx.benchmark_start;
    
    // If benchmark values are effectively equal (within numerical tolerance),
    // use the rate difference as an alternative calculation
    if (std::abs(ctx.benchmark_running - ctx.benchmark_start) < 1e-9) {
        base_incentive = ctx.annual_rate - ctx.forecast_rate_or_exog;
    }
    
    // Get the penalty for the current period
    // Periods are 1-indexed, so we need to use (period - 1) for array indexing
    double penalty = 0.0;
    if (!ctx.penalty_points.empty() && ctx.period > 0) {
        int penalty_index = ctx.period - 1;
        if (penalty_index < static_cast<int>(ctx.penalty_points.size())) {
            penalty = ctx.penalty_points[penalty_index];
        }
        // If penalty_index is out of bounds, penalty remains 0.0
    }
    
    // Calculate final refi_incentive by subtracting penalty
    double refi_incentive = base_incentive - penalty;
    
    // Add refi_incentive as the first feature
    features.push_back(refi_incentive);
    
    // Additional features can be added here as needed
    // For now, we just return the refi_incentive feature
    
    return features;
}

} // namespace cre
