#include "../include/utils/CPR.h"
#include <cmath>

LogisticCPRModel::LogisticCPRModel() : intercept_(0.0) {}

void LogisticCPRModel::setParameters(const std::map<std::string, double>& params) {
    parameters_ = params;
    
    // Extract intercept if present
    auto it = parameters_.find("intercept");
    if (it != parameters_.end()) {
        intercept_ = it->second;
    }
}

double LogisticCPRModel::predict(const std::vector<double>& features) const {
    double logit = intercept_;
    
    // Add feature contributions
    size_t idx = 0;
    for (const auto& pair : parameters_) {
        if (pair.first != "intercept" && idx < features.size()) {
            logit += pair.second * features[idx];
            idx++;
        }
    }
    
    // Apply logistic function
    double cpr = 1.0 / (1.0 + std::exp(-logit));
    return cpr;
}
