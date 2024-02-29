#include <zmq.hpp>
#include <string>
#include <iostream>
#include "limit_order_book.hpp"

// Include the necessary headers for CPU pinning
#ifdef __linux__
#include <sched.h>
#endif

using namespace std;

class messaging_hub {
private:
    zmq::context_t context;
    zmq::socket_t publisher;
    LimitOrderBook& lob;

public:
    MessagingHub(LimitOrderBook& lob) : context(1), publisher(context, ZMQ_PUB), lob(lob) {
        publisher.bind("tcp://*:5556");
    }

    void run() {
        // Pin the thread to a specific CPU core for better performance
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);  // CPU 0
        sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

        while (true) {
            if (lob.has_changed()) {
                string update = lob.get_update();
                zmq::message_t message(update.size());
                memcpy(message.data(), update.data(), update.size());
                publisher.send(message);
            }
        }
    }
};
