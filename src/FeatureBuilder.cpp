#include "cre/FeatureBuilder.h"
#include <cmath>
#include <stdexcept>

namespace cre {

FeatureBuilder::FeatureBuilder() : benchmark_lookup_(nullptr) {}

void FeatureBuilder::setBenchmarkLookup(const utils::BenchmarkLookup* lookup) {
    benchmark_lookup_ = lookup;
}

int FeatureBuilder::normalizeVintageYear(int year) const {
    return (year < 2012) ? 2012 : year;
}

double FeatureBuilder::calculateRefiIncentive(double current_rate, double benchmark_rate) const {
    if (benchmark_rate <= 0.0) return 0.0;
    return (benchmark_rate - current_rate) / benchmark_rate;
}

double FeatureBuilder::calculateSATO(double scheduled_balance, double original_balance) const {
    if (original_balance <= 0.0) return 0.0;
    return scheduled_balance / original_balance;
}

double FeatureBuilder::calculate2s10sSpread(double rate_2y, double rate_10y) const {
    return rate_10y - rate_2y;
}

double FeatureBuilder::calculateDebtYield(double noi, double balance) const {
    if (balance <= 0.0) return 0.0;
    return noi / balance;
}

double FeatureBuilder::getVintageOneHot(int loan_year, int target_year) const {
    int normalized_loan_year = normalizeVintageYear(loan_year);
    int normalized_target_year = normalizeVintageYear(target_year);
    return (normalized_loan_year == normalized_target_year) ? 1.0 : 0.0;
}

std::vector<double> FeatureBuilder::buildFeatures(
    const std::map<std::string, double>& raw_features,
    const std::vector<std::string>& feature_order) const {
    
    std::vector<double> features;
    
    for (const auto& feature_name : feature_order) {
        double value = 0.0;
        
        if (feature_name == "refi_incentive") {
            auto it_rate = raw_features.find("current_rate");
            auto it_bench = raw_features.find("benchmark_rate");
            if (it_rate != raw_features.end() && it_bench != raw_features.end()) {
                value = calculateRefiIncentive(it_rate->second, it_bench->second);
            }
        } else if (feature_name == "sato") {
            auto it_sched = raw_features.find("scheduled_balance");
            auto it_orig = raw_features.find("original_balance");
            if (it_sched != raw_features.end() && it_orig != raw_features.end()) {
                value = calculateSATO(it_sched->second, it_orig->second);
            }
        } else if (feature_name == "2s10s_spread" || feature_name == "two_s_ten") {
            auto it_2y = raw_features.find("rate_2y");
            auto it_10y = raw_features.find("rate_10y");
            if (it_2y != raw_features.end() && it_10y != raw_features.end()) {
                value = calculate2s10sSpread(it_2y->second, it_10y->second);
            }
        } else if (feature_name == "debt_yield") {
            auto it_noi = raw_features.find("noi");
            auto it_bal = raw_features.find("balance");
            if (it_noi != raw_features.end() && it_bal != raw_features.end()) {
                value = calculateDebtYield(it_noi->second, it_bal->second);
            }
        } else if (feature_name.substr(0, 8) == "vintage_") {
            // Parse vintage_YYYY
            try {
                int target_year = std::stoi(feature_name.substr(8));
                auto it_year = raw_features.find("loan_year");
                if (it_year != raw_features.end()) {
                    value = getVintageOneHot(static_cast<int>(it_year->second), target_year);
                }
            } catch (...) {
                value = 0.0;
            }
        } else {
            // Direct lookup
            auto it = raw_features.find(feature_name);
            if (it != raw_features.end()) {
                value = it->second;
            }
        }
        
        features.push_back(value);
    }
    
    return features;
}

} // namespace cre
