#include <iostream>
#include <queue>
#include <list>

namespace queue{
using namespace std;

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
class Compare {
public:
    bool operator() (Order& o1, Order& o2) {
        return o1.price < o2.price;
    }
};

class LimitOrderBook {
private:
    priority_queue<Order, vector<Order>, Compare> orders;
    int depth;

public:
    LimitOrderBook(int presicion, int d) : depth(d) {}

    void add_order(const Order& order, bool is_bid) {
        if (orders.size() < depth) {
            orders.push(order);
        } else if (order.price > orders.top().price) {
            orders.pop();
            orders.push(order);
        }
    }

    void delete_order(const Order& order, bool is_bid) {
        priority_queue<Order, vector<Order>, Compare> temp;
        while (!orders.empty()) {
            if (orders.top().price != order.price) {
                temp.push(orders.top());
            }
            orders.pop();
        }
        orders = temp;
    }

    int get_best_bid() {
        if (orders.empty()) {
            throw "Order book is empty";
        }
        return orders.top().price;
    }

    void print_bids() {
        priority_queue<Order, vector<Order>, Compare> temp = orders;
        while (!temp.empty()) {
            std::cout << temp.top().price << " ";
            temp.pop();
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
        lob.add_order(Order(1, 100.0, 10), true);
        lob.add_order(Order(2, 101.0, 10), true);
        lob.add_order(Order(3, 99.0, 10), true);
        lob.add_order(Order(4, 102.0, 10), true);
        lob.add_order(Order(5, 98.0, 10), true);
        lob.add_order(Order(6, 103.0, 10), true);  // This order should replace the order with price 98.0
        // Test print_bids
        lob.print_bids(); 
        
        // Test get_best_bid
        cout << "Best bid: " << lob.get_best_bid() << endl;  // Should print 103.0

        // Test delete_order
        lob.delete_order(Order(1, 100.0, 10), true);
        cout << "Best bid after deleting order with price 100.0: " << lob.get_best_bid() << endl;  // Should print 103.0

        // Test print_bids
        lob.print_bids(); 

    }
};

}