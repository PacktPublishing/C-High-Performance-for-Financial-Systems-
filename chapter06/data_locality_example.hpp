#include <vector>
#include <random>

class data_locality_example  {
public:
    data_locality_example(int size) : matrix_size(size), matrix(matrix_size, std::vector<int>(matrix_size)) 
    {
        fillMatrix();
    }


    void run_unoptimized() {
        for (int i = 0; i < matrix_size; ++i) {
            for (int j = 0; j < matrix_size; ++j) {
                matrix[j][i] = performCalculation();
            }
        }
    }

    void run_optimized() {
        for (int i = 0; i < matrix_size; ++i) {
            for (int j = 0; j < matrix_size; ++j) {
                matrix[i][j] = performCalculation();
            }
        }
    }

private:
    int matrix_size;
    std::vector<std::vector<int>> matrix;

    void fillMatrix() {
        for (int i = 0; i < matrix_size; ++i) {
            for (int j = 0; j < matrix_size; ++j) {
                matrix[i][j] = generateData(i, j);
            }
        }
    }
    int generateData(int row, int col) {
        // Simple example: populate with a function of the indices
        return row * col; 
    }
    int performCalculation() {
        // Example calculation (can be replaced with any other logic)
        // Simple arithmetic operation for demonstration
        //return rand() % 100;  // returns a pseudo-random number between 0 and 99
        return 10 * 2.02;
    }
};
