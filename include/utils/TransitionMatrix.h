#ifndef UTILS_TRANSITION_MATRIX_H
#define UTILS_TRANSITION_MATRIX_H

#include <vector>
#include <stdexcept>
#include <cmath>

namespace util {

class TransitionMatrix {
public:
    TransitionMatrix() = default;
    
    explicit TransitionMatrix(const std::vector<std::vector<double>>& matrix)
        : matrix_(matrix) {
        if (!validateTransitionMatrix(matrix_)) {
            throw std::runtime_error("Invalid transition matrix: rows must sum to 1.0");
        }
    }

    // Get matrix dimensions
    size_t rows() const { return matrix_.size(); }
    size_t cols() const { return matrix_.empty() ? 0 : matrix_[0].size(); }

    // Access matrix element
    double operator()(size_t i, size_t j) const {
        return matrix_[i][j];
    }

    // Get full matrix
    const std::vector<std::vector<double>>& getMatrix() const {
        return matrix_;
    }

    // Set matrix with validation
    void setMatrix(const std::vector<std::vector<double>>& matrix) {
        if (!validateTransitionMatrix(matrix)) {
            throw std::runtime_error("Invalid transition matrix: rows must sum to 1.0");
        }
        matrix_ = matrix;
    }

    // Validate transition matrix (rows sum to ~1.0)
    static bool validateTransitionMatrix(const std::vector<std::vector<double>>& matrix) {
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

private:
    std::vector<std::vector<double>> matrix_;
};

} // namespace util

#endif // UTILS_TRANSITION_MATRIX_H
