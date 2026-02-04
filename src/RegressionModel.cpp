#include "../include/cre/RegressionModel.h"

RegressionModel::RegressionModel() : intercept_(0.0) {}

void RegressionModel::setCoefficients(const std::map<std::string, double>& coefficients) {
    coefficients_.clear();
    for (const auto& pair : coefficients) {
        coefficients_.push_back(pair.second);
    }
}

void RegressionModel::setIntercept(double intercept) {
    intercept_ = intercept;
}

double RegressionModel::predict(const std::vector<double>& features) const {
    double result = intercept_;
    
    for (size_t i = 0; i < features.size() && i < coefficients_.size(); ++i) {
        result += coefficients_[i] * features[i];
    }
    
    return result;
}
