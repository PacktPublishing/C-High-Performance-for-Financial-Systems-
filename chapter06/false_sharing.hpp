#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

// Define a structure that may cause false sharing
struct DataUnoptimized {
    volatile int a;
    volatile int b;
};

// Define a structure with padding to avoid false sharing
struct DataOptimized {
    volatile int a;
    char pad[60]; // Assuming a 64-byte cache line size - sizeof(int) = 4 bytes
    volatile int b;
};

class false_sharing {
public:
    false_sharing() : unoptimizedData(), optimizedData() {
        unoptimizedData.a = 0;
        unoptimizedData.b = 0;
        optimizedData.a = 0;
        optimizedData.b = 0;
    }


    void run_unoptimized() {
        std::thread t1([&] { for (int i = 0; i < 10000000; ++i) ++unoptimizedData.a; });
        std::thread t2([&] { for (int i = 0; i < 10000000; ++i) ++unoptimizedData.b; });
        t1.join();
        t2.join();
        std::cout << "Unoptimized Data a: " << unoptimizedData.a << ", b: " << unoptimizedData.b << std::endl;
    }

    void run_optimized() {
        std::thread t1([&] { for (int i = 0; i < 10000000; ++i) ++optimizedData.a; });
        std::thread t2([&] { for (int i = 0; i < 10000000; ++i) ++optimizedData.b; });
        t1.join();
        t2.join();
        std::cout << "Optimized Data a: " << optimizedData.a << ", b: " << optimizedData.b << std::endl;
    }

private:
    DataUnoptimized unoptimizedData;
    DataOptimized optimizedData;
};
