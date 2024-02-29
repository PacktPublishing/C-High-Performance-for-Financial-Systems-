#include <shared_mutex>
#include "exploring_circular_array.hpp"
#include <tbb/concurrent_vector.h>
namespace smartblocking
{

using namespace circular_array;

class SmartBlockingLimitOrderBook : public LimitOrderBook {
private:
    Order* TMP_ptr_bid_end;
    std::shared_mutex lob_mutex;
public:
    SmartBlockingLimitOrderBook(int precision, int depth): LimitOrderBook(precision, depth){}


    void add_order(const Order& order, bool is_bid) override {
        LimitOrderBook::add_order(order, is_bid);
        //check if pointer has changed
        if (TMP_ptr_bid_end != ptr_bid_end)
        {
            //block and update
            std::unique_lock<std::shared_mutex> lock(lob_mutex);
            TMP_ptr_bid_end = ptr_bid_end;
        }
    }

    Order get_best_bid() override {
        std::shared_lock<std::shared_mutex> lock(lob_mutex);
        return *TMP_ptr_bid_end;
    }
};

}