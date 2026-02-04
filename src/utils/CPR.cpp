#include "utils/CPR.h"
#include <cmath>
#include <stdexcept>

namespace utils {

LogisticCPRModel::LogisticCPRModel() : intercept_(0.0) {}

void LogisticCPRModel::setParameters(const std::vector<double>& coefficients,
                                     const std::vector<std::string>& feature_names,
                                     double intercept) {
    if (coefficients.size() != feature_names.size()) {
        throw std::invalid_argument("Coefficients and feature names must have the same size");
    }
    coefficients_ = coefficients;
    feature_names_ = feature_names;
    intercept_ = intercept;
}

double LogisticCPRModel::predict(const std::vector<double>& features) const {
    if (features.size() != coefficients_.size()) {
        throw std::invalid_argument("Feature size mismatch");
    }
    
    double logit = intercept_;
    for (size_t i = 0; i < features.size(); ++i) {
        logit += coefficients_[i] * features[i];
    }
    
    // Logistic function: 1 / (1 + exp(-logit))
    double cpr = 1.0 / (1.0 + std::exp(-logit));
    
    // Clamp to [0, 1]
    if (cpr < 0.0) cpr = 0.0;
    if (cpr > 1.0) cpr = 1.0;
    
    return cpr;
}

} // namespace utils
