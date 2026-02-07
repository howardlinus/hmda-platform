#ifndef CRE_FEATUREBUILDER_H
#define CRE_FEATUREBUILDER_H

#include <vector>

namespace cre {

/**
 * @brief Context structure containing parameters for feature calculation.
 *
 * This structure holds all the necessary information for computing features
 * in a given forecasted period, including rates, benchmark values, and
 * per-period penalty points.
 *
 * Fields:
 * - period: The current forecasted period (1-indexed)
 * - annual_rate: The annual rate for the period
 * - forecast_rate_or_exog: The forecasted rate or exogenous variable
 * - benchmark_start: The starting benchmark value
 * - benchmark_running: The running/current benchmark value
 * - penalty_points: Array of per-period penalty points. Each element corresponds
 *   to a forecasted period. If not provided or shorter than required, absent
 *   periods are treated as having 0 penalty.
 *
 * Example usage:
 *   FeatureContext ctx;
 *   ctx.period = 1;
 *   ctx.annual_rate = 5.5;
 *   ctx.forecast_rate_or_exog = 5.0;
 *   ctx.benchmark_start = 100.0;
 *   ctx.benchmark_running = 105.0;
 *   ctx.penalty_points = {0.1, 0.2, 0.3};  // Penalty for periods 1, 2, 3
 */
struct FeatureContext {
    int period;
    double annual_rate;
    double forecast_rate_or_exog;
    double benchmark_start;
    double benchmark_running;
    std::vector<double> penalty_points;
};

/**
 * @brief Computes a feature vector based on the provided context.
 *
 * This function calculates various features including the refi_incentive feature,
 * which is computed as:
 *
 * refi_incentive = base_incentive - penalty_points[period-1]
 *
 * where base_incentive is calculated as:
 *   - Primary: (benchmark_running - benchmark_start)
 *   - Fallback: (annual_rate - forecast_rate_or_exog) if benchmarks are equal
 *
 * The penalty_points array is indexed by (period-1) since periods are 1-indexed
 * but arrays are 0-indexed. If penalty_points is not provided or if the period
 * exceeds the array length, a penalty of 0 is used for that period.
 *
 * @param ctx The feature context containing all necessary parameters
 * @return A vector of computed feature values
 *
 * Example:
 *   FeatureContext ctx;
 *   ctx.period = 2;
 *   ctx.annual_rate = 5.5;
 *   ctx.forecast_rate_or_exog = 5.0;
 *   ctx.benchmark_start = 100.0;
 *   ctx.benchmark_running = 105.0;
 *   ctx.penalty_points = {0.1, 0.2, 0.3};
 *   std::vector<double> features = computeFeatureVector(ctx);
 *   // features[0] will contain the refi_incentive value
 */
std::vector<double> computeFeatureVector(const FeatureContext& ctx);

} // namespace cre

#endif // CRE_FEATUREBUILDER_H
