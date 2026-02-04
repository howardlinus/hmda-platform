#include "cre/ARIMAX.h"
#include <algorithm>
#include <numeric>

namespace cre {

ARIMAX::ARIMAX(int p, int d, int q)
    : p_(p), d_(d), q_(q) {
}

void ARIMAX::setARCoefficients(const std::vector<double>& ar_coeffs) {
    ar_coeffs_ = ar_coeffs;
}

void ARIMAX::setMACoefficients(const std::vector<double>& ma_coeffs) {
    ma_coeffs_ = ma_coeffs;
}

void ARIMAX::setExogCoefficients(const std::vector<double>& exog_coeffs) {
    exog_coeffs_ = exog_coeffs;
}

void ARIMAX::setIntercept(double intercept) {
    intercept_ = intercept;
}

void ARIMAX::fit(const std::vector<double>& endog,
                 const std::vector<std::vector<double>>& exog) {
    // Store historical values for prediction
    history_.clear();
    errors_.clear();

    // Keep last p_ values for AR component
    size_t start = (endog.size() > static_cast<size_t>(p_)) ? 
                   endog.size() - p_ : 0;
    for (size_t i = start; i < endog.size(); ++i) {
        history_.push_back(endog[i]);
    }

    // Initialize errors to zero
    for (int i = 0; i < q_; ++i) {
        errors_.push_back(0.0);
    }
}

std::vector<double> ARIMAX::predict(int steps,
                                    const std::vector<std::vector<double>>& exog_future) {
    std::vector<double> predictions;
    predictions.reserve(steps);

    for (int step = 0; step < steps; ++step) {
        double pred = intercept_;

        // AR component
        for (size_t i = 0; i < ar_coeffs_.size() && i < history_.size(); ++i) {
            pred += ar_coeffs_[i] * history_[history_.size() - 1 - i];
        }

        // MA component
        for (size_t i = 0; i < ma_coeffs_.size() && i < errors_.size(); ++i) {
            pred += ma_coeffs_[i] * errors_[errors_.size() - 1 - i];
        }

        // Exogenous variables
        if (step < static_cast<int>(exog_future.size())) {
            const auto& exog_vars = exog_future[step];
            for (size_t i = 0; i < exog_coeffs_.size() && i < exog_vars.size(); ++i) {
                pred += exog_coeffs_[i] * exog_vars[i];
            }
        }

        predictions.push_back(pred);

        // Update history and errors
        history_.push_back(pred);
        if (history_.size() > static_cast<size_t>(p_)) {
            history_.pop_front();
        }

        errors_.push_back(0.0); // Assume zero error for future predictions
        if (errors_.size() > static_cast<size_t>(q_)) {
            errors_.pop_front();
        }
    }

    return predictions;
}

} // namespace cre
