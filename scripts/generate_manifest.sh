#!/bin/bash

# Script to generate a manifest of vendored dependencies

VENDOR_DIR="vendor"
MANIFEST_FILE="${VENDOR_DIR}/MANIFEST.txt"

echo "Generating vendor manifest..."

cat > "${MANIFEST_FILE}" << 'EOF'
HMDA Platform - Prepayment Library Vendor Dependencies
========================================================

This directory contains vendored third-party dependencies:

1. nlohmann/json (MIT License)
   - URL: https://github.com/nlohmann/json
   - Version: 3.11.2
   - License: MIT
   - Purpose: JSON parsing in examples only
   - File: nlohmann/json.hpp

2. Eigen (MPL2 License) - OPTIONAL
   - URL: https://gitlab.com/libeigen/eigen
   - Version: 3.4.0
   - License: Mozilla Public License 2.0
   - Purpose: Matrix operations for MatrixFractional power
   - Directory: eigen/Eigen/

Note: These dependencies are fetched by scripts/fetch_vendors.sh
      and are not included in the git repository.

To fetch dependencies:
    ./scripts/fetch_vendors.sh

EOF

echo "✓ Manifest generated at ${MANIFEST_FILE}"
