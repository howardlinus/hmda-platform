#include <iostream>
#include <fstream>
#include <vector>
#include "../include/cre/PrepaymentModel.h"
#include "../include/cre/ARIMAX.h"
#include "../include/utils/TransitionMatrix.h"

// JSON parsing only allowed in examples/
#ifdef HAVE_JSON
#include "../vendor/nlohmann/json.hpp"
using json = nlohmann::json;
#endif

// Helper function to load JSON file
#ifdef HAVE_JSON
json loadJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return json();
    }
    json j;
    file >> j;
    return j;
}
#endif

int main(int argc, char* argv[]) {
    std::cout << "=== HMDA Prepayment Library Example ===" << std::endl;
    
#ifdef HAVE_JSON
    // Load test data
    std::string test_data_dir = "test_data/";
    
    // 1. Demonstrate BenchmarkLookup
    std::cout << "\n1. BenchmarkLookup Example:" << std::endl;
    BenchmarkLookup benchmark_lookup;
    benchmark_lookup.addRate(2023, 1, "fixed_30", 0.065);
    benchmark_lookup.addRate(2023, 1, "fixed_15", 0.055);
    benchmark_lookup.addRate(2023, 2, "fixed_30", 0.068);
    
    std::cout << "   Rate for 2023 Q1 fixed_30: " 
              << benchmark_lookup.getRate(2023, 1, "fixed_30") << std::endl;
    std::cout << "   Rate for 2023 Q2 fixed_30: " 
              << benchmark_lookup.getRate(2023, 2, "fixed_30") << std::endl;
    
    // 2. Demonstrate LogisticCPRModel with setParameters
    std::cout << "\n2. LogisticCPRModel Example:" << std::endl;
    
    json model_params = loadJSON(test_data_dir + "logistic_model.json");
    
    LogisticCPRModel cpr_model;
    if (!model_params.empty() && model_params.contains("coefficients")) {
        std::map<std::string, double> params;
        for (auto& [key, value] : model_params["coefficients"].items()) {
            params[key] = value.get<double>();
        }
        cpr_model.setParameters(params);
        std::cout << "   Loaded " << params.size() << " parameters from JSON" << std::endl;
    }
    
    // 3. Demonstrate FeatureBuilder with approved features
    std::cout << "\n3. FeatureBuilder Example (approved features only):" << std::endl;
    FeatureBuilder feature_builder;
    feature_builder.addFeature("refi_incentive");
    feature_builder.addFeature("sato");
    feature_builder.addFeature("two_s_ten");  // alias for 2s10s_spread
    feature_builder.addFeature("debt_yield");
    feature_builder.addFeature("vintage_2012");
    feature_builder.addFeature("vintage_2015");
    feature_builder.addFeature("vintage_2020");
    
    Mortgage example_mortgage;
    example_mortgage.balance = 1000000.0;
    example_mortgage.rate = 0.075;
    example_mortgage.start_year = 2015;
    example_mortgage.start_quarter = 2;
    example_mortgage.product_type = "fixed_30";
    example_mortgage.current_benchmark_rate = 0.065;
    example_mortgage.sato = 1.2;
    example_mortgage.two_s_ten_spread = 0.015;
    example_mortgage.debt_yield = 0.08;
    
    std::vector<double> features = feature_builder.buildFeatures(example_mortgage);
    std::cout << "   Built " << features.size() << " features:" << std::endl;
    auto feature_names = feature_builder.getFeatureNames();
    for (size_t i = 0; i < features.size(); ++i) {
        std::cout << "     " << feature_names[i] << ": " << features[i] << std::endl;
    }
    
    // Test vintage mapping (< 2012 maps to 2012)
    Mortgage old_mortgage = example_mortgage;
    old_mortgage.start_year = 2010;  // Should map to 2012
    std::vector<double> old_features = feature_builder.buildFeatures(old_mortgage);
    std::cout << "   Vintage 2010 (mapped to 2012): vintage_2012 = " 
              << old_features[4] << std::endl;
    
    // 4. Demonstrate PrepaymentModel
    std::cout << "\n4. PrepaymentModel Example:" << std::endl;
    PrepaymentModel prepayment_model;
    prepayment_model.feature_builder = feature_builder;
    prepayment_model.cpr_model = cpr_model;
    prepayment_model.benchmark_lookup = benchmark_lookup;
    
    double cpr_prediction = prepayment_model.predictCPR(example_mortgage);
    std::cout << "   Predicted CPR: " << cpr_prediction << std::endl;
    
    // 5. Demonstrate ARIMAXParams and ARIMAXModel
    std::cout << "\n5. ARIMAX Forecast Example:" << std::endl;
    
    json arimax_params = loadJSON(test_data_dir + "model_params.json");
    
    ARIMAXParams params;
    if (!arimax_params.empty()) {
        params.p = arimax_params.value("p", 1);
        params.d = arimax_params.value("d", 0);
        params.q = arimax_params.value("q", 1);
        params.intercept = arimax_params.value("intercept", 0.0);
        
        if (arimax_params.contains("ar_coeffs")) {
            params.ar_coeffs = arimax_params["ar_coeffs"].get<std::vector<double>>();
        }
        if (arimax_params.contains("ma_coeffs")) {
            params.ma_coeffs = arimax_params["ma_coeffs"].get<std::vector<double>>();
        }
        if (arimax_params.contains("exog_coeffs")) {
            params.exog_coeffs = arimax_params["exog_coeffs"].get<std::vector<double>>();
        }
        
        std::cout << "   ARIMAX(" << params.p << "," << params.d << "," << params.q 
                  << ") loaded" << std::endl;
    }
    
    ARIMAXModel arimax_model(params);
    
    // Load exogenous future data
    json exog_future_json = loadJSON(test_data_dir + "exog_future.json");
    std::vector<std::vector<double>> exog_future;
    if (!exog_future_json.empty() && exog_future_json.is_array()) {
        for (const auto& row : exog_future_json) {
            exog_future.push_back(row.get<std::vector<double>>());
        }
        std::cout << "   Loaded " << exog_future.size() 
                  << " periods of exogenous data" << std::endl;
    }
    
    // Historical data for forecasting
    std::vector<double> history = {0.05, 0.052, 0.048, 0.055, 0.06};
    
    int forecast_steps = 3;
    std::vector<double> forecast = arimax_model.forecast(history, exog_future, forecast_steps);
    
    std::cout << "   Forecast " << forecast_steps << " steps:" << std::endl;
    for (size_t i = 0; i < forecast.size(); ++i) {
        std::cout << "     Step " << (i + 1) << ": " << forecast[i] << std::endl;
    }
    
    // 6. Demonstrate TransitionMatrix from CSV
    std::cout << "\n6. TransitionMatrix Example:" << std::endl;
    TransitionMatrix transition_matrix;
    transition_matrix.loadFromCSV(test_data_dir + "transition_matrix.csv");
    
    std::cout << "   Loaded transition matrix: " 
              << transition_matrix.size() << "x" << transition_matrix.size() << std::endl;
    
    std::vector<double> initial_state = {1.0, 0.0, 0.0};
    std::vector<double> next_state = transition_matrix.apply(initial_state);
    
    std::cout << "   Applied to state [1, 0, 0]:" << std::endl;
    std::cout << "   Result: [";
    for (size_t i = 0; i < next_state.size(); ++i) {
        std::cout << next_state[i];
        if (i < next_state.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    
#else
    std::cout << "\nJSON support not available. Please run:" << std::endl;
    std::cout << "  ./scripts/fetch_vendors.sh" << std::endl;
    std::cout << "  make example" << std::endl;
#endif
    
    std::cout << "\n=== Example completed ===" << std::endl;
    return 0;
}
