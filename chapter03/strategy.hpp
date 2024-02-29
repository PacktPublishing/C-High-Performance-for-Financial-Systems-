#include "exploring_circular_array.hpp"
#include <thread>
#include <sched.h>

namespace strategy {

    using namespace circular_array;

    class StrategyModule {
    private:
        LimitOrderBook& orderBook;

    public:
        StrategyModule(LimitOrderBook& lob) : orderBook(lob) {}

        void run() {
            // Pin this thread to the first CPU core
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
            pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

            // Busy waiting loop
            while (true) {
                Order best_bid = orderBook.get_best_bid();
                Order best_offer = orderBook.get_best_offer();

                // Dummy strategy logic
                if (best_bid.price > 100 && best_offer.price < 200) {
                    // Place a buy order
                    Order buy_order;
                    buy_order.price = best_offer.price - 1;
                    buy_order.quantity = 100;
                    orderBook.add_order(buy_order, true);
                    std::cout << "Placed buy order at price: " << buy_order.price << "\n";
                }
                else if (best_offer.price > 300 && best_bid.price < 400) {
                    // Place a sell order
                    Order sell_order;
                    sell_order.price = best_bid.price + 1;
                    sell_order.quantity = 100;
                    orderBook.add_order(sell_order, false);
                    std::cout << "Placed sell order at price: " << sell_order.price << "\n";
                }
            }
        }
    };

} // namespace strategy
