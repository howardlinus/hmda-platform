#ifndef CRE_REGRESSION_MODEL_H
#define CRE_REGRESSION_MODEL_H

#include <vector>
#include <string>

namespace cre {

// Simple linear regression model
class RegressionModel {
public:
    RegressionModel() = default;

    // Set coefficients and intercept
    void setParameters(const std::vector<double>& coefficients, double intercept);

    // Set feature names
    void setFeatureNames(const std::vector<std::string>& names);

    // Predict single sample
    double predict(const std::vector<double>& features) const;

    // Predict multiple samples
    std::vector<double> predict(const std::vector<std::vector<double>>& features) const;

    // Get parameters
    const std::vector<double>& getCoefficients() const { return coefficients_; }
    double getIntercept() const { return intercept_; }
    const std::vector<std::string>& getFeatureNames() const { return feature_names_; }

private:
    std::vector<double> coefficients_;
    double intercept_ = 0.0;
    std::vector<std::string> feature_names_;
};

} // namespace cre

#endif // CRE_REGRESSION_MODEL_H
