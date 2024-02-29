 #pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace circular_array
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
    std::vector<Order> bids;
    std::vector<Order> offers;
    int precision;
    int depth;
    double step_value;
    
protected:
    Order* ptr_bid_ini;
    Order* ptr_bid_end;
    Order* ptr_offer_ini;
    Order* ptr_offer_end;

int price_to_index(double price, bool is_bid, bool update_pointers=true)
{
    // the goal of this method is to update the ini/end pointers based on the incoming price
    // (in case the price is not valid or out of range, do nothing)
    if (is_bid)
    {
        double diff = 0;
        if (ptr_bid_ini == nullptr)
        {
            if (update_pointers)
                ptr_bid_ini = ptr_bid_end = &bids[0];
            return 0;
        }
        else if (price >= ptr_bid_ini->price && price <= ptr_bid_end->price)
        {
            // in this case, ini/end pointer stay the same
            int qty_steps = (price - ptr_bid_ini->price) / step_value;
            return (ptr_bid_ini + qty_steps) - &bids[0];
        }
        else if ( std::abs( std::max(ptr_bid_end->price, price) - std::min(ptr_bid_ini->price, price)) * step_value < depth){
            //In this scenario, we have available array items
            // Here we need to update ini/end pointer to their new postion.
            // Due to this condition, the end pointer will be at the new price
            // We must count if advancing the pointers, how many times we cycle. If we cycle more than once, all existing element values should be invalidated.
            int qty_steps = round((price - ptr_bid_end->price) * step_value);
            int cycles = abs(qty_steps) / depth;
            if (price > ptr_bid_end->price && cycles >= 1) //means that all the existing values are invalidated. It is like having a buffer from scratch
            {
                if (update_pointers)
                    ptr_bid_ini = ptr_bid_end = &bids[0];
                return 0;
            }
            else{
                int current_ini_position = ptr_bid_ini - &bids[0];
                int current_end_position = ptr_bid_end - &bids[0];
                int new_ini_position = (( (qty_steps>=0 ? current_ini_position: current_end_position) + qty_steps) % depth + depth) % depth;
                int new_end_position = (( (qty_steps>=0 ? current_end_position: current_ini_position) + qty_steps) % depth + depth) % depth;
                
                
                if (price < ptr_bid_ini->price)
                {
                    if (update_pointers)
                        ptr_bid_ini = &bids[0] + new_ini_position; // update to the new ini ptr
                    return new_ini_position;
                }
                if (price > ptr_bid_end->price)
                {
                    if (update_pointers)
                        ptr_bid_end = &bids[0] + new_end_position; // update end ptr
                    return new_end_position;
                }
            }
        }
        else if (price < ptr_bid_ini->price) //all array's item are used, hence discard
            return -1; //discard the incoming price
        else if ( std::abs( std::max(ptr_bid_end->price, price) - std::min(ptr_bid_ini->price, price)) * step_value > depth)
        {
            //In this scenario, we want to shift the circular buffer
            //here we need to update ini/end pointer to their new postion.
            // Due to this condition, the end pointer will be at the new price
            // We must count if advancing the pointers, how many times we cycle. If we cycle more than once, all existing element values should be invalidated.
            int qty_steps = round((price - ptr_bid_end->price) * step_value);
            int cycles = abs(qty_steps) / depth;
            if (price > ptr_bid_end->price && cycles >= 1) //means that all the existing values are invalidated. It is like having a buffer from scratch
            {
                if (update_pointers)
                    ptr_bid_ini = ptr_bid_end = &bids[0];
                return 0;
            }
            else{
                int current_ini_position = ptr_bid_ini - &bids[0];
                int current_end_position = ptr_bid_end - &bids[0];
                int new_ini_position = (( (qty_steps>=0 ? current_ini_position: current_end_position) + qty_steps) % depth + depth) % depth;
                int new_end_position = (( (qty_steps>=0 ? current_end_position: current_ini_position) + qty_steps) % depth + depth) % depth;
                if (qty_steps>1)
                {
                    //since we are skipping, clean/reset intermediate elements 
                    //this scenario happens when there is a gap up/down in the market
                    for(int i=0; i<new_end_position; i++)
                        bids[i].reset();
                }
                if (update_pointers)
                {
                    ptr_bid_ini = &bids[0] + new_ini_position; // update to the new ini ptr
                    ptr_bid_end = &bids[0] + new_end_position; // update end ptr
                }
                if (price < ptr_bid_ini->price)                   
                    return new_ini_position;
                if (price > ptr_bid_end->price)
                    return new_end_position;
            }            
        }        
    }
        else  /*HANDLE OFFER*/
        {
            double diff = 0;
            if (ptr_offer_ini == nullptr)
            {
                if (update_pointers)
                    ptr_offer_ini = ptr_offer_end = &offers[0];
                return 0;
            }
            else if (price >= ptr_offer_ini->price && price <= ptr_offer_end->price)
            {
                // In this scenario, we are adding in the middle of the buffer's range.
                // in this case, ini/end pointer stay the same
                int qty_steps = (price - ptr_offer_ini->price) / step_value;
                return (ptr_offer_ini + qty_steps) - &offers[0];
            }
            else if ( std::abs( std::max(ptr_offer_end->price, price) - std::min(ptr_offer_ini->price, price)) * step_value < depth){
                //In this scenario, we have available array items
                //here we need to update ini/end pointer to their new postion.
                // Due to this condition, the end pointer will be at the new price
                // We must count if advancing the pointers, how many times we cycle. If we cycle more than once, all existing element values should be invalidated.
                int qty_steps = round((price - ptr_offer_end->price) * step_value);
                int cycles = abs(qty_steps) / depth;
                if (price > ptr_offer_end->price && cycles >= 1) //means that all the existing values are invalidated. It is like having a buffer from scratch
                {
                    if (update_pointers)
                        ptr_offer_ini = ptr_offer_end = &offers[0];
                    return 0;
                }
                else{
                    int current_ini_position = ptr_offer_ini - &offers[0];
                    int current_end_position = ptr_offer_end - &offers[0];
                    int new_ini_position = (( (qty_steps>=0 ? current_ini_position: current_end_position) + qty_steps) % depth + depth) % depth;
                    int new_end_position = (( (qty_steps>=0 ? current_end_position: current_ini_position) + qty_steps) % depth + depth) % depth;
                    
                    
                    if (price < ptr_offer_ini->price)
                    {
                        if (update_pointers)
                            ptr_offer_ini = &offers[0] + new_ini_position; // update to the new ini ptr
                        return new_ini_position;
                    }
                    if (price > ptr_offer_end->price)
                    {
                        if (update_pointers)
                            ptr_offer_end = &offers[0] + new_end_position; // update end ptr
                        return new_end_position;
                    }
                }
            }
            else if (price > ptr_offer_end->price) //all array's item are used, hence discard
                return -1; //discard the incoming price
            else if ( std::abs( std::max(ptr_offer_end->price, price) - std::min(ptr_offer_ini->price, price)) * step_value > depth)
            {
                //In this scenario, we want to shift the circular buffer
                //here we need to update ini/end pointer to their new postion.
                // Due to this condition, the end pointer will be at the new price
                // We must count if advancing the pointers, how many times we cycle. If we cycle more than once, all existing element values should be invalidated.

                int qty_steps_ini = static_cast<int>(std::round((price - ptr_offer_ini->price) * step_value));
                int qty_steps_end = static_cast<int>(std::round((ptr_offer_end->price - price) * step_value));

                int cycles = abs(qty_steps_ini) / depth;
                if (price < ptr_offer_ini->price && cycles >= 1) //means that all the existing values are invalidated. It is like having a buffer from scratch
                {
                    ptr_offer_ini = ptr_offer_end = &offers[0];
                    return 0;
                }
                else{
                    int current_ini_position = ptr_offer_ini - &offers[0];
                    int current_end_position = ptr_offer_end - &offers[0];                    
                    int new_ini_position = (( current_ini_position + qty_steps_ini ) % depth + depth) % depth;
                    int new_end_position = (( current_end_position + qty_steps_end-1 ) % depth + depth) % depth;

                    if (qty_steps_ini>1)
                    {
                        //since we are skipping, clean/reset intermediate elements 
                        //this scenario happens when there is a gap up/down in the market
                        for(int i=0; i<new_end_position; i++)
                            offers[i].reset();
                    }
                    if (update_pointers)
                    {
                        ptr_offer_ini = &offers[0] + new_ini_position; // update to the new ini ptr
                        ptr_offer_end = &offers[0] + new_end_position; // update end ptr
                    }
                    if (price < ptr_offer_ini->price)                   
                        return new_ini_position;
                    if (price > ptr_offer_end->price)
                        return new_end_position;

                }            
            }
        }
    return -1;
}




public:
    LimitOrderBook(int precision, int depth) : precision(precision), depth(depth) {
        bids.resize(depth);
        offers.resize(depth);
        ptr_bid_ini = ptr_offer_ini = nullptr;
        ptr_bid_end = ptr_offer_end = nullptr;
        step_value = std::pow(10, precision);
    }

    virtual void add_order(const Order& order, bool is_bid) {        
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


    void update_order(const Order& order, bool is_bid) {
        int index = price_to_index(order.price, is_bid);
        if (index == -1)
            return; 
        if (is_bid) {
            bids[index] = order;
        } else {
            offers[index] = order;
        }
    }

    void delete_order(const Order& order, bool is_bid) {
        int index = price_to_index(order.price, is_bid);
        if (index == -1)
            return; 
        if (is_bid) {
            bids[index] = Order();
        } else {
            offers[index] = Order();
        }
    }

    virtual Order get_best_bid() {
        return *ptr_bid_end;
    }
    Order get_lowest_bid() {
        return *ptr_bid_ini;
    }


    Order get_best_offer() {
        return *ptr_offer_ini;
    }
    Order get_highest_offer() {
        return *ptr_offer_end;
    }

    void print_bids()
    {
        for (int i=0; i<bids.size(); i++)
        {
            std::cout << i << "_" << bids[i].price << " * ";
        }
        std::cout << std::endl;
        std::cout << "Bid ini/end=" << ptr_bid_ini->price << "/" << ptr_bid_end->price << std::endl;
    }
    void print_offers()
    {
        for (int i=0; i<offers.size(); i++)
        {
            std::cout << i << "_" << offers[i].price << " * ";
        }
        std::cout << std::endl;
        std::cout << "Offer ini/end=" << ptr_offer_ini->price << "/" << ptr_offer_end->price << std::endl;
    }

};
} // namespace circular_array
