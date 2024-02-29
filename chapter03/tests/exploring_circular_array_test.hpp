#include <cassert>
#include <iomanip>
#include <iostream>
//#include "../exploring_circular_array.hpp"
//#include "../exploring_hash_table.hpp"
//#include "../exploring_linked_list.hpp"
#include "../lockfree_limitorderbook.hpp"

using namespace lockfree;

    void test_lower_order_arrives(bool is_bid)
    {
        //Full vector, and a lower order arrives => discard
        LimitOrderBook lob(2, 10);

        lob.add_order(Order(1, 29500.24, 100), is_bid);
        lob.add_order(Order(2, 29500.23, 200), is_bid);    
        lob.add_order(Order(4, 29500.22, 400), is_bid);
        lob.add_order(Order(5, 29500.21, 500), is_bid);
        lob.add_order(Order(3, 29500.14, 300), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 5);
            assert(lob.get_best_bid().id == 1);
        }
        else{
            lob.print_offers();
            assert(lob.get_highest_offer().id == 2);
            assert(lob.get_best_offer().id == 3);
        }
        std::cout << "######TEST CASE 1 PASSED" << std::endl<< std::endl;
    }
    void test_lower_order_arrives2(bool is_bid)
    {
        //Full vector, and a lower order arrives => discard
        LimitOrderBook lob(2, 4);

        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.24, 500), is_bid);
        lob.add_order(Order(3, 29500.20, 300), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 1);
            assert(lob.get_best_bid().id == 5);
        }
        else
        {
            lob.print_offers();
            assert(lob.get_highest_offer().id == 4);
            assert(lob.get_best_offer().id == 3);
        }
        std::cout << "######TEST CASE 2 PASSED" << std::endl<< std::endl;
    }

    void test_higher_order_arrives(bool is_bid)
    {
        //Full vector, and a higher order arrives => discard the lowest (pivot the buffer)
        LimitOrderBook lob(2, 4);

        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.24, 500), is_bid);
        lob.add_order(Order(3, 29500.25, 300), is_bid);
        if (is_bid)
        {
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 2);
            assert(lob.get_best_bid().id == 3);
        }
        else{
            lob.print_offers();
            assert(lob.get_highest_offer().id == 5);
            assert(lob.get_best_offer().id == 1);
        }
        std::cout << "######TEST CASE 3 PASSED" << std::endl<< std::endl;
    }
    void test_order_arrives_with_gapup(bool is_bid)
    {
        //Full vector, and a higher order arrives with gap up
        LimitOrderBook lob(2, 4);
        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.24, 500), is_bid);
        lob.add_order(Order(3, 29500.27, 300), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 5);
            assert(lob.get_best_bid().id == 3);
        }
        else {
            lob.print_offers();
            assert(lob.get_highest_offer().id == 5);
            assert(lob.get_best_offer().id == 1);
        }
        std::cout << "######TEST CASE 4 PASSED" << std::endl<< std::endl;

    }
    void test_order_arrives_with_gapdown(bool is_bid)
    {
        //Full vector, and a higher order arrives with gap down => must be discarded (if bid)
        LimitOrderBook lob(2, 4);
        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.24, 500), is_bid);
        lob.add_order(Order(3, 29500.18, 300), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 1);
            assert(lob.get_best_bid().id == 5);
        }
        else{
            lob.print_offers();
            assert(lob.get_highest_offer().id == 1);
            assert(lob.get_best_offer().id == 3);
        }
        std::cout << "######TEST CASE 5 PASSED" << std::endl<< std::endl;
    }
    void test_order_arrives_with_gapup_cycle(bool is_bid)
    {
        //Full vector, and a higher order arrives with gap up, having at least 1 cycle
        LimitOrderBook lob(2, 4);
        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.28, 500), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 5);
            assert(lob.get_best_bid().id == 5);
        }
         else{
            lob.print_offers();
            assert(lob.get_highest_offer().id == 4);
            assert(lob.get_best_offer().id == 1);
        }
        std::cout << "######TEST CASE 6 PASSED" << std::endl<< std::endl;
    }
    void test_order_arrives_with_gapdown_cycle(bool is_bid)
    {
        //Full vector, and a higher order arrives with gap up, having at least 1 cycle
        LimitOrderBook lob(2, 4);
        lob.add_order(Order(1, 29500.21, 100), is_bid);
        lob.add_order(Order(2, 29500.22, 200), is_bid);    
        lob.add_order(Order(4, 29500.23, 400), is_bid);
        lob.add_order(Order(5, 29500.15, 500), is_bid);
        if (is_bid){
            lob.print_bids();
            assert(lob.get_lowest_bid().id == 1);
            assert(lob.get_best_bid().id == 4);
        }
         else{
            lob.print_offers();
            assert(lob.get_highest_offer().id == 5);
            assert(lob.get_best_offer().id == 5);
        }
        std::cout << "######TEST CASE 7 PASSED" << std::endl<< std::endl;
    }




    void run_all_tests()
    {
        std::cout << std::fixed;
        std::cout << std::setprecision(2);

        bool is_bid = true;
        test_lower_order_arrives(is_bid);
        test_lower_order_arrives2(is_bid);
        test_higher_order_arrives(is_bid);
        test_order_arrives_with_gapup(is_bid);
        test_order_arrives_with_gapdown(is_bid);
        test_order_arrives_with_gapup_cycle(is_bid);
        test_order_arrives_with_gapdown_cycle(is_bid);

        is_bid = false;
        test_lower_order_arrives(is_bid);
        test_lower_order_arrives2(is_bid);
        test_higher_order_arrives(is_bid);
        test_order_arrives_with_gapup(is_bid);
        test_order_arrives_with_gapdown(is_bid);
        test_order_arrives_with_gapup_cycle(is_bid);
        test_order_arrives_with_gapdown_cycle(is_bid);

    }


