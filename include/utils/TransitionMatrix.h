#ifndef TRANSITION_MATRIX_H
#define TRANSITION_MATRIX_H

#include <vector>
#include <string>

namespace utils {

class TransitionMatrix {
public:
    TransitionMatrix();
    TransitionMatrix(int size);
    
    // Load from CSV file
    void loadFromCSV(const std::string& filename);
    
    // Set matrix data directly
    void setMatrix(const std::vector<std::vector<double>>& matrix);
    
    // Get matrix data
    const std::vector<std::vector<double>>& getMatrix() const { return matrix_; }
    
    // Get matrix size
    int getSize() const { return size_; }
    
    // Get element at (i, j)
    double at(int i, int j) const;
    
    // Set element at (i, j)
    void set(int i, int j, double value);
    
private:
    std::vector<std::vector<double>> matrix_;
    int size_;
};

} // namespace utils

#endif // TRANSITION_MATRIX_H
