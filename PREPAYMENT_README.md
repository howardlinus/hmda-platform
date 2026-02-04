# HMDA Prepayment Library

This directory contains the C++ prepayment modeling library with ARIMAX forecast support, dynamic feature builder, and benchmark lookup functionality.

## Overview

The prepayment library provides:
- **Input-agnostic library** (no JSON parsing in src/)
- **Dynamic feature builder** supporting only approved features
- **Benchmark lookup** keyed by (year, quarter, product_type)
- **Vintage one-hot encoding** starting at 2012
- **ARIMAX forecast support** for time series modeling
- **MatrixFractional implementation** with optional Eigen support
- **Example UI** demonstrating JSON/CSV parsing and library usage

## Directory Structure

```
├── include/              # Header files
│   ├── utils/           # Utility classes (Benchmark, CPR, CSV, etc.)
│   └── cre/             # Core modeling classes (ARIMAX, FeatureBuilder, etc.)
├── src/                 # Implementation files (NO JSON parsing here)
│   ├── utils/          # Utility implementations
│   └── *.cpp           # Core implementations
├── examples/            # Example applications (JSON parsing allowed here)
│   └── run_arimax_infer.cpp
├── test_data/           # Sample test data
│   ├── logistic_model.json
│   ├── model_params.json
│   ├── exog_future.json
│   └── transition_matrix.csv
├── scripts/             # Build and setup scripts
│   ├── fetch_vendors.sh
│   └── generate_manifest.sh
├── vendor/              # Vendored dependencies (fetched by scripts)
│   ├── nlohmann/       # JSON library (for examples only)
│   └── eigen/          # Eigen library (optional)
├── Makefile            # Build configuration
└── PREPAYMENT_README.md # This file
```

## Building

### Prerequisites

- g++ compiler with C++17 support
- Standard library
- make

### Fetch Dependencies

Before building, fetch the vendor dependencies:

```bash
./scripts/fetch_vendors.sh
```

This will download:
- `nlohmann/json` (required for examples)
- `Eigen` (optional, for improved matrix operations)

### Build Library

Build the static library:

```bash
make lib
```

This creates `libprepayment.a` which can be linked into your applications.

### Build and Run Example

Build and run the example application:

```bash
make example
```

The example demonstrates:
1. BenchmarkLookup with (year, quarter, product_type) keys
2. LogisticCPRModel.setParameters() and predict()
3. FeatureBuilder with approved features only
4. Vintage mapping (years < 2012 map to 2012)
5. ARIMAXParams and ARIMAXModel forecasting
6. TransitionMatrix from CSV

### Clean Build Artifacts

```bash
make clean
```

## Approved Features

The FeatureBuilder supports only these features:

- `refi_incentive` - Refinance incentive (rate - benchmark_rate)
- `sato` - Seasoned at origination
- `2s10s_spread` (alias: `two_s_ten`) - 2s-10s Treasury spread
- `debt_yield` - Debt yield ratio
- `vintage_YYYY` - One-hot encoding for vintage year (e.g., vintage_2015)
  - Any start_year < 2012 is mapped to 2012

## Library Design

### Key Principles

1. **No JSON in src/**: All source files in `src/` are input-agnostic. JSON parsing is only allowed in `examples/`.

2. **Dynamic Feature Builder**: Features are added dynamically at runtime:
   ```cpp
   FeatureBuilder builder;
   builder.addFeature("refi_incentive");
   builder.addFeature("vintage_2015");
   std::vector<double> features = builder.buildFeatures(mortgage);
   ```

3. **Benchmark Lookup**: Uses composite key (year, quarter, product_type):
   ```cpp
   BenchmarkLookup lookup;
   lookup.addRate(2023, 1, "fixed_30", 0.065);
   double rate = lookup.getRate(2023, 1, "fixed_30");
   ```

4. **Vintage Mapping**: Years before 2012 automatically map to 2012:
   ```cpp
   Mortgage m;
   m.start_year = 2010;  // Maps to 2012 in feature builder
   ```

5. **Logistic CPR Model**: Simple container with setParameters():
   ```cpp
   LogisticCPRModel model;
   std::map<std::string, double> params;
   params["intercept"] = -2.5;
   params["refi_incentive"] = 0.8;
   model.setParameters(params);
   double cpr = model.predict(features);
   ```

6. **ARIMAX Support**: Time series forecasting with exogenous variables:
   ```cpp
   ARIMAXParams params;
   params.p = 1; params.d = 0; params.q = 1;
   ARIMAXModel model(params);
   auto forecast = model.forecast(history, exog_future, steps);
   ```

7. **Matrix Fractional Power**: Optional Eigen support with fallback:
   ```cpp
   auto result = MatrixFractional::power(matrix, 0.5);
   // Uses Eigen if available, otherwise uses heuristic
   ```

## Example Usage

See `examples/run_arimax_infer.cpp` for complete usage examples.

```cpp
#include "cre/PrepaymentModel.h"

// Create prepayment model
PrepaymentModel model;

// Setup feature builder
model.feature_builder.addFeature("refi_incentive");
model.feature_builder.addFeature("sato");
model.feature_builder.addFeature("vintage_2015");

// Setup CPR model parameters
std::map<std::string, double> params = {
    {"intercept", -2.5},
    {"refi_incentive", 0.8},
    {"sato", 0.3}
};
model.cpr_model.setParameters(params);

// Setup benchmark lookup
model.benchmark_lookup.addRate(2023, 1, "fixed_30", 0.065);

// Create mortgage
Mortgage mortgage;
mortgage.rate = 0.075;
mortgage.start_year = 2015;
mortgage.product_type = "fixed_30";
mortgage.current_benchmark_rate = 0.065;
mortgage.sato = 1.2;

// Predict CPR
double cpr = model.predictCPR(mortgage);
```

## Testing

The library includes test data in `test_data/`:
- `logistic_model.json` - Sample logistic model parameters
- `model_params.json` - Sample ARIMAX parameters
- `exog_future.json` - Sample exogenous forecast data
- `transition_matrix.csv` - Sample transition matrix

Run the example to test all functionality:
```bash
make example
```

## Dependencies

### Required for Library
- C++17 standard library only
- No external dependencies

### Required for Examples
- nlohmann/json (vendored via fetch_vendors.sh)

### Optional
- Eigen (vendored via fetch_vendors.sh) - Improves MatrixFractional performance

## License

See the main repository LICENSE file.

## Contributing

This library is part of the HMDA Platform. Contributions should:
1. Maintain input-agnostic design (no JSON in src/)
2. Only add approved features to FeatureBuilder
3. Follow existing code style
4. Include tests in examples/

## Notes

- Vendor dependencies are NOT committed to the repository
- Run `./scripts/fetch_vendors.sh` to download them separately
- The library compiles without vendor dependencies
- Vendor dependencies are only needed for building examples
