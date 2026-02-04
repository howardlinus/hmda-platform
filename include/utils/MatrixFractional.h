#ifndef MATRIX_FRACTIONAL_H
#define MATRIX_FRACTIONAL_H

#include <vector>

// Check if Eigen is available
#ifdef HAVE_EIGEN
#include <Eigen/Dense>
#endif

namespace utils {

class MatrixFractional {
public:
    MatrixFractional();
    
    // Compute matrix fractional power A^alpha
    // Uses Eigen if available, otherwise uses robust heuristic fallback
    std::vector<std::vector<double>> power(
        const std::vector<std::vector<double>>& matrix,
        double alpha) const;
    
private:
#ifdef HAVE_EIGEN
    std::vector<std::vector<double>> powerWithEigen(
        const std::vector<std::vector<double>>& matrix,
        double alpha) const;
#endif
    
    std::vector<std::vector<double>> powerWithHeuristic(
        const std::vector<std::vector<double>>& matrix,
        double alpha) const;
    
    // Helper: matrix multiplication
    std::vector<std::vector<double>> multiply(
        const std::vector<std::vector<double>>& A,
        const std::vector<std::vector<double>>& B) const;
    
    // Helper: identity matrix
    std::vector<std::vector<double>> identity(int size) const;
};

} // namespace utils

#endif // MATRIX_FRACTIONAL_H
