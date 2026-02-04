#ifndef MATRIX_FRACTIONAL_H
#define MATRIX_FRACTIONAL_H

#include <vector>

// MatrixFractional: Eigen-optional implementation
class MatrixFractional {
public:
    static std::vector<std::vector<double>> power(
        const std::vector<std::vector<double>>& matrix, 
        double exponent
    );
    
private:
    static std::vector<std::vector<double>> powerEigen(
        const std::vector<std::vector<double>>& matrix, 
        double exponent
    );
    
    static std::vector<std::vector<double>> powerFallback(
        const std::vector<std::vector<double>>& matrix, 
        double exponent
    );
    
    static std::vector<std::vector<double>> matrixMultiply(
        const std::vector<std::vector<double>>& a,
        const std::vector<std::vector<double>>& b
    );
};

#endif // MATRIX_FRACTIONAL_H
