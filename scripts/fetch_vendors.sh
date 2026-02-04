#!/bin/bash
# Script to fetch vendor header libraries

set -e

VENDOR_DIR="vendor"
mkdir -p "${VENDOR_DIR}"

echo "Fetching vendor headers..."

# Fetch nlohmann/json (required for example only)
NLOHMANN_VERSION="3.11.2"
NLOHMANN_URL="https://github.com/nlohmann/json/releases/download/v${NLOHMANN_VERSION}/json.hpp"
NLOHMANN_TARGET="${VENDOR_DIR}/nlohmann"

mkdir -p "${NLOHMANN_TARGET}"
if [ ! -f "${NLOHMANN_TARGET}/json.hpp" ]; then
    echo "Downloading nlohmann/json v${NLOHMANN_VERSION}..."
    curl -L -o "${NLOHMANN_TARGET}/json.hpp" "${NLOHMANN_URL}"
    echo "Downloaded nlohmann/json to ${NLOHMANN_TARGET}/json.hpp"
else
    echo "nlohmann/json already exists at ${NLOHMANN_TARGET}/json.hpp"
fi

# Fetch Eigen (optional, for MatrixFractional optimization)
EIGEN_VERSION="3.4.0"
EIGEN_URL="https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz"
EIGEN_TARGET="${VENDOR_DIR}/eigen"

if [ ! -d "${EIGEN_TARGET}/Eigen" ]; then
    echo "Downloading Eigen v${EIGEN_VERSION}..."
    curl -L -o /tmp/eigen.tar.gz "${EIGEN_URL}"
    mkdir -p "${EIGEN_TARGET}"
    tar -xzf /tmp/eigen.tar.gz -C /tmp
    mv /tmp/eigen-${EIGEN_VERSION}/Eigen "${EIGEN_TARGET}/"
    rm -rf /tmp/eigen.tar.gz /tmp/eigen-${EIGEN_VERSION}
    echo "Downloaded Eigen to ${EIGEN_TARGET}/Eigen"
else
    echo "Eigen already exists at ${EIGEN_TARGET}/Eigen"
fi

echo "Vendor headers fetched successfully!"
echo ""
echo "To enable Eigen optimization in MatrixFractional, add -DUSE_EIGEN to CXXFLAGS in Makefile"
