#include <vector>
#include <numeric> // For std::accumulate

class loop_unrolling_and_tiling_example {
public:
    loop_unrolling_and_tiling_example(std::vector<int>& data) : data_(data) {}

    // Standard loop for summing elements
    int loop_unoptimized() {
        int sum = 0;
        for (size_t i = 0; i < data_.size(); ++i) {
            sum += data_[i];
        }
        return sum;
    }

    // Optimized loop with loop unrolling
    int loop_optimized_unrolling() {
        int sum = 0;
        size_t i = 0;
        // Process 4 elements at a time
        for (; i + 3 < data_.size(); i += 4) {
            sum += data_[i] + data_[i + 1] + data_[i + 2] + data_[i + 3];
        }

        // Handle remaining elements
        for (; i < data_.size(); ++i) {
            sum += data_[i];
        }
        return sum;
    }

    void matrix_multiply(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b, std::vector<std::vector<int>>& c) {
        size_t N = a.size();
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                c[i][j] = 0;
                for (size_t k = 0; k < N; ++k) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }
    void matrix_multiply_tiled(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b, std::vector<std::vector<int>>& c) {
        size_t N = a.size();
        const size_t blockSize = 32; // Choose a block size that fits well in your cache
        for (size_t i = 0; i < N; i += blockSize) {
            for (size_t j = 0; j < N; j += blockSize) {
                for (size_t k = 0; k < N; k += blockSize) {
                    // Process a block
                    for (size_t ii = i; ii < std::min(i + blockSize, N); ++ii) {
                        for (size_t jj = j; jj < std::min(j + blockSize, N); ++jj) {
                            for (size_t kk = k; kk < std::min(k + blockSize, N); ++kk) {
                                c[ii][jj] += a[ii][kk] * b[kk][jj];
                            }
                        }
                    }
                }
            }
        }
    }


private:
    std::vector<int> data_;
};
