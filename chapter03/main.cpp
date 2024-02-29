#define RUN_BENCHMARK 0
#define RUN_UNIT_TEST 0



#if RUN_UNIT_TEST == 1
#include <iostream>
#include "tests/exploring_circular_array_test.hpp"

int main() {

    run_all_tests();

    std::cout << "Done..." << std::endl;
    return 0;
}
#elif RUN_UNIT_TEST == 0 && RUN_BENCHMARK == 0

#include "lockfree_limitorderbook.hpp"
#include <vector>
#include <thread>

std::vector<lockfree::Order> generate_random_orders_forlockfree(int num_orders) {
    std::vector<lockfree::Order> orders;
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < 100; ++i) {
        lockfree::Order order(id, price, 100);
        orders.push_back(order);
        id++;
        price += 0.01;
    }
    return orders;

}

int main(){
    // Create a new order book
    lockfree::LockFreeLimitOrderBook order_book(2, 100);

    // Prepare some orders
    std::vector<lockfree::Order> orders = generate_random_orders_forlockfree(80);

    // Create threads for add_order
    std::vector<std::thread> add_order_threads;
    for (int i = 0; i < 1; ++i) {
        add_order_threads.emplace_back([&]() {
            for (const auto& order : orders) {
                order_book.add_order(order, true);
            }
        });
    }
    // Create threads for get_best_bid
    std::vector<std::thread> get_best_bid_threads;
    for (int i = 0; i < 2; ++i) {
        get_best_bid_threads.emplace_back([&]() {
            for (int j = 0; j < 1; ++j) {
                order_book.get_best_bid();
            }
        });
    }

    std::cout << "Running for 10 sec...." << std::endl;
    sleep(10); //run for 10 seconds

    return 0;
}
#endif

#if RUN_BENCHMARK == 1
#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <iomanip>
#include <random>
#include <benchmark/benchmark.h>
#include "exploring_circular_array.hpp"
#include "exploring_hash_table.hpp"
#include "exploring_linked_list.hpp"
#include "exploring_queue.hpp"
#include "exploring_binary_tree.hpp"
#include "synchronized_limitorderbook.hpp"
#include "smartblocking_limitorderbook.hpp"
#include "lockfree_limitorderbook.hpp"

const int _LOB_DEPTH = 50;

static void AddOrder_CircularArray(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    circular_array::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    for (auto _ : state) {
        // Create a new order with incrementing id and price
        circular_array::Order order(id, price, 100);
        // Benchmark the add_order method
        lob.add_order(order, true);

        // Increment the id and price for the next order
        id++;
        price += 0.01;
    }

}
static void AddOrder_HashtTable(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
    hash_table::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    for (auto _ : state) {
        // Create a new order with incrementing id and price
        hash_table::Order order(id, price, 100);
        // Benchmark the add_order method
        lob.add_order(order, true);

        // Increment the id and price for the next order
        id++;
        price += 0.01;
    }
}
static void AddOrder_LinkedList(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
    linked_list::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    for (auto _ : state) {
        // Create a new order with incrementing id and price
        linked_list::Order order(id, price, 100);
        // Benchmark the add_order method
        lob.add_order(order, true);

        // Increment the id and price for the next order
        id++;
        price += 0.01;
    }

}
static void AddOrder_Queue(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
    queue::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    for (auto _ : state) {
        // Create a new order with incrementing id and price
        queue::Order order(id, price, 100);
        // Benchmark the add_order method
        lob.add_order(order, true);

        // Increment the id and price for the next order
        id++;
        price += 0.01;
    }

}
static void AddOrder_BinaryTree(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
    binary_tree::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    for (auto _ : state) {
        // Create a new order with incrementing id and price
        binary_tree::Order order(id, price, 100);
        // Benchmark the add_order method
        lob.add_order(order, true);

        // Increment the id and price for the next order
        id++;
        price += 0.01;
    }

}


static void DeleteOrder_CircularArray(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    circular_array::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        circular_array::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }

    // Random number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, _LOB_DEPTH);

    for (auto _ : state) {
        // Generate a random id
        int random_id = distribution(generator);
        // Create a new order with the random id
        circular_array::Order order(random_id, 10.01 + (random_id - 1) * 0.01, 100);
        // Benchmark the delete_order method
        lob.delete_order(order, true);

        // Generate a new random id for the new order
        int new_id = distribution(generator);
        // Create a new order with the new random id
        circular_array::Order new_order(new_id, 10.01 + (new_id - 1) * 0.01, 100);
        // Add the new order to the LOB
        lob.add_order(new_order, true);
    }
}
static void DeleteOrder_HashTable(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    hash_table::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        hash_table::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }

    // Random number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, _LOB_DEPTH);

    for (auto _ : state) {
        // Generate a random id
        int random_id = distribution(generator);
        // Create a new order with the random id
        hash_table::Order order(random_id, 10.01 + (random_id - 1) * 0.01, 100);
        // Benchmark the delete_order method
        lob.delete_order(order, true);

        // Generate a new random id for the new order
        int new_id = distribution(generator);
        // Create a new order with the new random id
        hash_table::Order new_order(new_id, 10.01 + (new_id - 1) * 0.01, 100);
        // Add the new order to the LOB
        lob.add_order(new_order, true);
    }
}
static void DeleteOrder_LinkedList(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    linked_list::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        linked_list::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }

    // Random number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, _LOB_DEPTH);

    for (auto _ : state) {
        // Generate a random id
        int random_id = distribution(generator);
        // Create a new order with the random id
        linked_list::Order order(random_id, 10.01 + (random_id - 1) * 0.01, 100);
        // Benchmark the delete_order method
        lob.delete_order(order, true);

        // Generate a new random id for the new order
        int new_id = distribution(generator);
        // Create a new order with the new random id
        linked_list::Order new_order(new_id, 10.01 + (new_id - 1) * 0.01, 100);
        // Add the new order to the LOB
        lob.add_order(new_order, true);
    }
}
static void DeleteOrder_Queue(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    queue::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        queue::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }

    // Random number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, _LOB_DEPTH);

    for (auto _ : state) {
        // Generate a random id
        int random_id = distribution(generator);
        // Create a new order with the random id
        queue::Order order(random_id, 10.01 + (random_id - 1) * 0.01, 100);
        // Benchmark the delete_order method
        lob.delete_order(order, true);

        // Generate a new random id for the new order
        int new_id = distribution(generator);
        // Create a new order with the new random id
        queue::Order new_order(new_id, 10.01 + (new_id - 1) * 0.01, 100);
        // Add the new order to the LOB
        lob.add_order(new_order, true);
    }
}
static void DeleteOrder_BinaryTree(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    binary_tree::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        binary_tree::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }

    // Random number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, _LOB_DEPTH);

    for (auto _ : state) {
        // Generate a random id
        int random_id = distribution(generator);
        // Create a new order with the random id
        binary_tree::Order order(random_id, 10.01 + (random_id - 1) * 0.01, 100);
        // Benchmark the delete_order method
        lob.delete_order(order, true);

        // Generate a new random id for the new order
        int new_id = distribution(generator);
        // Create a new order with the new random id
        binary_tree::Order new_order(new_id, 10.01 + (new_id - 1) * 0.01, 100);
        // Add the new order to the LOB
        lob.add_order(new_order, true);
    }
}


static void GetBestPrice_CircularArray(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    circular_array::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        circular_array::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }


    for (auto _ : state) {
        lob.get_best_bid();
    }
}
static void GetBestPrice_HashTable(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    hash_table::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        hash_table::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }


    for (auto _ : state) {
        lob.get_best_bid();
    }
}
static void GetBestPrice_LinkedList(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    linked_list::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        linked_list::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }


    for (auto _ : state) {
        lob.get_best_bid();
    }
}
static void GetBestPrice_Queue(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    queue::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        queue::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }


    for (auto _ : state) {
        lob.get_best_bid();
    }
}
static void GetBestPrice_BinaryTree(benchmark::State& state) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Set the CPU affinity to CPU 0

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }

    binary_tree::LimitOrderBook lob(2, _LOB_DEPTH);
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        binary_tree::Order order(id, price, 100);
        lob.add_order(order, true);
        id++;
        price += 0.01;
    }


    for (auto _ : state) {
        lob.get_best_bid();
    }
}


/*
BENCHMARK(AddOrder_BinaryTree);
BENCHMARK(AddOrder_HashtTable);
BENCHMARK(AddOrder_Queue);
BENCHMARK(AddOrder_LinkedList);
BENCHMARK(AddOrder_CircularArray);

BENCHMARK(DeleteOrder_BinaryTree);
BENCHMARK(DeleteOrder_HashTable);
BENCHMARK(DeleteOrder_Queue);
BENCHMARK(DeleteOrder_LinkedList);
BENCHMARK(DeleteOrder_CircularArray);


BENCHMARK(GetBestPrice_BinaryTree);
BENCHMARK(GetBestPrice_HashTable);
BENCHMARK(GetBestPrice_Queue);
BENCHMARK(GetBestPrice_LinkedList);
BENCHMARK(GetBestPrice_CircularArray);
*/

//BENCHMARK MULTI-THREADING for Circular Array
static std::vector<synchronized::Order> generate_random_orders(int num_orders) {
    std::vector<synchronized::Order> orders;
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        synchronized::Order order(id, price, 100);
        orders.push_back(order);
        id++;
        price += 0.01;
    }
    return orders;

}
static void BM_MultiThreadedAddOrderAndGetBestBid(benchmark::State& state) {
    // Create a new order book
    synchronized::SynchronizedLimitOrderBook order_book(2, 100);

    // Prepare some orders
    std::vector<synchronized::Order> orders = generate_random_orders(state.range(0));

    // Run the benchmark
    for (auto _ : state) {
        // Create threads for add_order
        std::vector<std::thread> add_order_threads;
        for (int i = 0; i < 2; ++i) {
            add_order_threads.emplace_back([&]() {
                for (const auto& order : orders) {
                    order_book.add_order(order, true);
                }
            });
        }

        // Create threads for get_best_bid
        std::vector<std::thread> get_best_bid_threads;
        for (int i = 0; i < state.range(1); ++i) {
            get_best_bid_threads.emplace_back([&]() {
                for (int j = 0; j < state.range(0); ++j) {
                    order_book.get_best_bid();
                }
            });
        }

        // Join all threads
        for (auto& thread : add_order_threads) {
            thread.join();
        }
        for (auto& thread : get_best_bid_threads) {
            thread.join();
        }
    }
}


static std::vector<smartblocking::Order> generate_random_orders_forsmartblocking(int num_orders) {
    std::vector<smartblocking::Order> orders;
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        smartblocking::Order order(id, price, 100);
        orders.push_back(order);
        id++;
        price += 0.01;
    }
    return orders;

}
static void BM_SmartBlockingAddOrderAndGetBestBid(benchmark::State& state) {
    // Create a new order book
    smartblocking::SmartBlockingLimitOrderBook order_book(2, 100);

    // Prepare some orders
    std::vector<smartblocking::Order> orders = generate_random_orders_forsmartblocking(state.range(0));

    // Run the benchmark
    for (auto _ : state) {
        // Create threads for add_order
        std::vector<std::thread> add_order_threads;
        for (int i = 0; i < 2; ++i) {
            add_order_threads.emplace_back([&]() {
                for (const auto& order : orders) {
                    order_book.add_order(order, true);
                }
            });
        }

        // Create threads for get_best_bid
        std::vector<std::thread> get_best_bid_threads;
        for (int i = 0; i < state.range(1); ++i) {
            get_best_bid_threads.emplace_back([&]() {
                for (int j = 0; j < state.range(0); ++j) {
                    order_book.get_best_bid();
                }
            });
        }

        // Join all threads
        for (auto& thread : add_order_threads) {
            thread.join();
        }
        for (auto& thread : get_best_bid_threads) {
            thread.join();
        }
    }
}

static std::vector<lockfree::Order> generate_random_orders_forlockfree(int num_orders) {
    std::vector<lockfree::Order> orders;
    // Variables to increment for each order
    int id = 1;
    double price = 10.01;

    // Initialize the LOB with orders
    for (int i = 0; i < _LOB_DEPTH; ++i) {
        lockfree::Order order(id, price, 100);
        orders.push_back(order);
        id++;
        price += 0.01;
    }
    return orders;

}
static void BM_LockFreeAddOrderAndGetBestBid(benchmark::State& state) {
    // Create a new order book
    lockfree::LockFreeLimitOrderBook order_book(2, 100);

    // Prepare some orders
    std::vector<lockfree::Order> orders = generate_random_orders_forlockfree(state.range(0));

    // Run the benchmark
    for (auto _ : state) {
        // Create threads for add_order
        std::vector<std::thread> add_order_threads;
        for (int i = 0; i < 2; ++i) {
            add_order_threads.emplace_back([&]() {
                for (const auto& order : orders) {
                    order_book.add_order(order, true);
                }
            });
        }

        // Create threads for get_best_bid
        std::vector<std::thread> get_best_bid_threads;
        for (int i = 0; i < state.range(1); ++i) {
            get_best_bid_threads.emplace_back([&]() {
                for (int j = 0; j < state.range(0); ++j) {
                    order_book.get_best_bid();
                }
            });
        }

        // Join all threads
        for (auto& thread : add_order_threads) {
            thread.join();
        }
        for (auto& thread : get_best_bid_threads) {
            thread.join();
        }
    }
}


// Register the benchmark
BENCHMARK(BM_MultiThreadedAddOrderAndGetBestBid)
    ->Args({1000, 10})  // 1000 orders, 10 reader threads
    ->Args({1000, 20});  // 1000 orders, 20 reader threads
BENCHMARK(BM_SmartBlockingAddOrderAndGetBestBid)
    ->Args({1000, 10})  // 1000 orders, 10 reader threads
    ->Args({1000, 20});  // 1000 orders, 20 reader threads

BENCHMARK(BM_LockFreeAddOrderAndGetBestBid)
    ->Args({1000, 10})  // 1000 orders, 10 reader threads
    ->Args({1000, 20});  // 1000 orders, 20 reader threads



BENCHMARK_MAIN();
#endif