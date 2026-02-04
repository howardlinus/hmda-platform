#ifndef CPR_H
#define CPR_H

#include <vector>
#include <string>

namespace utils {

// Logistic CPR Model - parameter container
class LogisticCPRModel {
public:
    LogisticCPRModel();
    
    // Set model parameters (no JSON parsing)
    void setParameters(const std::vector<double>& coefficients, 
                      const std::vector<std::string>& feature_names,
                      double intercept = 0.0);
    
    // Predict CPR given feature values
    double predict(const std::vector<double>& features) const;
    
    // Get feature names
    const std::vector<std::string>& getFeatureNames() const { return feature_names_; }
    
    // Get coefficients
    const std::vector<double>& getCoefficients() const { return coefficients_; }
    
    // Get intercept
    double getIntercept() const { return intercept_; }
    
private:
    std::vector<double> coefficients_;
    std::vector<std::string> feature_names_;
    double intercept_;
};

} // namespace utils

#endif // CPR_H
