#ifndef MORTGAGE_H
#define MORTGAGE_H

#include <vector>
#include <string>
#include "utils/TransitionMatrix.h"

namespace cre {

struct LoanCharacteristics {
    double original_balance;
    double current_balance;
    double interest_rate;
    int remaining_term_months;
    int original_term_months;
    double current_ltv;
    double dscr;
    std::string property_type;
    std::string product_type;
    int origination_year;
    int origination_quarter;
    
    LoanCharacteristics() 
        : original_balance(0.0), current_balance(0.0), interest_rate(0.0),
          remaining_term_months(0), original_term_months(0), 
          current_ltv(0.0), dscr(0.0),
          origination_year(0), origination_quarter(0) {}
};

struct AmortizationRow {
    int period;
    double beginning_balance;
    double scheduled_payment;
    double interest_payment;
    double principal_payment;
    double ending_balance;
    double cpr;
    double prepayment_amount;
    
    AmortizationRow() 
        : period(0), beginning_balance(0.0), scheduled_payment(0.0),
          interest_payment(0.0), principal_payment(0.0), ending_balance(0.0),
          cpr(0.0), prepayment_amount(0.0) {}
};

class Mortgage {
public:
    Mortgage();
    Mortgage(const LoanCharacteristics& characteristics);
    
    // Set loan characteristics
    void setCharacteristics(const LoanCharacteristics& characteristics);
    
    // Get loan characteristics
    const LoanCharacteristics& getCharacteristics() const { return characteristics_; }
    
    // Generate amortization schedule with prepayment
    std::vector<AmortizationRow> amortizationSchedule(
        const std::vector<double>& cpr_forecast,
        const utils::TransitionMatrix* transition_matrix = nullptr) const;
    
    // Calculate scheduled monthly payment
    double calculateMonthlyPayment() const;
    
    // Calculate remaining balance after n periods with constant CPR
    double calculateBalance(int periods, double constant_cpr = 0.0) const;
    
private:
    LoanCharacteristics characteristics_;
    
    // Apply prepayment to balance
    double applyPrepayment(double balance, double cpr) const;
};

} // namespace cre

#endif // MORTGAGE_H
