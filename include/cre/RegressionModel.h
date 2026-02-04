#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <vector>
#include <map>
#include <string>

class RegressionModel {
public:
    RegressionModel();
    
    void setCoefficients(const std::map<std::string, double>& coefficients);
    void setIntercept(double intercept);
    
    double predict(const std::vector<double>& features) const;
    
private:
    std::vector<double> coefficients_;
    double intercept_;
};

#endif // REGRESSION_MODEL_H
