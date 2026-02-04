#ifndef CRE_FEATURE_BUILDER_H
#define CRE_FEATURE_BUILDER_H

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>

namespace cre {

// FeatureBuilder: constructs features for prepayment models
// Supports only: refi_incentive, sato, 2s10s_spread (alias two_s_ten), debt_yield, vintage_YYYY
class FeatureBuilder {
public:
    struct InputData {
        double coupon_rate = 0.0;
        double benchmark_rate = 0.0;
        double seasonality = 0.0;
        double spread_2s10s = 0.0;
        double debt_yield = 0.0;
        int start_year = 2012;
    };

    FeatureBuilder() = default;

    // Add feature to be computed
    void addFeature(const std::string& feature_name) {
        if (!isValidFeature(feature_name)) {
            throw std::runtime_error("Invalid feature: " + feature_name);
        }
        if (std::find(features_.begin(), features_.end(), feature_name) == features_.end()) {
            features_.push_back(feature_name);
        }
    }

    // Build features from input data
    std::vector<double> build(const InputData& data) const {
        std::vector<double> result;
        result.reserve(features_.size());

        for (const auto& feature : features_) {
            result.push_back(computeFeature(feature, data));
        }

        return result;
    }

    // Get feature names
    const std::vector<std::string>& getFeatureNames() const {
        return features_;
    }

    // Clear all features
    void clear() {
        features_.clear();
    }

private:
    std::vector<std::string> features_;

    // Check if feature name is valid
    bool isValidFeature(const std::string& name) const {
        if (name == "refi_incentive" || name == "sato" || 
            name == "2s10s_spread" || name == "two_s_ten" ||
            name == "debt_yield") {
            return true;
        }
        
        // Check for vintage_YYYY pattern
        if (name.length() >= 12 && name.substr(0, 8) == "vintage_") {
            std::string year_str = name.substr(8);
            try {
                int year = std::stoi(year_str);
                return year >= 2012 && year <= 2100;
            } catch (...) {
                return false;
            }
        }

        return false;
    }

    // Compute individual feature
    double computeFeature(const std::string& feature, const InputData& data) const {
        if (feature == "refi_incentive") {
            // Refi incentive: coupon_rate - benchmark_rate
            return data.coupon_rate - data.benchmark_rate;
        } else if (feature == "sato") {
            // Seasonality-adjusted turnover
            return data.seasonality;
        } else if (feature == "2s10s_spread" || feature == "two_s_ten") {
            // 2s-10s spread
            return data.spread_2s10s;
        } else if (feature == "debt_yield") {
            return data.debt_yield;
        } else if (feature.substr(0, 8) == "vintage_") {
            // One-hot encoding for vintage year
            // Map any year < 2012 to 2012
            int vintage_year = std::max(data.start_year, 2012);
            int feature_year = std::stoi(feature.substr(8));
            return (vintage_year == feature_year) ? 1.0 : 0.0;
        }

        return 0.0;
    }
};

} // namespace cre

#endif // CRE_FEATURE_BUILDER_H
