#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "cre/PrepaymentModel.h"
#include "cre/Mortgage.h"
#include "cre/ARIMAX.h"
#include "utils/CSV.h"
#include "utils/TransitionMatrix.h"

using json = nlohmann::json;

// Helper to load JSON from file
json loadJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + filename);
    }
    json j;
    file >> j;
    return j;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Prepayment Model Example ===\n" << std::endl;

        // Parse command line arguments or use defaults
        std::string logistic_model_file = "test_data/logistic_model.json";
        std::string model_params_file = "test_data/model_params.json";
        std::string exog_future_file = "test_data/exog_future.json";
        std::string transition_matrix_file = "test_data/transition_matrix.csv";

        if (argc > 1) logistic_model_file = argv[1];
        if (argc > 2) model_params_file = argv[2];
        if (argc > 3) exog_future_file = argv[3];
        if (argc > 4) transition_matrix_file = argv[4];

        // Load logistic model parameters
        std::cout << "Loading logistic model from: " << logistic_model_file << std::endl;
        json logistic_json = loadJson(logistic_model_file);
        
        std::vector<double> coefficients = logistic_json["coefficients"].get<std::vector<double>>();
        double intercept = logistic_json["intercept"].get<double>();
        std::vector<std::string> feature_names = logistic_json["feature_names"].get<std::vector<std::string>>();

        // Create prepayment model and configure
        cre::PrepaymentModel prepay_model;
        prepay_model.cpr_model.setParameters(coefficients, intercept);
        
        // Add features to feature builder
        for (const auto& name : feature_names) {
            prepay_model.feature_builder.addFeature(name);
        }
        std::cout << "Configured " << feature_names.size() << " features" << std::endl;

        // Load benchmark rates (example data)
        // In real usage, these would come from external data
        prepay_model.product_type = "30Y_FIXED";
        prepay_model.benchmark_lookup->setBenchmark(2023, 1, "30Y_FIXED", 0.065);
        prepay_model.benchmark_lookup->setBenchmark(2023, 2, "30Y_FIXED", 0.068);
        prepay_model.benchmark_lookup->setBenchmark(2023, 3, "30Y_FIXED", 0.070);
        prepay_model.benchmark_lookup->setBenchmark(2023, 4, "30Y_FIXED", 0.072);
        std::cout << "Loaded " << prepay_model.benchmark_lookup->size() << " benchmark rates" << std::endl;

        // Build features for a sample loan
        cre::FeatureBuilder::InputData sample_input;
        sample_input.coupon_rate = 0.075;
        sample_input.benchmark_rate = prepay_model.benchmark_lookup->getBenchmark(2023, 1, "30Y_FIXED");
        sample_input.seasonality = 0.15;
        sample_input.spread_2s10s = 0.025;
        sample_input.debt_yield = 0.12;
        sample_input.start_year = 2020;

        std::vector<double> features = prepay_model.feature_builder.build(sample_input);
        std::cout << "\nBuilt feature vector (size=" << features.size() << "):" << std::endl;
        for (size_t i = 0; i < features.size() && i < feature_names.size(); ++i) {
            std::cout << "  " << feature_names[i] << ": " << features[i] << std::endl;
        }

        // Predict CPR
        double cpr_prediction = prepay_model.cpr_model.predict(features);
        std::cout << "\nPredicted CPR: " << (cpr_prediction * 100.0) << "%" << std::endl;

        // Load ARIMAX model parameters
        std::cout << "\nLoading ARIMAX model from: " << model_params_file << std::endl;
        json arimax_json = loadJson(model_params_file);
        
        int p = arimax_json["p"].get<int>();
        int d = arimax_json["d"].get<int>();
        int q = arimax_json["q"].get<int>();
        
        cre::ARIMAX arimax_model(p, d, q);
        arimax_model.setARCoefficients(arimax_json["ar_coeffs"].get<std::vector<double>>());
        arimax_model.setMACoefficients(arimax_json["ma_coeffs"].get<std::vector<double>>());
        arimax_model.setExogCoefficients(arimax_json["exog_coeffs"].get<std::vector<double>>());
        arimax_model.setIntercept(arimax_json["intercept"].get<double>());
        
        std::cout << "ARIMAX(" << p << "," << d << "," << q << ") configured" << std::endl;

        // Load exogenous future data
        std::cout << "\nLoading exogenous future data from: " << exog_future_file << std::endl;
        json exog_json = loadJson(exog_future_file);
        std::vector<std::vector<double>> exog_future = exog_json["exog_data"].get<std::vector<std::vector<double>>>();
        
        // Make predictions
        int forecast_steps = static_cast<int>(exog_future.size());
        std::vector<double> predictions = arimax_model.predict(forecast_steps, exog_future);
        
        std::cout << "ARIMAX predictions for " << forecast_steps << " periods:" << std::endl;
        for (size_t i = 0; i < predictions.size(); ++i) {
            std::cout << "  Period " << (i+1) << ": " << predictions[i] << std::endl;
        }

        // Load and validate transition matrix
        std::cout << "\nLoading transition matrix from: " << transition_matrix_file << std::endl;
        auto csv_data = util::CSVReader::read(transition_matrix_file);
        
        std::vector<std::vector<double>> trans_matrix;
        for (size_t i = 1; i < csv_data.size(); ++i) { // Skip header
            // Skip empty rows
            if (csv_data[i].empty() || (csv_data[i].size() == 1 && csv_data[i][0].empty())) {
                continue;
            }
            std::vector<double> row;
            for (size_t j = 1; j < csv_data[i].size(); ++j) { // Skip row label
                if (!csv_data[i][j].empty()) {
                    row.push_back(std::stod(csv_data[i][j]));
                }
            }
            if (!row.empty()) {
                trans_matrix.push_back(row);
            }
        }

        util::TransitionMatrix tm(trans_matrix);
        std::cout << "Transition matrix loaded: " << tm.rows() << "x" << tm.cols() << std::endl;
        std::cout << "Matrix is valid: " << (util::TransitionMatrix::validateTransitionMatrix(trans_matrix) ? "YES" : "NO") << std::endl;

        // Demonstrate mortgage amortization with CPR
        std::cout << "\n=== Mortgage Amortization Example ===" << std::endl;
        double principal = 300000.0;
        double annual_rate = 0.045;
        int term_months = 360;
        
        cre::Mortgage mortgage(principal, annual_rate, term_months);
        std::cout << "Mortgage: $" << principal << " at " << (annual_rate * 100) << "% for " << term_months << " months" << std::endl;
        std::cout << "Monthly payment: $" << mortgage.calculateMonthlyPayment() << std::endl;

        // Use constant CPR for demonstration
        std::vector<double> cpr_schedule(12, cpr_prediction);
        auto schedule = mortgage.amortizationSchedule(cpr_schedule);
        
        std::cout << "\nFirst 12 months of amortization schedule:" << std::endl;
        std::cout << "Period | Principal | Interest | Remaining | CPR" << std::endl;
        for (size_t i = 0; i < std::min(schedule.size(), size_t(12)); ++i) {
            const auto& pmt = schedule[i];
            std::cout << "  " << pmt.period << "    | $" << pmt.principal_payment 
                     << " | $" << pmt.interest_payment 
                     << " | $" << pmt.remaining_balance
                     << " | " << (pmt.cpr * 100) << "%" << std::endl;
        }

        std::cout << "\nExample completed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
