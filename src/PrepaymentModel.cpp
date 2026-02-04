#include "../include/cre/PrepaymentModel.h"

double PrepaymentModel::predictCPR(const Mortgage& mortgage) const {
    // Build features using the feature builder
    std::vector<double> features = feature_builder.buildFeatures(mortgage);
    
    // Predict CPR using the logistic model
    return cpr_model.predict(features);
}
