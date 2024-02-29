#include <vector>
#include <iostream>

// Define a simple Point structure without cache line alignment
struct PointUnaligned {
    double x;
    double y;
};

// Define a Point structure with cache line alignment
struct __attribute__((aligned(16))) PointAligned {
    double x;
    double y;
};


class cache_line_aligned {
public:
    // Constructor initializes both aligned and unaligned data
    cache_line_aligned() : pointsUnaligned(numPoints), pointsAligned(numPoints) {
        
        for (size_t i = 0; i < numPoints; ++i) {
            // Initialize with some values
            pointsUnaligned[i] = {i * 1.0, i * 2.0};
            pointsAligned[i] = {i * 1.0, i * 2.0};
        }
    }

    // Method to process points without cache line alignment
    void run_unoptimized() {
        double sumX = 0.0, sumY = 0.0;
        for (const auto& point : pointsUnaligned) {
            sumX += point.x;
            sumY += point.y;
        }
        std::cout << "Unoptimized Sum X: " << sumX << ", Sum Y: " << sumY << std::endl;
    }


    // Method to process points with cache line alignment
    void run_optimized() {
        double sumX = 0.0, sumY = 0.0;
        for (const auto& point : pointsAligned) {
            sumX += point.x;
            sumY += point.y;
        }
        std::cout << "Optimized Sum X: " << sumX << ", Sum Y: " << sumY << std::endl;
    }

private:
    const size_t numPoints = 1000000;
    std::vector<PointUnaligned> pointsUnaligned;
    std::vector<PointAligned> pointsAligned;
};
