#ifndef FEATURE_BUILDER_H
#define FEATURE_BUILDER_H

#include <vector>
#include <string>
#include <map>
#include "Mortgage.h"

// FeatureBuilder: supports only approved features
// - refi_incentive, sato, 2s10s_spread (alias: two_s_ten), debt_yield
// - vintage_YYYY (one-hot encoding, start_year < 2012 maps to 2012)
class FeatureBuilder {
public:
    FeatureBuilder();
    
    void addFeature(const std::string& feature_name);
    std::vector<double> buildFeatures(const Mortgage& mortgage) const;
    std::vector<std::string> getFeatureNames() const;
    
private:
    std::vector<std::string> feature_names_;
    
    double computeFeature(const std::string& feature_name, const Mortgage& mortgage) const;
    bool isVintageFeature(const std::string& feature_name) const;
    int extractVintageYear(const std::string& feature_name) const;
};

#endif // FEATURE_BUILDER_H
