#include "../include/utils/MatrixFractional.h"
#include <cmath>
#include <stdexcept>

// Check if Eigen is available
#ifdef HAVE_EIGEN
#include <Eigen/Dense>
#endif

std::vector<std::vector<double>> MatrixFractional::power(
    const std::vector<std::vector<double>>& matrix, 
    double exponent
) {
#ifdef HAVE_EIGEN
    return powerEigen(matrix, exponent);
#else
    return powerFallback(matrix, exponent);
#endif
}

#ifdef HAVE_EIGEN
std::vector<std::vector<double>> MatrixFractional::powerEigen(
    const std::vector<std::vector<double>>& matrix, 
    double exponent
) {
    size_t n = matrix.size();
    if (n == 0) return matrix;
    
    // Convert to Eigen matrix
    Eigen::MatrixXd eigen_mat(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            eigen_mat(i, j) = matrix[i][j];
        }
    }
    
    // Compute eigendecomposition
    Eigen::EigenSolver<Eigen::MatrixXd> solver(eigen_mat);
    Eigen::MatrixXcd V = solver.eigenvectors();
    Eigen::VectorXcd d = solver.eigenvalues();
    
    // Raise eigenvalues to the power
    for (int i = 0; i < d.size(); ++i) {
        d(i) = std::pow(d(i), exponent);
    }
    
    // Reconstruct: V * D^exp * V^-1
    Eigen::MatrixXcd D = d.asDiagonal();
    Eigen::MatrixXcd result_complex = V * D * V.inverse();
    
    // Convert back to std::vector (take real part)
    std::vector<std::vector<double>> result(n, std::vector<double>(n));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i][j] = result_complex(i, j).real();
        }
    }
    
    return result;
}
#else
std::vector<std::vector<double>> MatrixFractional::powerEigen(
    const std::vector<std::vector<double>>& matrix, 
    double exponent
) {
    // This should never be called when Eigen is not available
    return powerFallback(matrix, exponent);
}
#endif

std::vector<std::vector<double>> MatrixFractional::powerFallback(
    const std::vector<std::vector<double>>& matrix, 
    double exponent
) {
    // Simple heuristic: for fractional powers between 0 and 1,
    // interpolate between identity and the matrix itself
    size_t n = matrix.size();
    if (n == 0) return matrix;
    
    std::vector<std::vector<double>> result(n, std::vector<double>(n, 0.0));
    
    if (exponent == 0.0) {
        // Return identity matrix
        for (size_t i = 0; i < n; ++i) {
            result[i][i] = 1.0;
        }
        return result;
    }
    
    if (exponent == 1.0) {
        return matrix;
    }
    
    if (exponent > 0.0 && exponent < 1.0) {
        // Linear interpolation between identity and matrix
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                double identity_val = (i == j) ? 1.0 : 0.0;
                result[i][j] = identity_val * (1.0 - exponent) + matrix[i][j] * exponent;
            }
        }
        return result;
    }
    
    if (exponent > 1.0) {
        // For integer parts, multiply the matrix
        int int_part = static_cast<int>(exponent);
        double frac_part = exponent - int_part;
        
        // Start with identity
        result = std::vector<std::vector<double>>(n, std::vector<double>(n, 0.0));
        for (size_t i = 0; i < n; ++i) {
            result[i][i] = 1.0;
        }
        
        // Multiply int_part times
        for (int k = 0; k < int_part; ++k) {
            result = matrixMultiply(result, matrix);
        }
        
        // Apply fractional part if non-zero
        if (frac_part > 0.0) {
            auto frac_matrix = powerFallback(matrix, frac_part);
            result = matrixMultiply(result, frac_matrix);
        }
        
        return result;
    }
    
    // For negative exponents or other cases, return the matrix as-is
    return matrix;
}

std::vector<std::vector<double>> MatrixFractional::matrixMultiply(
    const std::vector<std::vector<double>>& a,
    const std::vector<std::vector<double>>& b
) {
    size_t n = a.size();
    if (n == 0 || b.size() == 0) return a;
    
    size_t m = b[0].size();
    std::vector<std::vector<double>> result(n, std::vector<double>(m, 0.0));
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            for (size_t k = 0; k < b.size(); ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
    return result;
}
