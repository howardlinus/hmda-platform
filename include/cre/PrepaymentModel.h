#ifndef PREPAYMENT_MODEL_H
#define PREPAYMENT_MODEL_H

#include "utils/CPR.h"
#include "cre/FeatureBuilder.h"
#include <vector>
#include <map>

namespace cre {

// PrepaymentModel combines FeatureBuilder and LogisticCPRModel
class PrepaymentModel {
public:
    PrepaymentModel();
    
    // Set the CPR model
    void setCPRModel(const utils::LogisticCPRModel& model);
    
    // Set the feature builder
    void setFeatureBuilder(const FeatureBuilder& builder);
    
    // Predict CPR for a loan given raw features
    double predictCPR(const std::map<std::string, double>& raw_features) const;
    
private:
    utils::LogisticCPRModel cpr_model_;
    FeatureBuilder feature_builder_;
    bool has_cpr_model_;
    bool has_feature_builder_;
};

} // namespace cre

#endif // PREPAYMENT_MODEL_H
