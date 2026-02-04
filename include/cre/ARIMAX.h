#ifndef ARIMAX_H
#define ARIMAX_H

#include <vector>

namespace cre {

struct ARIMAXParams {
    std::vector<double> ar_params;      // AR parameters
    std::vector<double> ma_params;      // MA parameters
    std::vector<double> exog_params;    // Exogenous parameters
    double constant;                     // Constant term
    int order_ar;                        // AR order (p)
    int order_d;                         // Differencing order (d)
    int order_ma;                        // MA order (q)
    
    ARIMAXParams() : constant(0.0), order_ar(0), order_d(0), order_ma(0) {}
};

class ARIMAX {
public:
    ARIMAX();
    
    // Set ARIMAX parameters
    void setParams(const ARIMAXParams& params);
    
    // Forecast future values given historical data and exogenous variables
    std::vector<double> forecast(
        const std::vector<double>& history,
        const std::vector<std::vector<double>>& exog_future,
        int n_periods) const;
    
    // Get parameters
    const ARIMAXParams& getParams() const { return params_; }
    
private:
    ARIMAXParams params_;
    
    // Apply differencing
    std::vector<double> difference(const std::vector<double>& series, int order) const;
    
    // Reverse differencing
    std::vector<double> undifference(
        const std::vector<double>& differenced,
        const std::vector<double>& original,
        int order) const;
};

} // namespace cre

#endif // ARIMAX_H
