#include "../include/utils/TransitionMatrix.h"
#include "../include/utils/CSV.h"
#include <stdexcept>

TransitionMatrix::TransitionMatrix() {}

TransitionMatrix::TransitionMatrix(const std::vector<std::vector<double>>& matrix) 
    : matrix_(matrix) {}

void TransitionMatrix::loadFromCSV(const std::string& filename) {
    auto data = CSVReader::read(filename);
    matrix_.clear();
    
    for (const auto& row : data) {
        std::vector<double> numeric_row;
        for (const auto& cell : row) {
            if (!cell.empty()) {
                try {
                    numeric_row.push_back(std::stod(cell));
                } catch (...) {
                    numeric_row.push_back(0.0);
                }
            }
        }
        if (!numeric_row.empty()) {
            matrix_.push_back(numeric_row);
        }
    }
}

std::vector<double> TransitionMatrix::apply(const std::vector<double>& state) const {
    if (matrix_.empty() || state.size() != matrix_[0].size()) {
        return state;
    }
    
    std::vector<double> result(matrix_.size(), 0.0);
    
    for (size_t i = 0; i < matrix_.size(); ++i) {
        for (size_t j = 0; j < matrix_[i].size() && j < state.size(); ++j) {
            result[i] += matrix_[i][j] * state[j];
        }
    }
    
    return result;
}
