#ifndef UTILS_CPR_H
#define UTILS_CPR_H

#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>

namespace util {

// LogisticCPRModel: parameter container with setParameters and predict
class LogisticCPRModel {
public:
    LogisticCPRModel() = default;

    // Set model parameters (coefficients and intercept)
    void setParameters(const std::vector<double>& coefficients, double intercept) {
        coefficients_ = coefficients;
        intercept_ = intercept;
    }

    // Predict CPR given features
    // Returns probability in [0, 1] range
    // Throws std::runtime_error if feature dimension mismatch
    double predict(const std::vector<double>& features) const {
        if (features.size() != coefficients_.size()) {
            std::ostringstream oss;
            oss << "Feature size mismatch: expected " << coefficients_.size() 
                << ", got " << features.size();
            throw std::runtime_error(oss.str());
        }

        double logit = intercept_;
        for (size_t i = 0; i < features.size(); ++i) {
            logit += coefficients_[i] * features[i];
        }

        // Apply sigmoid function
        return 1.0 / (1.0 + std::exp(-logit));
    }

    // Get coefficients
    const std::vector<double>& getCoefficients() const {
        return coefficients_;
    }

    // Get intercept
    double getIntercept() const {
        return intercept_;
    }

private:
    std::vector<double> coefficients_;
    double intercept_ = 0.0;
};

} // namespace util

#endif // UTILS_CPR_H
