#ifndef CRE_ARIMAX_H
#define CRE_ARIMAX_H

#include <vector>
#include <deque>

namespace cre {

// ARIMAX model: Autoregressive Integrated Moving Average with eXogenous variables
class ARIMAX {
public:
    ARIMAX(int p, int d, int q);

    // Set model parameters
    void setARCoefficients(const std::vector<double>& ar_coeffs);
    void setMACoefficients(const std::vector<double>& ma_coeffs);
    void setExogCoefficients(const std::vector<double>& exog_coeffs);
    void setIntercept(double intercept);

    // Fit model (placeholder - not fully implemented)
    void fit(const std::vector<double>& endog, 
             const std::vector<std::vector<double>>& exog);

    // Predict future values
    std::vector<double> predict(int steps,
                                const std::vector<std::vector<double>>& exog_future);

    // Get/set parameters
    int getP() const { return p_; }
    int getD() const { return d_; }
    int getQ() const { return q_; }

private:
    int p_; // AR order
    int d_; // Integration order
    int q_; // MA order

    std::vector<double> ar_coeffs_;
    std::vector<double> ma_coeffs_;
    std::vector<double> exog_coeffs_;
    double intercept_ = 0.0;

    std::deque<double> history_; // Historical values
    std::deque<double> errors_;  // Historical errors
};

} // namespace cre

#endif // CRE_ARIMAX_H
