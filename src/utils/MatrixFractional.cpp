#include "utils/MatrixFractional.h"
#include <cmath>
#include <stdexcept>

namespace utils {

MatrixFractional::MatrixFractional() {}

std::vector<std::vector<double>> MatrixFractional::power(
    const std::vector<std::vector<double>>& matrix,
    double alpha) const {
    
    if (matrix.empty()) {
        return matrix;
    }
    
    int n = matrix.size();
    for (const auto& row : matrix) {
        if (static_cast<int>(row.size()) != n) {
            throw std::invalid_argument("Matrix must be square");
        }
    }
    
#ifdef HAVE_EIGEN
    return powerWithEigen(matrix, alpha);
#else
    return powerWithHeuristic(matrix, alpha);
#endif
}

#ifdef HAVE_EIGEN
std::vector<std::vector<double>> MatrixFractional::powerWithEigen(
    const std::vector<std::vector<double>>& matrix,
    double alpha) const {
    
    int n = matrix.size();
    Eigen::MatrixXd eigen_mat(n, n);
    
    // Convert to Eigen matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            eigen_mat(i, j) = matrix[i][j];
        }
    }
    
    // Compute eigendecomposition
    Eigen::EigenSolver<Eigen::MatrixXd> solver(eigen_mat);
    Eigen::VectorXcd eigenvalues = solver.eigenvalues();
    Eigen::MatrixXcd eigenvectors = solver.eigenvectors();
    
    // Compute eigenvalues^alpha
    Eigen::VectorXcd eigenvalues_power(n);
    for (int i = 0; i < n; ++i) {
        eigenvalues_power(i) = std::pow(eigenvalues(i), alpha);
    }
    
    // Reconstruct: V * D^alpha * V^(-1)
    Eigen::MatrixXcd D = eigenvalues_power.asDiagonal();
    Eigen::MatrixXcd result_complex = eigenvectors * D * eigenvectors.inverse();
    
    // Convert back to std::vector (take real part)
    std::vector<std::vector<double>> result(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = result_complex(i, j).real();
        }
    }
    
    return result;
}
#endif

std::vector<std::vector<double>> MatrixFractional::powerWithHeuristic(
    const std::vector<std::vector<double>>& matrix,
    double alpha) const {
    
    int n = matrix.size();
    
    // Special cases
    if (std::abs(alpha) < 1e-10) {
        return identity(n);
    }
    
    if (std::abs(alpha - 1.0) < 1e-10) {
        return matrix;
    }
    
    // For integer powers, use repeated multiplication
    if (std::abs(alpha - std::round(alpha)) < 1e-10) {
        int k = static_cast<int>(std::round(alpha));
        if (k > 0) {
            auto result = matrix;
            for (int i = 1; i < k; ++i) {
                result = multiply(result, matrix);
            }
            return result;
        }
    }
    
    // Heuristic approximation for fractional powers
    // Use Taylor series: A^alpha ≈ I + alpha*(A - I) + O(alpha^2)
    // This is a first-order approximation suitable for alpha close to 1
    auto I = identity(n);
    std::vector<std::vector<double>> result(n, std::vector<double>(n));
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = I[i][j] + alpha * (matrix[i][j] - I[i][j]);
        }
    }
    
    return result;
}

std::vector<std::vector<double>> MatrixFractional::multiply(
    const std::vector<std::vector<double>>& A,
    const std::vector<std::vector<double>>& B) const {
    
    int n = A.size();
    std::vector<std::vector<double>> result(n, std::vector<double>(n, 0.0));
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return result;
}

std::vector<std::vector<double>> MatrixFractional::identity(int size) const {
    std::vector<std::vector<double>> result(size, std::vector<double>(size, 0.0));
    for (int i = 0; i < size; ++i) {
        result[i][i] = 1.0;
    }
    return result;
}

} // namespace utils
