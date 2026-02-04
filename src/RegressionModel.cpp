#include "cre/RegressionModel.h"
#include <stdexcept>

namespace cre {

RegressionModel::RegressionModel() : intercept_(0.0) {}

void RegressionModel::setCoefficients(const std::vector<double>& coefficients,
                                     const std::vector<std::string>& feature_names,
                                     double intercept) {
    if (coefficients.size() != feature_names.size()) {
        throw std::invalid_argument("Coefficients and feature names must have the same size");
    }
    coefficients_ = coefficients;
    feature_names_ = feature_names;
    intercept_ = intercept;
}

double RegressionModel::predict(const std::vector<double>& features) const {
    if (features.size() != coefficients_.size()) {
        throw std::invalid_argument("Feature size mismatch");
    }
    
    double prediction = intercept_;
    for (size_t i = 0; i < features.size(); ++i) {
        prediction += coefficients_[i] * features[i];
    }
    
    return prediction;
}

} // namespace cre
