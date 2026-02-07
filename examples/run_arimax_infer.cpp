#include <iostream>
#include <vector>
#include "cre/FeatureBuilder.h"

/**
 * @brief Example program demonstrating ARIMAX inference with penalty points.
 *
 * This example shows how to:
 * 1. Set up user input for per-period penalty points
 * 2. Create a FeatureContext with the penalty points
 * 3. Compute feature vectors for multiple periods
 * 4. Display the computed refi_incentive values
 */
int main() {
    std::cout << "=== ARIMAX Inference with Penalty Points Example ===" << std::endl;
    std::cout << std::endl;
    
    // Step 1: Define user input for penalty points
    // Each element corresponds to a forecasted period
    std::vector<double> user_penalty_points = {0.1, 0.2, 0.15, 0.25, 0.3};
    
    std::cout << "User-defined penalty points per period:" << std::endl;
    for (size_t i = 0; i < user_penalty_points.size(); ++i) {
        std::cout << "  Period " << (i + 1) << ": " << user_penalty_points[i] << std::endl;
    }
    std::cout << std::endl;
    
    // Step 2: Define forecast parameters
    // These would typically come from a model or user input
    double benchmark_start = 100.0;
    std::vector<double> benchmark_running_values = {102.5, 105.0, 103.5, 106.0, 108.0};
    std::vector<double> annual_rates = {5.5, 5.6, 5.4, 5.7, 5.8};
    std::vector<double> forecast_rates = {5.0, 5.1, 5.0, 5.2, 5.3};
    
    // Step 3: Compute features for each period
    std::cout << "Computing refi_incentive for each period:" << std::endl;
    std::cout << std::endl;
    
    for (size_t period_idx = 0; period_idx < benchmark_running_values.size(); ++period_idx) {
        // Create FeatureContext for this period
        cre::FeatureContext ctx;
        ctx.period = static_cast<int>(period_idx + 1); // Periods are 1-indexed
        ctx.annual_rate = annual_rates[period_idx];
        ctx.forecast_rate_or_exog = forecast_rates[period_idx];
        ctx.benchmark_start = benchmark_start;
        ctx.benchmark_running = benchmark_running_values[period_idx];
        ctx.penalty_points = user_penalty_points; // Pass the penalty points array
        
        // Compute feature vector
        std::vector<double> features = cre::computeFeatureVector(ctx);
        
        // Display results
        std::cout << "Period " << ctx.period << ":" << std::endl;
        std::cout << "  Benchmark: " << ctx.benchmark_running << " (start: " << ctx.benchmark_start << ")" << std::endl;
        std::cout << "  Benchmark difference: " << (ctx.benchmark_running - ctx.benchmark_start) << std::endl;
        std::cout << "  Annual rate: " << ctx.annual_rate << std::endl;
        std::cout << "  Forecast rate: " << ctx.forecast_rate_or_exog << std::endl;
        std::cout << "  Rate difference: " << (ctx.annual_rate - ctx.forecast_rate_or_exog) << std::endl;
        
        // Get penalty for this period (0-indexed array access)
        double penalty = 0.0;
        if (period_idx < user_penalty_points.size()) {
            penalty = user_penalty_points[period_idx];
        }
        std::cout << "  Penalty: " << penalty << std::endl;
        std::cout << "  Computed refi_incentive: " << features[0] << std::endl;
        std::cout << std::endl;
    }
    
    // Step 4: Demonstrate handling of missing penalty points
    std::cout << "=== Example with insufficient penalty points ===" << std::endl;
    std::cout << std::endl;
    
    // Create a shorter penalty points array
    std::vector<double> short_penalty_points = {0.1, 0.2};
    
    // Try to compute for period 3 (beyond the penalty array)
    cre::FeatureContext ctx_missing;
    ctx_missing.period = 3;
    ctx_missing.annual_rate = 5.5;
    ctx_missing.forecast_rate_or_exog = 5.0;
    ctx_missing.benchmark_start = 100.0;
    ctx_missing.benchmark_running = 103.0;
    ctx_missing.penalty_points = short_penalty_points;
    
    std::vector<double> features_missing = cre::computeFeatureVector(ctx_missing);
    
    std::cout << "Period " << ctx_missing.period << " with only 2 penalty values:" << std::endl;
    std::cout << "  Benchmark difference: " << (ctx_missing.benchmark_running - ctx_missing.benchmark_start) << std::endl;
    std::cout << "  Penalty (defaults to 0): 0.0" << std::endl;
    std::cout << "  Computed refi_incentive: " << features_missing[0] << std::endl;
    std::cout << std::endl;
    
    // Step 5: Demonstrate with empty penalty points
    std::cout << "=== Example with no penalty points ===" << std::endl;
    std::cout << std::endl;
    
    cre::FeatureContext ctx_no_penalty;
    ctx_no_penalty.period = 1;
    ctx_no_penalty.annual_rate = 5.5;
    ctx_no_penalty.forecast_rate_or_exog = 5.0;
    ctx_no_penalty.benchmark_start = 100.0;
    ctx_no_penalty.benchmark_running = 102.5;
    ctx_no_penalty.penalty_points = {}; // Empty penalty array
    
    std::vector<double> features_no_penalty = cre::computeFeatureVector(ctx_no_penalty);
    
    std::cout << "Period " << ctx_no_penalty.period << " with empty penalty array:" << std::endl;
    std::cout << "  Benchmark difference: " << (ctx_no_penalty.benchmark_running - ctx_no_penalty.benchmark_start) << std::endl;
    std::cout << "  Penalty (defaults to 0): 0.0" << std::endl;
    std::cout << "  Computed refi_incentive: " << features_no_penalty[0] << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Example completed successfully ===" << std::endl;
    
    return 0;
}
