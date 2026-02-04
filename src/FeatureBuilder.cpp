#include "../include/cre/FeatureBuilder.h"
#include <algorithm>
#include <stdexcept>

FeatureBuilder::FeatureBuilder() {}

void FeatureBuilder::addFeature(const std::string& feature_name) {
    feature_names_.push_back(feature_name);
}

std::vector<double> FeatureBuilder::buildFeatures(const Mortgage& mortgage) const {
    std::vector<double> features;
    
    for (const auto& feature_name : feature_names_) {
        features.push_back(computeFeature(feature_name, mortgage));
    }
    
    return features;
}

std::vector<std::string> FeatureBuilder::getFeatureNames() const {
    return feature_names_;
}

double FeatureBuilder::computeFeature(const std::string& feature_name, const Mortgage& mortgage) const {
    // Handle approved features
    if (feature_name == "refi_incentive") {
        // Refinance incentive: spread between current mortgage rate and benchmark
        return mortgage.rate - mortgage.current_benchmark_rate;
    }
    else if (feature_name == "sato") {
        return mortgage.sato;
    }
    else if (feature_name == "2s10s_spread" || feature_name == "two_s_ten") {
        return mortgage.two_s_ten_spread;
    }
    else if (feature_name == "debt_yield") {
        return mortgage.debt_yield;
    }
    else if (isVintageFeature(feature_name)) {
        // Vintage one-hot encoding
        int vintage_year = extractVintageYear(feature_name);
        
        // Map any start_year < 2012 to 2012
        int effective_start_year = std::max(mortgage.start_year, 2012);
        
        return (effective_start_year == vintage_year) ? 1.0 : 0.0;
    }
    
    // Unknown feature
    return 0.0;
}

bool FeatureBuilder::isVintageFeature(const std::string& feature_name) const {
    return feature_name.find("vintage_") == 0;
}

int FeatureBuilder::extractVintageYear(const std::string& feature_name) const {
    if (!isVintageFeature(feature_name)) {
        return 2012;
    }
    
    // Extract year from "vintage_YYYY"
    std::string year_str = feature_name.substr(8);  // Skip "vintage_"
    try {
        return std::stoi(year_str);
    } catch (...) {
        return 2012;
    }
}
