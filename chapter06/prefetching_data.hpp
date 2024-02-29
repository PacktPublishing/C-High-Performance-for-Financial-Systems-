#include <vector>
#include <iostream>
#include <xmmintrin.h>

class prefetching_data {
public:
    prefetching_data() {        
        // Initialize a large vector with dummy data for the demonstration
        data_.resize(100000000);
        for (int i = 0; i < 100000000; ++i) {
            data_[i] = i;
        }
    }

    // Run without prefetching
    int run_unoptimized() {
        int sum = 0;
        for (size_t i = 0; i < data_.size(); ++i) {
            sum += data_[i];
        }
        return sum;        
    }

    // Run with prefetching
    int run_optimized() {
        int sum = 0;
        size_t dataSize = data_.size();
        // Prefetch distance determined experimentally or based on cache line size
        constexpr size_t prefetchDistance = 32 / sizeof(int);

        for (size_t i = 0; i < dataSize; ++i) {
            // Prefetch data ahead
            if (i + prefetchDistance < dataSize) {
                __builtin_prefetch(&data_[i + prefetchDistance], 0, 1);
            }
            sum += data_[i];
        }
        return sum;
        
    }

private:
    std::vector<int> data_;
};
