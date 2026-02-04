#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <vector>
#include <string>

namespace cre {

class RegressionModel {
public:
    RegressionModel();
    
    // Set regression coefficients
    void setCoefficients(const std::vector<double>& coefficients,
                        const std::vector<std::string>& feature_names,
                        double intercept = 0.0);
    
    // Predict using linear regression
    double predict(const std::vector<double>& features) const;
    
    // Get feature names
    const std::vector<std::string>& getFeatureNames() const { return feature_names_; }
    
    // Get coefficients
    const std::vector<double>& getCoefficients() const { return coefficients_; }
    
private:
    std::vector<double> coefficients_;
    std::vector<std::string> feature_names_;
    double intercept_;
};

} // namespace cre

#endif // REGRESSION_MODEL_H
