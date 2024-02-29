#include "quickfix/Application.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"
#include "quickfix/fix44/Message.h"
#include "quickfix/fix44/MessageCracker.h"

#include "exploring_circular_array.hpp"
#include "lockfree_limitorderbook.hpp"

using namespace circular_array;
using namespace lockfree;

class MyFIXApplication : public FIX::Application, public FIX::MessageCracker
{
public:
    MyFIXApplication(circular_array::LimitOrderBook& lob) : orderBook(lob) {}

    void onCreate(const FIX::SessionID&) override {}
    void onLogon(const FIX::SessionID& sessionID) override {}
    void onLogout(const FIX::SessionID& sessionID) override {}
    void toAdmin(FIX::Message&, const FIX::SessionID&) override {}
    void toApp(FIX::Message&, const FIX::SessionID&) override {}

    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& session) override
    {
        crack(message, session);
    }

    void onMessage(const FIX44::MarketDataIncrementalRefresh& message, const FIX::SessionID&) override {
        // Loop over all the groups (i.e., all the updates in this message)
        int numUpdates = message.groupCount(FIX::FIELD::NoMDEntries);
        for (int i = 1; i <= numUpdates; ++i) {
            FIX44::MarketDataIncrementalRefresh::NoMDEntries group;
            message.getGroup(i, group);

            Order order;

            // Extract the order ID
            FIX::MDEntryID mdEntryID;
            if (group.isSet(mdEntryID)) {
                group.get(mdEntryID);
                order.id = std::stoi(mdEntryID.getValue());
            }

            // Extract the price
            FIX::MDEntryPx mdEntryPx;
            if (group.isSet(mdEntryPx)) {
                group.get(mdEntryPx);
                order.price = mdEntryPx.getValue();
            }

            // Extract the quantity
            FIX::MDEntrySize mdEntrySize;
            if (group.isSet(mdEntrySize)) {
                group.get(mdEntrySize);
                order.quantity = static_cast<int>(mdEntrySize.getValue());
            }

            // Determine whether it's a bid or an offer based on the MDEntryType field
            FIX::MDEntryType mdEntryType;
            group.get(mdEntryType);
            bool is_bid = (mdEntryType.getValue() == FIX::MDEntryType_BID);

            // Determine the update action
            FIX::MDUpdateAction mdUpdateAction;
            group.get(mdUpdateAction);
            switch (mdUpdateAction.getValue()) {
                case FIX::MDUpdateAction_NEW:
                    orderBook.add_order(order, is_bid);
                    break;
                case FIX::MDUpdateAction_CHANGE:
                    orderBook.update_order(order, is_bid);
                    break;
                case FIX::MDUpdateAction_DELETE:
                    orderBook.delete_order(order, is_bid);
                    break;
            }
        }
    }
private:
    circular_array::LimitOrderBook& orderBook;
};
