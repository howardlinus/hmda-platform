#include "utils/TransitionMatrix.h"
#include "utils/CSV.h"
#include <stdexcept>

namespace utils {

TransitionMatrix::TransitionMatrix() : size_(0) {}

TransitionMatrix::TransitionMatrix(int size) : size_(size) {
    matrix_.resize(size, std::vector<double>(size, 0.0));
}

void TransitionMatrix::loadFromCSV(const std::string& filename) {
    CSVReader reader;
    auto rows = reader.read(filename);
    
    if (rows.empty()) {
        throw std::runtime_error("Empty CSV file");
    }
    
    size_ = rows.size();
    matrix_.resize(size_, std::vector<double>(size_, 0.0));
    
    for (int i = 0; i < size_; ++i) {
        if (static_cast<int>(rows[i].size()) < size_) {
            throw std::runtime_error("Inconsistent matrix dimensions in CSV");
        }
        for (int j = 0; j < size_; ++j) {
            try {
                matrix_[i][j] = std::stod(rows[i][j]);
            } catch (...) {
                throw std::runtime_error("Invalid number format in CSV");
            }
        }
    }
}

void TransitionMatrix::setMatrix(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) {
        size_ = 0;
        matrix_.clear();
        return;
    }
    
    size_ = matrix.size();
    matrix_ = matrix;
    
    // Validate square matrix
    for (const auto& row : matrix_) {
        if (static_cast<int>(row.size()) != size_) {
            throw std::runtime_error("Matrix must be square");
        }
    }
}

double TransitionMatrix::at(int i, int j) const {
    if (i < 0 || i >= size_ || j < 0 || j >= size_) {
        throw std::out_of_range("Matrix index out of range");
    }
    return matrix_[i][j];
}

void TransitionMatrix::set(int i, int j, double value) {
    if (i < 0 || i >= size_ || j < 0 || j >= size_) {
        throw std::out_of_range("Matrix index out of range");
    }
    matrix_[i][j] = value;
}

} // namespace utils
