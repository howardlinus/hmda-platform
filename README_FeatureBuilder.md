# FeatureBuilder C++ Module

This module provides feature calculation functionality for ARIMAX inference, specifically supporting the `refi_incentive` feature with per-period penalty points.

## Overview

The FeatureBuilder module allows users to compute feature vectors for financial forecasting models. The primary feature is `refi_incentive`, which calculates refinancing incentive values based on benchmark changes or rate differences, with configurable per-period penalty adjustments.

## Components

### FeatureContext

A structure containing all necessary parameters for feature calculation:

- `period`: Current forecasted period (1-indexed)
- `annual_rate`: Annual rate for the period
- `forecast_rate_or_exog`: Forecasted rate or exogenous variable
- `benchmark_start`: Starting benchmark value
- `benchmark_running`: Running/current benchmark value
- `penalty_points`: Array of per-period penalty points (optional)

### computeFeatureVector

Function that calculates features based on the provided context.

**Formula for refi_incentive:**
```
refi_incentive = base_incentive - penalty_points[period-1]
```

where `base_incentive` is:
- Primary: `(benchmark_running - benchmark_start)`
- Fallback: `(annual_rate - forecast_rate_or_exog)` if benchmarks are equal

**Penalty Points Handling:**
- Indexed by `(period-1)` since periods are 1-indexed
- If not provided or out of bounds, penalty defaults to 0
- Missing periods are treated as having 0 penalty

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Example

```cpp
#include "cre/FeatureBuilder.h"

// Define penalty points for each period
std::vector<double> penalty_points = {0.1, 0.2, 0.15, 0.25, 0.3};

// Create context for period 1
cre::FeatureContext ctx;
ctx.period = 1;
ctx.annual_rate = 5.5;
ctx.forecast_rate_or_exog = 5.0;
ctx.benchmark_start = 100.0;
ctx.benchmark_running = 102.5;
ctx.penalty_points = penalty_points;

// Compute features
std::vector<double> features = cre::computeFeatureVector(ctx);
double refi_incentive = features[0];
```

## Running the Example

After building, run the example program:

```bash
./build/run_arimax_infer
```

This demonstrates:
1. Setting up user input for per-period penalty points
2. Creating FeatureContext with penalty points
3. Computing feature vectors for multiple periods
4. Handling missing or insufficient penalty points

## API Documentation

See `include/cre/FeatureBuilder.h` for detailed API documentation.

## Testing

The example program `examples/run_arimax_infer.cpp` serves as both a usage demonstration and a test of the implementation, covering:

- Normal operation with full penalty points array
- Handling of insufficient penalty points (defaults to 0)
- Handling of empty penalty points array (defaults to 0)

Expected output shows:
- Calculated refi_incentive values for each period
- Proper penalty subtraction
- Correct fallback behavior for missing penalties

## Integration

This module is designed to integrate with the HMDA Platform's financial modeling capabilities. The penalty points mechanism allows for flexible adjustment of refinancing incentive calculations based on user-defined risk or cost factors per forecasting period.
