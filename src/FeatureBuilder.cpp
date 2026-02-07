#include "cre/FeatureBuilder.h"
#include <algorithm>
#include <cmath>

namespace cre {

std::vector<double> computeFeatureVector(const FeatureContext& ctx) {
    std::vector<double> features;
    
    // Calculate refi_incentive feature
    // Formula: (benchmark_running - benchmark_start) OR (annual_rate - forecast_rate_or_exog)
    //          MINUS penalty_points[period-1]
    
    // First, compute the base incentive value
    // Using the benchmark difference as the primary calculation
    double base_incentive = ctx.benchmark_running - ctx.benchmark_start;
    
    // Alternative calculation using rates (can be used based on context)
    // Keeping the benchmark-based calculation as the primary one
    // The "OR" in the formula suggests this could be an alternative calculation
    // depending on the use case. For this implementation, we'll use benchmark difference
    // but if benchmark values are equal, we'll use the rate difference
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
