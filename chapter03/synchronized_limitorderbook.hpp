#include <shared_mutex>
#include "exploring_circular_array.hpp"

namespace synchronized
{

using namespace circular_array;

class SynchronizedLimitOrderBook : public LimitOrderBook {
private:
    std::shared_mutex lob_mutex;
public:
    SynchronizedLimitOrderBook(int precision, int depth): LimitOrderBook(precision, depth){}
    void add_order(const Order& order, bool is_bid) override {
        std::unique_lock<std::shared_mutex> lock(lob_mutex);
        // Call the base class method
        LimitOrderBook::add_order(order, is_bid);
    }

    Order get_best_bid() override {
        std::shared_lock<std::shared_mutex> lock(lob_mutex);
        // Call the base class method
        return LimitOrderBook::get_best_bid();
    }
};
}

