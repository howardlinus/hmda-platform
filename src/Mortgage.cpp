#include "cre/Mortgage.h"
#include <cmath>
#include <algorithm>

namespace cre {

Mortgage::Mortgage(double principal, double annual_rate, int term_months)
    : principal_(principal), annual_rate_(annual_rate), term_months_(term_months) {
}

double Mortgage::calculateMonthlyPayment() const {
    if (annual_rate_ <= 0.0) {
        // If rate is zero, just divide principal by term
        return principal_ / term_months_;
    }

    double monthly_rate = annual_rate_ / 12.0;
    double factor = std::pow(1.0 + monthly_rate, term_months_);
    return principal_ * monthly_rate * factor / (factor - 1.0);
}

std::vector<MortgagePayment> Mortgage::amortizationSchedule(
    const std::vector<double>& cpr_rates) const {
    
    std::vector<MortgagePayment> schedule;
    schedule.reserve(term_months_);

    double remaining_balance = principal_;
    double monthly_rate = annual_rate_ / 12.0;
    double scheduled_payment = calculateMonthlyPayment();

    for (int period = 1; period <= term_months_; ++period) {
        if (remaining_balance <= 0.0) {
            break;
        }

        MortgagePayment payment;
        payment.period = period;
        payment.scheduled_payment = scheduled_payment;

        // Calculate interest payment
        payment.interest_payment = remaining_balance * monthly_rate;

        // Calculate principal payment (before prepayment)
        double principal_scheduled = scheduled_payment - payment.interest_payment;
        principal_scheduled = std::max(0.0, std::min(principal_scheduled, remaining_balance));

        // Apply CPR if provided
        double cpr = 0.0;
        if (!cpr_rates.empty() && period - 1 < static_cast<int>(cpr_rates.size())) {
            cpr = cpr_rates[period - 1];
        }
        payment.cpr = cpr;

        // Calculate prepayment amount
        // SMM (Single Monthly Mortality) = 1 - (1 - CPR)^(1/12)
        double smm = 1.0 - std::pow(1.0 - cpr, 1.0 / 12.0);
        double prepayment_amount = remaining_balance * smm;

        // Total principal payment
        payment.principal_payment = principal_scheduled + prepayment_amount;
        payment.principal_payment = std::min(payment.principal_payment, remaining_balance);

        // Update remaining balance
        remaining_balance -= payment.principal_payment;
        payment.remaining_balance = std::max(0.0, remaining_balance);

        schedule.push_back(payment);
    }

    return schedule;
}

} // namespace cre
