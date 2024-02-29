#pragma once
#include "exploring_circular_array.hpp"
#include <tbb/concurrent_vector.h>
namespace lockfree
{
    using namespace circular_array;
    class LockFreeLimitOrderBook : public LimitOrderBook {
    private:
        tbb::concurrent_vector<Order> bids;
        tbb::concurrent_vector<Order> offers;

    public:
        LockFreeLimitOrderBook(int precision, int depth) : LimitOrderBook(precision, depth) {
            bids.resize(depth);
            offers.resize(depth);
        }

        void add_order(const Order& order, bool is_bid) override {
            if (is_bid) {
                int index = price_to_index(order.price, true);
                if (index > -1)
                    bids[index] = order;
            } else {
                int index = price_to_index(order.price, false);
                if (index > -1)
                    offers[index] = order;
            }
        }

        Order get_best_bid() override {
            return *ptr_bid_end;
        }

        Order get_best_offer() {
            return *ptr_offer_ini;
        }
    };

} // namespace lockfree



