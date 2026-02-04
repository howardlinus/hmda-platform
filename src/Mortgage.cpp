#include "cre/Mortgage.h"
#include <cmath>
#include <stdexcept>

namespace cre {

Mortgage::Mortgage() {}

Mortgage::Mortgage(const LoanCharacteristics& characteristics)
    : characteristics_(characteristics) {}

void Mortgage::setCharacteristics(const LoanCharacteristics& characteristics) {
    characteristics_ = characteristics;
}

double Mortgage::calculateMonthlyPayment() const {
    if (characteristics_.remaining_term_months <= 0) {
        return 0.0;
    }
    
    double balance = characteristics_.current_balance;
    double monthly_rate = characteristics_.interest_rate / 12.0;
    int n = characteristics_.remaining_term_months;
    
    if (monthly_rate <= 0.0) {
        return balance / n;
    }
    
    // Monthly payment formula: P = L * [r(1+r)^n] / [(1+r)^n - 1]
    double factor = std::pow(1.0 + monthly_rate, n);
    double payment = balance * (monthly_rate * factor) / (factor - 1.0);
    
    return payment;
}

double Mortgage::applyPrepayment(double balance, double cpr) const {
    if (cpr <= 0.0 || balance <= 0.0) {
        return balance;
    }
    
    // CPR is annual rate, convert to monthly (SMM = 1 - (1 - CPR)^(1/12))
    double smm = 1.0 - std::pow(1.0 - cpr, 1.0 / 12.0);
    
    // Clamp SMM
    if (smm < 0.0) smm = 0.0;
    if (smm > 1.0) smm = 1.0;
    
    double prepayment = balance * smm;
    return balance - prepayment;
}

double Mortgage::calculateBalance(int periods, double constant_cpr) const {
    double balance = characteristics_.current_balance;
    double monthly_rate = characteristics_.interest_rate / 12.0;
    double payment = calculateMonthlyPayment();
    
    for (int i = 0; i < periods && balance > 0.0; ++i) {
        double interest = balance * monthly_rate;
        double principal = payment - interest;
        
        if (principal < 0.0) principal = 0.0;
        
        balance -= principal;
        
        if (balance < 0.0) {
            balance = 0.0;
            break;
        }
        
        // Apply prepayment
        if (constant_cpr > 0.0) {
            balance = applyPrepayment(balance, constant_cpr);
        }
    }
    
    return balance;
}

std::vector<AmortizationRow> Mortgage::amortizationSchedule(
    const std::vector<double>& cpr_forecast,
    const utils::TransitionMatrix* transition_matrix) const {
    
    std::vector<AmortizationRow> schedule;
    
    double balance = characteristics_.current_balance;
    double monthly_rate = characteristics_.interest_rate / 12.0;
    double payment = calculateMonthlyPayment();
    
    int periods = characteristics_.remaining_term_months;
    if (!cpr_forecast.empty() && static_cast<int>(cpr_forecast.size()) < periods) {
        periods = cpr_forecast.size();
    }
    
    for (int i = 0; i < periods && balance > 0.001; ++i) {
        AmortizationRow row;
        row.period = i + 1;
        row.beginning_balance = balance;
        
        double interest = balance * monthly_rate;
        double principal = payment - interest;
        
        if (principal < 0.0) principal = 0.0;
        if (principal > balance) principal = balance;
        
        row.scheduled_payment = payment;
        row.interest_payment = interest;
        row.principal_payment = principal;
        
        balance -= principal;
        
        // Apply CPR
        double cpr = 0.0;
        if (i < static_cast<int>(cpr_forecast.size())) {
            cpr = cpr_forecast[i];
        }
        row.cpr = cpr;
        
        if (cpr > 0.0 && balance > 0.0) {
            double balance_before_prepay = balance;
            balance = applyPrepayment(balance, cpr);
            row.prepayment_amount = balance_before_prepay - balance;
        } else {
            row.prepayment_amount = 0.0;
        }
        
        row.ending_balance = balance;
        
        schedule.push_back(row);
        
        if (balance <= 0.0) {
            break;
        }
    }
    
    return schedule;
}

} // namespace cre
