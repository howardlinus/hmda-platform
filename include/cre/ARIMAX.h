#ifndef ARIMAX_H
#define ARIMAX_H

#include <vector>

struct ARIMAXParams {
    int p;  // AR order
    int d;  // differencing order
    int q;  // MA order
    std::vector<double> ar_coeffs;
    std::vector<double> ma_coeffs;
    std::vector<double> exog_coeffs;
    double intercept;
    
    ARIMAXParams();
};

class ARIMAXModel {
public:
    ARIMAXModel();
    ARIMAXModel(const ARIMAXParams& params);
    
    void setParams(const ARIMAXParams& params);
    std::vector<double> forecast(
        const std::vector<double>& history,
        const std::vector<std::vector<double>>& exog_future,
        int steps
    ) const;
    
private:
    ARIMAXParams params_;
    
    std::vector<double> difference(const std::vector<double>& series, int order) const;
    std::vector<double> integrate(const std::vector<double>& series, const std::vector<double>& base, int order) const;
};

#endif // ARIMAX_H
