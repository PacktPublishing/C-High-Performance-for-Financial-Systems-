#pragma once
#include <iostream>

namespace linked_list {
class Order {
public:
    int id;
    double price;
    Order* next;

    Order(int id, double price, int quantity) : price(price), id(id)
    {
        next = NULL;
    }
    Order (const Order& o)
    {
        id = o.id;
        price = o.price;
        next = NULL;
    }
    Order(double p) {
        price = p;
        next = NULL;
    }
};


class LimitOrderBook {
public:
    Order* head;
    Order* tail;
    int depth;
    int size;

    LimitOrderBook(int presicion, int depth) : head(nullptr), tail(nullptr), depth(depth), size(0) {}
    LimitOrderBook(int depth) : head(nullptr), tail(nullptr), depth(depth), size(0) {}
  
    void add_order(const Order& order, bool is_bid) {
        Order* newNode = new Order(order);
        if (size == depth) {
            if (order.price <= head->price) {
                // discard
                delete newNode;
                return;
            } else {
                // remove the head (lowest value)
                Order* temp = head;
                head = head->next;
                delete temp;
                size--;
            }
        }

        if (head == NULL || head->price >= order.price) {
            newNode->next = head;
            head = newNode;
            if (tail == NULL) {
                tail = head;
            }
        } else {
            Order* current = head;
            while (current->next != NULL && current->next->price < order.price) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
            if (newNode->next == NULL) {
                tail = newNode;
            }
        }
        size++;
    }
  
    void delete_order(const Order& order, bool is_bid) {
        if (head == NULL) return;
        if (head->price == order.price) {
            Order* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }
        Order* current = head;
        while (current->next != NULL && current->next->price != order.price) {
            current = current->next;
        }
        if (current->next != NULL) {
            Order* temp = current->next;
            current->next = current->next->next;
            delete temp;
            size--;
        }
    }

    Order get_lowest_bid(){
        if (head == NULL) {
            throw "Order book is empty";
        }
        return *head;
    }
    Order get_best_bid() {
        if (head == NULL) {
            throw "Order book is empty";
        }
        return *tail;
    }    
    void print_bids() {
        Order* order = head;
        while (order != nullptr) {
            std::cout << order->price << " ";
            order = order->next;
        }
        std::cout << std::endl;
    }



    Order get_highest_offer(){ return Order(0, 0, 0);}
    Order get_best_offer(){ return Order(0, 0, 0);} 
    void print_offers(){}


};

}