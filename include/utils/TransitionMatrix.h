#ifndef TRANSITION_MATRIX_H
#define TRANSITION_MATRIX_H

#include <vector>
#include <string>

class TransitionMatrix {
public:
    TransitionMatrix();
    TransitionMatrix(const std::vector<std::vector<double>>& matrix);
    
    void loadFromCSV(const std::string& filename);
    std::vector<double> apply(const std::vector<double>& state) const;
    
    size_t size() const { return matrix_.size(); }
    const std::vector<std::vector<double>>& getMatrix() const { return matrix_; }
    
private:
    std::vector<std::vector<double>> matrix_;
};

#endif // TRANSITION_MATRIX_H
