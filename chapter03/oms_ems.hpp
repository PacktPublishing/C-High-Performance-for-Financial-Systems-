#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <zmq.hpp>

using namespace std;
class FIXEngine
{
    public:
    void SendOrder (const Order& order){}
};

// Order class
struct Order {
    int id;
    string status;
    // Add other order details
};

// High performance queue
template <typename T>
class UpdatesQueue {
private:
    queue<T> q;
    mutex mtx;
public:
    void push(T order) {
        lock_guard<mutex> lock(mtx);
        q.push(order);
    }
    T pop() {
        lock_guard<mutex> lock(mtx);
        T val = q.front();
        q.pop();
        return val;
    }
    bool empty() {
        lock_guard<mutex> lock(mtx);
        return q.empty();
    }
};

// OMS class
class OMS {
private:
    unordered_map<int, Order> activeOrders;
    unordered_map<int, Order> filledOrders;
    UpdatesQueue<Order> orderUpdates;
    zmq::context_t context;
    zmq::socket_t subscriber;
    std::thread marketDataThread;
    
    bool order_validation_ok(const Order&o){
        //do validations
        
        return true;
    }
public:
    OMS() : context(1), subscriber(context, ZMQ_SUB) {
        subscriber.connect("tcp://localhost:5556");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
    }
    OMS() : context(1), subscriber(context, ZMQ_SUB) {
        subscriber.connect("tcp://localhost:5556");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        //launch new thread to read incoming market data messages from the Messaging Hub
        marketDataThread = std::thread(&OMS::ReceiveMarketData, this);
    }

    ~OMS() {
        if (marketDataThread.joinable()) {
            marketDataThread.join();
        }
    }    
    void ReceiveMarketData() {
        while (true) {
            zmq::message_t update;
            subscriber.recv(&update);
            std::string update_str(static_cast<char*>(update.data()), update.size());
            // Process the update
            // ...
        }
    }

    void run(){
        // Pin this thread to the first CPU core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        while (true){
            ProcessOrderUpdates()
        }
    }

    bool SendOrder(Order order) {
        // Validate order
        if (!order_validation(order))
            return false;
        
        // If valid, add to active orders and send to EMS
        activeOrders.push_back(order);
        EMS::SendOrder(order);

        return true;
    }
    void ExecutionReport(const Order& order, const string& status) {
        // Add order update to queue
        Order updated_order = order;
        updated_order.status = status;
        order_updates.push(updated_order);
    }
    void ProcessOrderUpdates() {
        // Update order status
        // If filled, move to filled orders
        // If cancelled, remove from active orders
        // Otherwise, leave in active orders
        while (!order_updates.empty()) {
            Order order = order_updates.front();
            order_updates.pop();
            // Update order status
            auto it = active_orders.find(order.id);
            if (it != active_orders.end()) {
                it->second.status = order.status;
                if (order.status == "filled") {
                    // Move to filled orders
                    filled_orders[it->first] = it->second;
                    active_orders.erase(it);
                } else if (order.status == "cancelled") {
                    // Remove from active orders
                    active_orders.erase(it);
                }
            }
        }
    }

    void PersistToDB() {
        // Persist filled orders to DB


        //this function should be called by another thread, 
        // at defined intervals, to unload filled arrays, 
        // and have them persist in DB
    }

};

// EMS class
class EMS {
private:
    UpdatesQueue<Order> orderQueue;
    FIXEngine smart_desicion(const Order& order)
    {
        //based on the order and any other logic, 
        //decide where to send the order.
        return FIXEngine();
    }

public:
    void SendOrder(const Order& order) {
        // Keep it on queue
        orderQueue.push(order);
    }
    void ProcessOrderQueue() {
        while (!orderQueue.empty()) {
            Order order = orderQueue.pop();
            
            // Send order to venue
            FIXEngine venue = smart_decision(order);
            venue.sendOrder(order);
        }
    }
};