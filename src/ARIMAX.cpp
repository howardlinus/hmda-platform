#include "../include/cre/ARIMAX.h"
#include <algorithm>

ARIMAXParams::ARIMAXParams()
    : p(0), d(0), q(0), intercept(0.0) {}

ARIMAXModel::ARIMAXModel() {}

ARIMAXModel::ARIMAXModel(const ARIMAXParams& params) : params_(params) {}

void ARIMAXModel::setParams(const ARIMAXParams& params) {
    params_ = params;
}

std::vector<double> ARIMAXModel::forecast(
    const std::vector<double>& history,
    const std::vector<std::vector<double>>& exog_future,
    int steps
) const {
    if (history.empty() || steps <= 0) {
        return std::vector<double>();
    }
    
    // Apply differencing if needed
    std::vector<double> diff_history = difference(history, params_.d);
    
    // Initialize forecast
    std::vector<double> forecast_diff;
    std::vector<double> residuals(diff_history.size(), 0.0);
    
    // Generate forecasts for differenced series
    for (int step = 0; step < steps; ++step) {
        double forecast_val = params_.intercept;
        
        // AR component
        for (size_t i = 0; i < params_.ar_coeffs.size(); ++i) {
            int lag = i + 1;
            if (forecast_diff.size() >= static_cast<size_t>(lag)) {
                forecast_val += params_.ar_coeffs[i] * forecast_diff[forecast_diff.size() - lag];
            } else if (diff_history.size() >= static_cast<size_t>(lag - forecast_diff.size())) {
                int hist_idx = diff_history.size() - (lag - forecast_diff.size());
                forecast_val += params_.ar_coeffs[i] * diff_history[hist_idx];
            }
        }
        
        // MA component (using zero residuals for forecast)
        for (size_t i = 0; i < params_.ma_coeffs.size(); ++i) {
            int lag = i + 1;
            if (residuals.size() >= static_cast<size_t>(lag)) {
                forecast_val += params_.ma_coeffs[i] * residuals[residuals.size() - lag];
            }
        }
        
        // Exogenous component
        if (step < static_cast<int>(exog_future.size())) {
            for (size_t i = 0; i < params_.exog_coeffs.size() && i < exog_future[step].size(); ++i) {
                forecast_val += params_.exog_coeffs[i] * exog_future[step][i];
            }
        }
        
        forecast_diff.push_back(forecast_val);
        residuals.push_back(0.0);
    }
    
    // Integrate back if differencing was applied
    std::vector<double> forecast = integrate(forecast_diff, history, params_.d);
    
    return forecast;
}

std::vector<double> ARIMAXModel::difference(const std::vector<double>& series, int order) const {
    std::vector<double> result = series;
    
    for (int d = 0; d < order; ++d) {
        std::vector<double> diff;
        for (size_t i = 1; i < result.size(); ++i) {
            diff.push_back(result[i] - result[i - 1]);
        }
        result = diff;
    }
    
    return result;
}

std::vector<double> ARIMAXModel::integrate(
    const std::vector<double>& series, 
    const std::vector<double>& base,
    int order
) const {
    if (order == 0) {
        return series;
    }
    
    std::vector<double> result = series;
    
    for (int d = 0; d < order; ++d) {
        std::vector<double> integrated;
        
        // Start with last value from base
        double last_val = base.empty() ? 0.0 : base.back();
        
        for (size_t i = 0; i < result.size(); ++i) {
            last_val += result[i];
            integrated.push_back(last_val);
        }
        
        result = integrated;
    }
    
    return result;
}
