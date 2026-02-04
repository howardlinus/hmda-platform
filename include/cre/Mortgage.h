#ifndef CRE_MORTGAGE_H
#define CRE_MORTGAGE_H

#include <vector>
#include <string>

namespace cre {

struct MortgagePayment {
    int period;
    double principal_payment;
    double interest_payment;
    double remaining_balance;
    double scheduled_payment;
    double cpr;  // Conditional prepayment rate
};

class Mortgage {
public:
    Mortgage(double principal, double annual_rate, int term_months);

    // Compute amortization schedule with optional CPR rates
    std::vector<MortgagePayment> amortizationSchedule(
        const std::vector<double>& cpr_rates = std::vector<double>()) const;

    // Get mortgage properties
    double getPrincipal() const { return principal_; }
    double getAnnualRate() const { return annual_rate_; }
    int getTermMonths() const { return term_months_; }

    // Calculate monthly payment (fixed rate)
    double calculateMonthlyPayment() const;

private:
    double principal_;
    double annual_rate_;
    int term_months_;
};

} // namespace cre

#endif // CRE_MORTGAGE_H
