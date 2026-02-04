#ifndef CPR_H
#define CPR_H

#include <vector>
#include <string>
#include <map>

// LogisticCPRModel: simple container with setParameters() and predict()
class LogisticCPRModel {
public:
    LogisticCPRModel();
    
    void setParameters(const std::map<std::string, double>& params);
    double predict(const std::vector<double>& features) const;
    
private:
    std::map<std::string, double> parameters_;
    double intercept_;
};

#endif // CPR_H
