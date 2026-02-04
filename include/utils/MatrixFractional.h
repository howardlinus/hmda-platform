#ifndef UTILS_MATRIX_FRACTIONAL_H
#define UTILS_MATRIX_FRACTIONAL_H

#include <vector>
#include <cmath>
#include <stdexcept>

// Check if Eigen is available
#ifdef USE_EIGEN
#include <Eigen/Dense>
#endif

namespace util {

class MatrixFractional {
public:
    // Compute matrix raised to fractional power
    // Uses Eigen if available, otherwise uses a simple heuristic fallback
    static std::vector<std::vector<double>> power(
        const std::vector<std::vector<double>>& matrix,
        double exponent) {
        
        if (matrix.empty()) {
            throw std::runtime_error("Cannot compute power of empty matrix");
        }

        size_t n = matrix.size();
        for (const auto& row : matrix) {
            if (row.size() != n) {
                throw std::runtime_error("Matrix must be square");
            }
        }

#ifdef USE_EIGEN
        return powerEigen(matrix, exponent);
#else
        return powerFallback(matrix, exponent);
#endif
    }

private:
#ifdef USE_EIGEN
    static std::vector<std::vector<double>> powerEigen(
        const std::vector<std::vector<double>>& matrix,
        double exponent) {
        
        size_t n = matrix.size();
        Eigen::MatrixXd M(n, n);
        
        // Convert to Eigen matrix
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                M(i, j) = matrix[i][j];
            }
        }

        // Compute eigendecomposition
        Eigen::EigenSolver<Eigen::MatrixXd> solver(M);
        Eigen::MatrixXcd V = solver.eigenvectors();
        Eigen::VectorXcd d = solver.eigenvalues();

        // Raise eigenvalues to power
        for (int i = 0; i < d.size(); ++i) {
            d(i) = std::pow(d(i), exponent);
        }

        // Reconstruct: M^p = V * D^p * V^-1
        Eigen::MatrixXcd result = V * d.asDiagonal() * V.inverse();

        // Convert back to std::vector (take real part)
        std::vector<std::vector<double>> output(n, std::vector<double>(n));
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                output[i][j] = result(i, j).real();
            }
        }

        return output;
    }
#endif

    // Fallback heuristic for matrix power when Eigen is not available
    // Uses simple interpolation between identity and the matrix
    static std::vector<std::vector<double>> powerFallback(
        const std::vector<std::vector<double>>& matrix,
        double exponent) {
        
        size_t n = matrix.size();
        std::vector<std::vector<double>> result(n, std::vector<double>(n, 0.0));

        if (std::abs(exponent) < 1e-10) {
            // M^0 = Identity
            for (size_t i = 0; i < n; ++i) {
                result[i][i] = 1.0;
            }
            return result;
        }

        if (std::abs(exponent - 1.0) < 1e-10) {
            // M^1 = M
            return matrix;
        }

        // Simple heuristic: linear interpolation for fractional powers
        // M^p ≈ (1-p)*I + p*M for p in [0,1]
        // For p > 1, use repeated multiplication (approximate)
        if (exponent >= 0.0 && exponent <= 1.0) {
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    double identity = (i == j) ? 1.0 : 0.0;
                    result[i][j] = (1.0 - exponent) * identity + exponent * matrix[i][j];
                }
            }
        } else if (exponent > 1.0) {
            // For integer part, do repeated multiplication
            int int_part = static_cast<int>(exponent);
            double frac_part = exponent - int_part;

            result = matrix;
            for (int k = 1; k < int_part; ++k) {
                result = multiplyMatrices(result, matrix);
            }

            if (frac_part > 1e-10) {
                auto frac_matrix = powerFallback(matrix, frac_part);
                result = multiplyMatrices(result, frac_matrix);
            }
        } else {
            // Negative exponent: use inverse (not implemented in fallback)
            throw std::runtime_error("Negative exponent not supported in fallback mode");
        }

        return result;
    }

    // Helper: multiply two matrices
    static std::vector<std::vector<double>> multiplyMatrices(
        const std::vector<std::vector<double>>& A,
        const std::vector<std::vector<double>>& B) {
        
        size_t n = A.size();
        std::vector<std::vector<double>> result(n, std::vector<double>(n, 0.0));

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                for (size_t k = 0; k < n; ++k) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        return result;
    }
};

// Helper function to validate transition matrix
inline bool validateTransitionMatrix(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) return false;

    const double tolerance = 1e-6;
    for (const auto& row : matrix) {
        double sum = 0.0;
        for (double val : row) {
            if (val < 0.0 || val > 1.0) return false;
            sum += val;
        }
        if (std::abs(sum - 1.0) > tolerance) return false;
    }
    return true;
}

} // namespace util

#endif // UTILS_MATRIX_FRACTIONAL_H
