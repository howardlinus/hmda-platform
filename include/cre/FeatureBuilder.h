#ifndef FEATURE_BUILDER_H
#define FEATURE_BUILDER_H

#include <vector>
#include <string>
#include <map>
#include "utils/Benchmark.h"

namespace cre {

// Feature builder supporting only specific features:
// - refi_incentive
// - sato
// - 2s10s_spread (alias: two_s_ten)
// - debt_yield
// - vintage_YYYY (one-hot encoding, maps <2012 to 2012)
class FeatureBuilder {
public:
    FeatureBuilder();
    
    // Set benchmark lookup for refi_incentive calculation
    void setBenchmarkLookup(const utils::BenchmarkLookup* lookup);
    
    // Build features for a single loan
    // Inputs: map of raw feature names to values
    // Returns: vector of feature values in order specified by feature_order
    std::vector<double> buildFeatures(
        const std::map<std::string, double>& raw_features,
        const std::vector<std::string>& feature_order) const;
    
    // Calculate individual features
    double calculateRefiIncentive(double current_rate, double benchmark_rate) const;
    double calculateSATO(double scheduled_balance, double original_balance) const;
    double calculate2s10sSpread(double rate_2y, double rate_10y) const;
    double calculateDebtYield(double noi, double balance) const;
    
    // Get vintage one-hot encoding (returns 1.0 if matches, 0.0 otherwise)
    // Maps any year < 2012 to 2012
    double getVintageOneHot(int loan_year, int target_year) const;
    
private:
    const utils::BenchmarkLookup* benchmark_lookup_;
    
    int normalizeVintageYear(int year) const;
};

} // namespace cre

#endif // FEATURE_BUILDER_H
