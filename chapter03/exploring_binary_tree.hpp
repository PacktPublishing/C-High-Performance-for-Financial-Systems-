#pragma once

#include <set>
namespace binary_tree{
class Order {
public:
    int id;
    double price;
    int quantity;

    Order(int id, double price, int quantity) : id(id), price(price), quantity(quantity) {}

    // This operator is used by std::set to order the elements
    bool operator<(const Order& other) const {
        return price < other.price;
    }
};

class LimitOrderBook {
private:
    std::set<Order> orders;
    int depth;

public:
    LimitOrderBook(int presicion, int depth) : depth(depth) {}

    void add_order(const Order& order, bool is_bid) {
        if (orders.size() < depth) {
            orders.insert(order);
        } else if (order.price > (*orders.rbegin()).price) {
            orders.erase(--orders.end());
            orders.insert(order);
        }
    }

    void delete_order(const Order& order, bool is_bid) {
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if (it->id == order.id) {
                orders.erase(it);
                break;
            }
        }
    }

    double get_best_bid() {
        if (orders.empty()) {
            throw "Order book is empty";
        }
        return (*orders.rbegin()).price;
    }
    void print_bids() {
        for (auto it = orders.rbegin(); it != orders.rend(); ++it) {
            std::cout << it->price << " ";
        }
        std::cout << std::endl;
    }
    
    Order get_highest_offer(){return Order(0,0,0);};
    Order get_best_offer(){return Order(0,0,0);};    
    void print_offers(){}
    void do_test()
    {
        LimitOrderBook lob(2, 5);

        // Test add_order
        lob.add_order(binary_tree::Order(1, 10.01, 100), true);
        lob.add_order(binary_tree::Order(2, 10.02, 100), true);
        lob.add_order(binary_tree::Order(6, 10.05, 100), true);
        lob.add_order(binary_tree::Order(3, 10.03, 100), true);
        lob.add_order(binary_tree::Order(4, 10.04, 100), true);
        lob.add_order(binary_tree::Order(5, 10.00, 100), true);
        // Test print_bids
        lob.print_bids(); 
        
        // Try to add an order with a price lower than the lowest in the book
        lob.add_order(binary_tree::Order(7, 9.98, 100), true);

        // The best bid should still be 10.05
        std::cout << "Best bid: " << lob.get_best_bid() << std::endl;

        // Now add an order with a price higher than the highest in the book
        lob.add_order(binary_tree::Order(8, 10.06, 100), true);

        // The best bid should now be 10.06
        std::cout << "Best bid: " << lob.get_best_bid() << std::endl;

        // Delete an order
        lob.delete_order(binary_tree::Order(3, 10.03, 100), true);

        // The best bid should still be 10.06
        std::cout << "Best bid: " << lob.get_best_bid() << std::endl;


        // Test print_bids
        lob.print_bids(); 

    }
};

}