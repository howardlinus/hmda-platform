#!/bin/bash
# Script to fetch vendor headers (nlohmann::json, Eigen)

set -e

VENDOR_DIR="vendor"
mkdir -p "$VENDOR_DIR"

echo "Fetching vendor headers..."

# Fetch nlohmann/json (single header)
echo "Fetching nlohmann/json..."
NLOHMANN_DIR="$VENDOR_DIR/nlohmann"
mkdir -p "$NLOHMANN_DIR"

if [ ! -f "$NLOHMANN_DIR/json.hpp" ]; then
    curl -L -o "$NLOHMANN_DIR/json.hpp" \
        https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
    echo "  Downloaded nlohmann/json.hpp"
else
    echo "  nlohmann/json.hpp already exists"
fi

# Fetch Eigen (headers only)
echo "Fetching Eigen..."
EIGEN_DIR="$VENDOR_DIR/eigen"

if [ ! -d "$EIGEN_DIR/Eigen" ]; then
    mkdir -p "$EIGEN_DIR"
    cd "$EIGEN_DIR"
    
    # Download and extract Eigen
    curl -L -o eigen.tar.gz \
        https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
    tar xzf eigen.tar.gz
    mv eigen-3.4.0/Eigen .
    rm -rf eigen-3.4.0 eigen.tar.gz
    
    cd - > /dev/null
    echo "  Downloaded Eigen headers"
else
    echo "  Eigen headers already exist"
fi

echo "Vendor headers fetched successfully!"
echo ""
echo "To build with Eigen support, use: make HAVE_EIGEN=1"
