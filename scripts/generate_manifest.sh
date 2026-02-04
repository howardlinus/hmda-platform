#!/bin/bash
# Script to generate a manifest of library files

echo "Generating prepayment library manifest..."

cat << EOF
Prepayment Library Manifest
============================

Header Files (include/):
  utils/
    - Benchmark.h
    - CPR.h
    - CSV.h
    - TransitionMatrix.h
    - MatrixFractional.h
  cre/
    - ARIMAX.h
    - RegressionModel.h
    - FeatureBuilder.h
    - PrepaymentModel.h
    - Mortgage.h

Source Files (src/):
  utils/
    - Benchmark.cpp
    - CPR.cpp
    - CSV.cpp
    - TransitionMatrix.cpp
    - MatrixFractional.cpp
  cre/
    - ARIMAX.cpp
    - RegressionModel.cpp
    - FeatureBuilder.cpp
    - Mortgage.cpp

Examples:
  - examples/run_arimax_infer.cpp

Test Data:
  - test_data/logistic_model.json
  - test_data/model_params.json
  - test_data/exog_future.json
  - test_data/transition_matrix.csv

Build Artifacts:
  - libprepayment.a (static library)
  - examples/run_arimax_infer (example binary)
EOF
