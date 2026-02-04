#include "cre/ARIMAX.h"
#include <stdexcept>
#include <algorithm>

namespace cre {

ARIMAX::ARIMAX() {}

void ARIMAX::setParams(const ARIMAXParams& params) {
    params_ = params;
}

std::vector<double> ARIMAX::difference(const std::vector<double>& series, int order) const {
    if (order == 0) {
        return series;
    }
    
    std::vector<double> result = series;
    for (int d = 0; d < order; ++d) {
        std::vector<double> diff;
        for (size_t i = 1; i < result.size(); ++i) {
            diff.push_back(result[i] - result[i-1]);
        }
        result = diff;
    }
    
    return result;
}

std::vector<double> ARIMAX::undifference(
    const std::vector<double>& differenced,
    const std::vector<double>& original,
    int order) const {
    
    if (order == 0) {
        return differenced;
    }
    
    std::vector<double> result = differenced;
    
    for (int d = 0; d < order; ++d) {
        std::vector<double> undiff;
        
        // Get the last value from original to start integration
        double last_val = 0.0;
        if (!original.empty()) {
            last_val = original[original.size() - order + d];
        }
        
        undiff.push_back(last_val);
        for (size_t i = 0; i < result.size(); ++i) {
            undiff.push_back(undiff.back() + result[i]);
        }
        
        result = undiff;
    }
    
    return result;
}

std::vector<double> ARIMAX::forecast(
    const std::vector<double>& history,
    const std::vector<std::vector<double>>& exog_future,
    int n_periods) const {
    
    if (n_periods <= 0) {
        return {};
    }
    
    // Apply differencing
    std::vector<double> diff_history = difference(history, params_.order_d);
    
    // Initialize with zeros for residuals (MA component)
    std::vector<double> residuals(params_.order_ma, 0.0);
    
    std::vector<double> forecasts;
    std::vector<double> level_forecasts; // In differenced space
    
    for (int t = 0; t < n_periods; ++t) {
        double forecast_val = params_.constant;
        
        // AR component
        for (int p = 0; p < params_.order_ar; ++p) {
            if (p < static_cast<int>(level_forecasts.size())) {
                forecast_val += params_.ar_params[p] * level_forecasts[level_forecasts.size() - 1 - p];
            } else if (p - level_forecasts.size() < diff_history.size()) {
                int idx = diff_history.size() - 1 - (p - level_forecasts.size());
                forecast_val += params_.ar_params[p] * diff_history[idx];
            }
        }
        
        // MA component (using stored residuals)
        for (int q = 0; q < params_.order_ma; ++q) {
            if (q < static_cast<int>(residuals.size())) {
                forecast_val += params_.ma_params[q] * residuals[residuals.size() - 1 - q];
            }
        }
        
        // Exogenous component
        if (t < static_cast<int>(exog_future.size())) {
            for (size_t i = 0; i < exog_future[t].size() && i < params_.exog_params.size(); ++i) {
                forecast_val += params_.exog_params[i] * exog_future[t][i];
            }
        }
        
        level_forecasts.push_back(forecast_val);
        
        // Update residuals (for future forecasts, assume 0)
        residuals.push_back(0.0);
        if (residuals.size() > static_cast<size_t>(params_.order_ma)) {
            residuals.erase(residuals.begin());
        }
    }
    
    // Undo differencing
    forecasts = undifference(level_forecasts, history, params_.order_d);
    
    // Return only n_periods
    if (forecasts.size() > static_cast<size_t>(n_periods)) {
        forecasts.resize(n_periods);
    }
    
    return forecasts;
}

} // namespace cre
