#!/bin/bash

# Script to fetch vendor dependencies (nlohmann/json and Eigen)

VENDOR_DIR="vendor"
NLOHMANN_DIR="${VENDOR_DIR}/nlohmann"
EIGEN_DIR="${VENDOR_DIR}/eigen"

mkdir -p "${NLOHMANN_DIR}"
mkdir -p "${EIGEN_DIR}"

echo "Fetching nlohmann/json..."
if [ ! -f "${NLOHMANN_DIR}/json.hpp" ]; then
    curl -L -o "${NLOHMANN_DIR}/json.hpp" \
        "https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp"
    if [ $? -eq 0 ]; then
        echo "✓ nlohmann/json fetched successfully"
    else
        echo "✗ Failed to fetch nlohmann/json"
        exit 1
    fi
else
    echo "✓ nlohmann/json already exists"
fi

echo ""
echo "Fetching Eigen (optional)..."
if [ ! -d "${EIGEN_DIR}/Eigen" ]; then
    # Download and extract Eigen
    EIGEN_VERSION="3.4.0"
    EIGEN_URL="https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz"
    
    curl -L -o "/tmp/eigen.tar.gz" "${EIGEN_URL}"
    if [ $? -eq 0 ]; then
        tar -xzf /tmp/eigen.tar.gz -C /tmp/
        cp -r "/tmp/eigen-${EIGEN_VERSION}/Eigen" "${EIGEN_DIR}/"
        rm -rf "/tmp/eigen.tar.gz" "/tmp/eigen-${EIGEN_VERSION}"
        echo "✓ Eigen fetched successfully"
    else
        echo "⚠ Failed to fetch Eigen (optional)"
    fi
else
    echo "✓ Eigen already exists"
fi

echo ""
echo "Vendor dependencies setup complete!"
echo "Run 'make lib' to build the library"
echo "Run 'make example' to build the example (requires nlohmann/json)"
