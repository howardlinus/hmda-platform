#include "cre/RegressionModel.h"

namespace cre {

void RegressionModel::setParameters(const std::vector<double>& coefficients, 
                                    double intercept) {
    coefficients_ = coefficients;
    intercept_ = intercept;
}

void RegressionModel::setFeatureNames(const std::vector<std::string>& names) {
    feature_names_ = names;
}

double RegressionModel::predict(const std::vector<double>& features) const {
    double result = intercept_;
    
    size_t n = std::min(features.size(), coefficients_.size());
    for (size_t i = 0; i < n; ++i) {
        result += coefficients_[i] * features[i];
    }
    
    return result;
}

std::vector<double> RegressionModel::predict(
    const std::vector<std::vector<double>>& features) const {
    
    std::vector<double> results;
    results.reserve(features.size());
    
    for (const auto& feature_vec : features) {
        results.push_back(predict(feature_vec));
    }
    
    return results;
}

} // namespace cre
