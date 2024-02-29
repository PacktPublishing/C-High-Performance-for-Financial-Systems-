#pragma once
#include <unordered_map>
#include <list>

namespace hash_table
{
class Order {
public:
    int id;
    double price;
    int quantity;

    Order() : id(0), price(0), quantity(0) {}
    Order(int id, double price, int quantity) : id(id), price(price), quantity(quantity) {}
    void reset()
    {
        id=0;
        price=0.0;
        quantity=0.0;
    }
};

class LimitOrderBook {
private:
    std::unordered_map<double, Order> book_bids;
    std::unordered_map<double, Order> book_offers;
    int _depth;
    Order* lowestBid;
    Order* highestBid;
    Order* lowestOffer;
    Order* highestOffer; 
public:
    LimitOrderBook() {}
    LimitOrderBook(int precision, int depth): _depth(depth) {}

    void add_order(const Order& order, bool is_bid) {
        //add a new level in the order book

        if (is_bid) {
            // to do bids: check pointers to decide if the current order can be added.
            //     - if adding an order with a lower price than the lowest existing price in book_bids
            //             Then, it must be discarded
            //      - If adding an order higher than the highest existing price
            //             Then the lowest existing item must be removed. 

            // Add to bids            
            book_bids[order.price] = order;
            
            // Update pointers
            if (!lowestBid || order.price < lowestBid->price) lowestBid = &book_bids[order.price];
            if (!highestBid || order.price > highestBid->price) highestBid = &book_bids[order.price];
        } else {
            // to do offers: check pointers to decide if the current order can be added.
            //     - if adding an order with a lower price than the lowest existing price in book_offers
            //             Then the highest existing item must be removed.
            //      - If adding an order higher than the highest existing price
            //             Then, it must be discarded 

            // Add to asks
            book_offers[order.price] = order;
            
            // Update pointers
            if (!lowestOffer || order.price < lowestOffer->price) lowestOffer = &book_offers[order.price];
            if (!highestOffer|| order.price > highestOffer->price) highestOffer = &book_offers[order.price];
        }
    }

    void delete_order(const Order& order, bool is_bid) {
        if (is_bid)
            book_bids.erase(order.price);
        else
            book_offers.erase(order.price);

        //After removing the order, we would need to check if that affected
        // any of the pointers.
        // If it did, we need to update them, and that will require to traverse through the map.
        // Not the ideal situation.
    }
    void update_order(const Order& order, bool is_bid){
        delete_order(order, is_bid);
        add_order(order, is_bid);
    }


    Order get_best_bid() {
        return *highestBid;
    }
    Order get_lowest_bid() {
        return *lowestBid;
    }


    Order get_best_offer() {
        return *lowestOffer;
    }
    Order get_highest_offer() {
        return *highestOffer;
    }

    void print_bids()
    {
    }
    void print_offers()
    {
    }

};
}