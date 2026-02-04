#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// JSON parsing allowed in examples only
#include <nlohmann/json.hpp>

// Library headers (no JSON parsing)
#include "cre/ARIMAX.h"
#include "cre/FeatureBuilder.h"
#include "cre/Mortgage.h"
#include "utils/Benchmark.h"
#include "utils/CPR.h"
#include "utils/CSV.h"
#include "utils/TransitionMatrix.h"

using json = nlohmann::json;

void printUsage(const char* program) {
    std::cout << "Usage: " << program << " <logistic_model.json> <model_params.json> <exog_future.json> <transition_matrix.csv>\n";
    std::cout << "\nExample demonstrates:\n";
    std::cout << "  - Parsing JSON/CSV and populating library structs\n";
    std::cout << "  - Populating BenchmarkLookup\n";
    std::cout << "  - Constructing LogisticCPRModel via setParameters\n";
    std::cout << "  - Filling ARIMAXParams\n";
    std::cout << "  - Running Mortgage::amortizationSchedule\n";
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string logistic_model_file = argv[1];
    std::string model_params_file = argv[2];
    std::string exog_future_file = argv[3];
    std::string transition_matrix_file = argv[4];
    
    try {
        std::cout << "=== ARIMAX Inference Example ===" << std::endl;
        std::cout << std::endl;
        
        // 1. Load and parse logistic model JSON
        std::cout << "1. Loading logistic CPR model from " << logistic_model_file << std::endl;
        std::ifstream logistic_file(logistic_model_file);
        if (!logistic_file.is_open()) {
            throw std::runtime_error("Cannot open logistic model file");
        }
        json logistic_json;
        logistic_file >> logistic_json;
        
        // Parse logistic model and populate LogisticCPRModel
        utils::LogisticCPRModel cpr_model;
        std::vector<double> coefficients;
        std::vector<std::string> feature_names;
        double intercept = 0.0;
        
        if (logistic_json.contains("coefficients")) {
            coefficients = logistic_json["coefficients"].get<std::vector<double>>();
        }
        if (logistic_json.contains("feature_names")) {
            feature_names = logistic_json["feature_names"].get<std::vector<std::string>>();
        }
        if (logistic_json.contains("intercept")) {
            intercept = logistic_json["intercept"].get<double>();
        }
        
        cpr_model.setParameters(coefficients, feature_names, intercept);
        std::cout << "   Loaded " << feature_names.size() << " features" << std::endl;
        
        // 2. Load and parse ARIMAX model parameters JSON
        std::cout << "\n2. Loading ARIMAX parameters from " << model_params_file << std::endl;
        std::ifstream params_file(model_params_file);
        if (!params_file.is_open()) {
            throw std::runtime_error("Cannot open model params file");
        }
        json params_json;
        params_file >> params_json;
        
        // Fill ARIMAXParams
        cre::ARIMAXParams arimax_params;
        if (params_json.contains("ar_params")) {
            arimax_params.ar_params = params_json["ar_params"].get<std::vector<double>>();
        }
        if (params_json.contains("ma_params")) {
            arimax_params.ma_params = params_json["ma_params"].get<std::vector<double>>();
        }
        if (params_json.contains("exog_params")) {
            arimax_params.exog_params = params_json["exog_params"].get<std::vector<double>>();
        }
        if (params_json.contains("constant")) {
            arimax_params.constant = params_json["constant"].get<double>();
        }
        if (params_json.contains("order")) {
            auto order = params_json["order"];
            arimax_params.order_ar = order[0].get<int>();
            arimax_params.order_d = order[1].get<int>();
            arimax_params.order_ma = order[2].get<int>();
        }
        
        cre::ARIMAX arimax_model;
        arimax_model.setParams(arimax_params);
        std::cout << "   ARIMAX order: (" << arimax_params.order_ar << ", " 
                  << arimax_params.order_d << ", " << arimax_params.order_ma << ")" << std::endl;
        
        // 3. Load exogenous future data JSON
        std::cout << "\n3. Loading exogenous future data from " << exog_future_file << std::endl;
        std::ifstream exog_file(exog_future_file);
        if (!exog_file.is_open()) {
            throw std::runtime_error("Cannot open exog future file");
        }
        json exog_json;
        exog_file >> exog_json;
        
        std::vector<std::vector<double>> exog_future;
        if (exog_json.is_array()) {
            for (const auto& row : exog_json) {
                if (row.is_array()) {
                    exog_future.push_back(row.get<std::vector<double>>());
                }
            }
        }
        std::cout << "   Loaded " << exog_future.size() << " periods of exogenous data" << std::endl;
        
        // 4. Load transition matrix CSV
        std::cout << "\n4. Loading transition matrix from " << transition_matrix_file << std::endl;
        utils::TransitionMatrix transition_matrix;
        transition_matrix.loadFromCSV(transition_matrix_file);
        std::cout << "   Loaded " << transition_matrix.getSize() << "x" 
                  << transition_matrix.getSize() << " transition matrix" << std::endl;
        
        // 5. Populate BenchmarkLookup (example data)
        std::cout << "\n5. Populating BenchmarkLookup with example data" << std::endl;
        utils::BenchmarkLookup benchmark_lookup;
        benchmark_lookup.setBenchmark(2023, 1, "fixed_30", 0.065);
        benchmark_lookup.setBenchmark(2023, 2, "fixed_30", 0.067);
        benchmark_lookup.setBenchmark(2023, 3, "fixed_30", 0.070);
        benchmark_lookup.setBenchmark(2023, 4, "fixed_30", 0.068);
        std::cout << "   Added benchmark rates for 2023 Q1-Q4" << std::endl;
        
        // 6. Setup FeatureBuilder
        std::cout << "\n6. Setting up FeatureBuilder" << std::endl;
        cre::FeatureBuilder feature_builder;
        feature_builder.setBenchmarkLookup(&benchmark_lookup);
        
        // 7. Create a sample mortgage
        std::cout << "\n7. Creating sample mortgage" << std::endl;
        cre::LoanCharacteristics loan;
        loan.original_balance = 1000000.0;
        loan.current_balance = 950000.0;
        loan.interest_rate = 0.055;  // 5.5% annual
        loan.remaining_term_months = 300;
        loan.original_term_months = 360;
        loan.current_ltv = 0.75;
        loan.dscr = 1.5;
        loan.property_type = "multifamily";
        loan.product_type = "fixed_30";
        loan.origination_year = 2020;
        loan.origination_quarter = 3;
        
        cre::Mortgage mortgage(loan);
        std::cout << "   Loan: $" << loan.current_balance << " at " << (loan.interest_rate * 100) 
                  << "% for " << loan.remaining_term_months << " months" << std::endl;
        
        // 8. Generate CPR forecast
        // For this example, we use a constant CPR. In production, you could:
        // - Use ARIMAX to forecast CPR from historical data and exogenous variables
        // - Use the LogisticCPRModel with current loan features
        // - Combine multiple forecasting methods
        std::cout << "\n8. Generating CPR forecast" << std::endl;
        int forecast_periods = 12;  // 1 year
        std::vector<double> cpr_forecast(forecast_periods, 0.08);  // 8% CPR
        
        std::cout << "   Generated " << forecast_periods << " periods with constant CPR 8%" << std::endl;
        
        // 9. Generate amortization schedule
        std::cout << "\n9. Generating amortization schedule" << std::endl;
        auto schedule = mortgage.amortizationSchedule(cpr_forecast, &transition_matrix);
        
        std::cout << "\n=== Amortization Schedule ===" << std::endl;
        std::cout << "Period | Beginning Balance | Scheduled Payment | Interest | Principal | CPR | Prepayment | Ending Balance" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        
        for (size_t i = 0; i < std::min(schedule.size(), size_t(12)); ++i) {
            const auto& row = schedule[i];
            printf("%6d | %17.2f | %17.2f | %8.2f | %9.2f | %5.1f%% | %10.2f | %14.2f\n",
                   row.period,
                   row.beginning_balance,
                   row.scheduled_payment,
                   row.interest_payment,
                   row.principal_payment,
                   row.cpr * 100,
                   row.prepayment_amount,
                   row.ending_balance);
        }
        
        if (schedule.size() > 12) {
            std::cout << "... (" << schedule.size() << " total periods)" << std::endl;
        }
        
        // 10. Summary
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "Initial Balance: $" << loan.current_balance << std::endl;
        if (!schedule.empty()) {
            std::cout << "Final Balance: $" << schedule.back().ending_balance << std::endl;
            double total_principal = 0.0;
            double total_interest = 0.0;
            double total_prepayment = 0.0;
            for (const auto& row : schedule) {
                total_principal += row.principal_payment;
                total_interest += row.interest_payment;
                total_prepayment += row.prepayment_amount;
            }
            std::cout << "Total Principal Paid: $" << total_principal << std::endl;
            std::cout << "Total Interest Paid: $" << total_interest << std::endl;
            std::cout << "Total Prepayment: $" << total_prepayment << std::endl;
        }
        
        std::cout << "\n=== Example completed successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
