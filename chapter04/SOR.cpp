
/*
----------------------------------------------------------------------------------
DISCLAIMER:

The following code is primarily for illustrative purposes, aiming to showcase the 
concept of dynamic portfolio optimization using Reinforcement Learning in C++ with TensorFlow. 
It is not intended for actual trading or any real-world financial decision-making. While 
efforts have been made to ensure its accuracy, there's no guarantee that it will compile or 
function flawlessly in all environments.

This code includes:
1. Structures and classes representing market data and financial instruments.
2. A representation of a trading environment where an agent can buy, sell, or hold assets.
3. A basic implementation of a Deep Q-Network (DQN) agent that learns to optimize a portfolio 
   based on Reinforcement Learning.
4. A main routine that ties all the components together, simulating the agent's interactions 
   with the trading environment over multiple episodes.

Financial markets are complex, influenced by countless factors. The model in this code is a 
simplified representation and might not account for all real-world intricacies. It's crucial 
to consult with financial experts before making any investment decisions and to thoroughly 
test any models in safe environments before real-world application.

Use this code at your own risk.
----------------------------------------------------------------------------------
*/

#include <map>
#include <string>
#include <vector>
#include <random>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>

class PortfolioState {
private:
    double cashOnHand;
    int current_asset_count;  // Current number of assets held
    std::map<std::string, double> assetPositions;  // Asset name to quantity mapping
    std::map<std::string, double> pendingOrders;  // Asset name to quantity of pending orders

public:
    // Constructor
    PortfolioState() : cashOnHand(0.0), current_asset_count(0) {}
    PortfolioState(double cash, int asset_count) : cashOnHand(cash), current_asset_count(asset_count) {}

    // Set and get cash on hand
    void setCashOnHand(double cash) {
        cashOnHand = cash;
    }

    double getCashOnHand() const {
        return cashOnHand;
    }
    int getCurrentAssetCount() const {
        return current_asset_count;
    }
    void setCurrentAssetCount(int asset_count) {
        current_asset_count = asset_count; 
    }

    // Methods for asset positions
    void setAssetPosition(const std::string& assetName, double quantity) {
        assetPositions[assetName] = quantity;
    }

    double getAssetPosition(const std::string& assetName) const {
        if (assetPositions.find(assetName) != assetPositions.end()) {
            return assetPositions.at(assetName);
        }
        return 0.0;  // Default to zero if asset not found
    }

    // Methods for pending orders
    void setPendingOrder(const std::string& assetName, double quantity) {
        pendingOrders[assetName] = quantity;
    }

    double getPendingOrder(const std::string& assetName) const {
        if (pendingOrders.find(assetName) != pendingOrders.end()) {
            return pendingOrders.at(assetName);
        }
        return 0.0;  // Default to zero if order not found
    }

    // Clear pending order
    void clearPendingOrder(const std::string& assetName) {
        pendingOrders.erase(assetName);
    }
};

class MarketData {
private:
    // Price Information
    double bidPrice;
    double askPrice;
    double lastTradedPrice;
    double vwap;

    // Order Book Depth (for simplicity, we'll represent just total volume at bid and ask)
    double bidVolume;
    double askVolume;

    // Trade Volume
    double tradeVolume;

public:
    // Constructor
    MarketData() : bidPrice(0.0), askPrice(0.0), 
                lastTradedPrice(0.0), vwap(0.0), bidVolume(0.0), 
                askVolume(0.0), tradeVolume(0.0) {}

    // Setters and Getters for Price Information
    void setBidPrice(double bid) { bidPrice = bid; }
    double getBidPrice() const { return bidPrice; }
    void setAskPrice(double ask) { askPrice = ask; }
    double getAskPrice() const { return askPrice; }
    void setLastTradedPrice(double lastPrice) { lastTradedPrice = lastPrice; }
    double getLastTradedPrice() const { return lastTradedPrice; }
    void setVWAP(double volumeWeightedAvgPrice) { vwap = volumeWeightedAvgPrice; }
    double getVWAP() const { return vwap; }
    // Setters and Getters for Order Book Depth
    void setBidVolume(double volume) { bidVolume = volume;}
    double getBidVolume() const { return bidVolume; }
    void setAskVolume(double volume) { askVolume = volume; }
    double getAskVolume() const { return askVolume; }
    // Setters and Getters for Trade Volume
    void setTradeVolume(double volume) { tradeVolume = volume; }
    double getTradeVolume() const { return tradeVolume; }
};

class VenueMetrics {
private:
    std::string venueName;

    // Metrics related to order execution quality
    double slippageRate;       // Percentage slippage rate
    double rejectionRate;      // Percentage of orders rejected by the venue
    double orderCancelRate;    // Percentage of orders that were cancelled before execution
    double historicalFillRate; // Historical percentage of order fills at this venue

public:
    VenueMetrics(std::string name) : venueName(name), slippageRate(0.0), rejectionRate(0.0), 
                                     orderCancelRate(0.0), historicalFillRate(0.0) {}

    // Setters
    void setSlippageRate(double rate) { slippageRate = rate; }
    void setRejectionRate(double rate) { rejectionRate = rate; }
    void setOrderCancelRate(double rate) { orderCancelRate = rate; }
    void setHistoricalFillRate(double rate) { historicalFillRate = rate; }

    // Getters
    double getSlippageRate() const { return slippageRate; }
    double getRejectionRate() const { return rejectionRate; }
    double getOrderCancelRate() const { return orderCancelRate; }
    double getHistoricalFillRate() const { return historicalFillRate; }
    std::string getVenueName() const { return venueName; }
};

class Order {
public:
    enum class OrderType { BUY, SELL };
    enum class OrderStatus { PENDING, PARTIALLY_FILLED, FILLED, CANCELED };

private:
    std::string assetSymbol;   // Symbol of the asset being traded (e.g., "AAPL" for Apple Inc.)
    OrderType type;            // Buy or Sell order
    double size;               // Amount of asset to buy/sell
    double price;              // Desired execution price
    OrderStatus status;        // Current status of the order

public:
    Order(std::string symbol, OrderType ordType, double ordSize, double ordPrice) 
        : assetSymbol(symbol), type(ordType), size(ordSize), price(ordPrice), status(OrderStatus::PENDING) {}

    // Setters
    void setStatus(OrderStatus stat) { status = stat; }
    void setPrice(double prc) { price = prc; }
    void setSize(double sz) { size = sz; }

    // Getters
    std::string getAssetSymbol() const { return assetSymbol; }
    OrderType getType() const { return type; }
    double getSize() const { return size; }
    double getPrice() const { return price; }
    OrderStatus getStatus() const { return status; }
};

class Environment {
private:
    // Attributes that represent the current state of the market
    PortfolioState current_portfolio_state;
    MarketData current_market_data;
    VenueMetrics venue_metrics;

    // Random number generator for simulation purposes (e.g., market fluctuations)
    std::mt19937 rng;

    // Method to simulate the market reaction to the action
    double simulateMarketReaction(const int action) {
        // This method simulates price changes in response to actions
        // For now, we'll use a random number generator to simulate market fluctuations
        std::uniform_real_distribution<double> price_change(-0.5, 0.5); // price can change by +/- 0.5 units
        return price_change(rng);
    }

public:
    Environment(const PortfolioState& portfolio_state, const MarketData& market_data, const VenueMetrics& metrics) 
        : current_portfolio_state(portfolio_state), current_market_data(market_data), venue_metrics(metrics) {
        // Seed the random number generator
        rng.seed(std::random_device()());
    }

    // Get the current state representation of the environment
    std::vector<double> getStateRepresentation() const {
        std::vector<double> state_representation;

        // Append portfolio state details
        state_representation.push_back(current_portfolio_state.getCashOnHand());
        state_representation.push_back(current_portfolio_state.getCurrentAssetCount());

        // Append market data details
        state_representation.push_back(current_market_data.getLastTradedPrice());

        // Append venue metrics details
        state_representation.push_back(venue_metrics.getSlippageRate());
        state_representation.push_back(venue_metrics.getRejectionRate());
        state_representation.push_back(venue_metrics.getOrderCancelRate());
        state_representation.push_back(venue_metrics.getHistoricalFillRate());

        return state_representation;
    }

    // Simulate the effect of an action taken by the DRL agent
    double step(const int action) {
        double reward = 0.0;

        // Calculate the price change based on the action taken
        double price_change = simulateMarketReaction(action);
        double new_price = current_market_data.getLastTradedPrice() + price_change;

        if (action == 0) { // Buy
            if (current_portfolio_state.getCashOnHand() >= new_price) {
                // Update portfolio to reflect the purchase
                current_portfolio_state.setCashOnHand(current_portfolio_state.getCashOnHand() - new_price);
                current_portfolio_state.setCurrentAssetCount(current_portfolio_state.getCurrentAssetCount() + 1);
                
                // Calculate reward based on the change in portfolio value
                reward = price_change;
            } else {
                // Negative reward if not enough cash to buy
                reward = -1;
            }
        } else if (action == 1) { // Sell
            if (current_portfolio_state.getCurrentAssetCount() > 0) {
                // Update portfolio to reflect the sale
                current_portfolio_state.setCashOnHand(current_portfolio_state.getCashOnHand() + new_price);
                current_portfolio_state.setCurrentAssetCount(current_portfolio_state.getCurrentAssetCount() - 1);

                // Calculate reward based on the change in portfolio value
                reward = price_change;
            } else {
                // Negative reward if no asset to sell
                reward = -1;
            }
        } else if (action == 2) { // Hold
            // Calculate reward based on market dynamics, opportunity costs, etc.
            reward = price_change * current_portfolio_state.getCurrentAssetCount(); // Benefit or loss from 
                                                                                    // holding the asset
        }

        // Update the current market price after the action
        current_market_data.setLastTradedPrice(new_price);

        return reward;
    }

    // Reset the environment to a starting state for a new episode of training
    void reset(const PortfolioState& portfolio_state, const MarketData& market_data) {
        current_portfolio_state = portfolio_state;
        current_market_data = market_data;
    }
};

class DRLAgent {
private:
    tensorflow::Scope scope;                   // TensorFlow computation scope
    tensorflow::Session session;               // TensorFlow session for computation
    tensorflow::Output model;                  // The neural network model (Deep Q-Network)
    double exploration_rate;                   // Current exploration rate for epsilon-greedy strategy
    std::deque<Experience> experience_replay;  // Experience replay memory
    int global_step;                           // Tracking the number of steps
    const int TRAINING_INTERVAL = 100;         // Training frequency
    const int BATCH_SIZE = 64;                 // Size of the training batch
    const int MAX_REPLAY_MEMORY_SIZE = 10000;  // Maximum size of replay memory
    const int TOTAL_POSSIBLE_ACTIONS = 3;      // Define this based on your problem


    tensorflow::Output buildModel(tensorflow::Input state_input) {
        // Define a simple feed-forward neural network here
        auto layer_1 = tensorflow::ops::Relu(scope, tensorflow::ops::MatMul(scope, state_input, /* weights_1 */));
        auto layer_2 = tensorflow::ops::Relu(scope, tensorflow::ops::MatMul(scope, layer_1, /* weights_2 */));
        return tensorflow::ops::MatMul(scope, layer_2, /* weights_output */);
    }
    tensorflow::Tensor tensorFromState(const State& state) {
        // Placeholder tensor for our state data
        tensorflow::Tensor state_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, STATE_SIZE}));

        // Convert portfolio state
        auto tensor_mapped = state_tensor.tensor<float, 2>();
        tensor_mapped(0, 0) = state.portfolio.currentAssetCount;

        // Convert recent market data
        tensor_mapped(0, 1) = state.marketData.price;
        tensor_mapped(0, 2) = state.marketData.bidAskSpread;
        tensor_mapped(0, 3) = state.marketData.VWAP;

        // Convert venue quality metrics
        tensor_mapped(0, 4) = state.venueMetrics.slippageRate;
        tensor_mapped(0, 5) = state.venueMetrics.rejectionRate;
        tensor_mapped(0, 6) = state.venueMetrics.orderCancelRate;
        tensor_mapped(0, 7) = state.venueMetrics.historicalFillRate;


        return state_tensor;
    }
    tensorflow::Tensor computeLoss(const std::vector<tensorflow::Tensor>& targetQs, 
                            const std::vector<tensorflow::Tensor>& predictedQs, 
                            const std::vector<tensorflow::Tensor>& actions, 
                            const std::vector<tensorflow::Tensor>& rewards, 
                            const std::vector<tensorflow::Tensor>& done_flags) {
        // Loss = Summation of (reward + gamma * max(targetQ) - predictedQ)^2
        // where gamma is the discount factor

        const float gamma = 0.99; // discount factor
        tensorflow::Tensor loss(tensorflow::DT_FLOAT, tensorflow::TensorShape({})); // scalar tensor to 
                                                                                    // store loss value
        auto loss_tensor = loss.tensor<float, 0>();

        float accumulated_loss = 0.0;
        for(size_t i = 0; i < targetQs.size(); i++) {
            float max_targetQ = targetQs[i].tensor<float, 2>().maximum(); // assuming Q-values are in 2D tensor
            float predictedQ = predictedQs[i].tensor<float, 2>()(0, actions[i].tensor<int, 1>()(0));
            accumulated_loss += std::pow(
                rewards[i].tensor<float, 1>()(0) + gamma * max_targetQ * (1 - done_flags[i].tensor<float, 1>()(0)) 
                - predictedQ, 2);
        }

        loss_tensor(0) = accumulated_loss / targetQs.size(); // average loss

        return loss;
    }

public:
    // Constructor to initialize DRLAgent with default parameters
    DRLAgent() : scope(tensorflow::Scope::NewRootScope()), 
                 session(tensorflow::CreateSession(scope)),
                 exploration_rate(1.0),
                 global_step(0) {
        // Define the state input placeholder
        auto state_input = tensorflow::ops::Placeholder(scope, tensorflow::DT_FLOAT);
        
        // Build the model
        model = buildModel(state_input);

        // Compile the TensorFlow graph
        TF_CHECK_OK(session->Create(scope.ToGraphDef()));
    }

    // Method to choose action based on current state using epsilon-greedy strategy
    int chooseAction(const PortfolioState& state) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        // Exploration: Choose a random action
        if (dis(gen) < exploration_rate) {
            return std::rand() % TOTAL_POSSIBLE_ACTIONS;
        }

        // Exploitation: Choose the action with the highest Q-value
        tensorflow::Tensor state_tensor = tensorFromState(state);  // Convert state to TensorFlow tensor
        std::vector<tensorflow::Tensor> outputs;

        TF_CHECK_OK(session.Run({{state_input, state_tensor}}, {model}, &outputs));

        // Extract the Q-values from the output tensor
        auto q_values = outputs[0].flat<float>();

        // Find the action with the maximum Q-value
        int best_action = std::distance(q_values.data(), 
                            std::max_element(q_values.data(), q_values.data() + TOTAL_POSSIBLE_ACTIONS));

        return best_action;
    }

    // Method to train the model based on experiences in replay memory
    void train() {
        // Check if it's time to train (based on a predefined interval)
        if (global_step % TRAINING_INTERVAL != 0) {
            return;
        }

        // Sample a mini-batch from the replay memory
        std::vector<Experience> mini_batch; // Initialize an empty vector
        if(experience_replay.size() >= BATCH_SIZE) {
            for(int i=0; i<BATCH_SIZE; i++) {
                mini_batch.push_back(experience_replay[i]);
            }
        }

        // Prepare tensors for states, next_states, rewards, actions, and done_flags
        // Convert the data from the mini_batch into TensorFlow tensors
        std::vector<tensorflow::Tensor> states, next_states, rewards, actions, done_flags;
        for (const auto& exp : mini_batch) {
            states.push_back(tensorFromState(exp.state));
            next_states.push_back(tensorFromState(exp.next_state));
            rewards.push_back(tensorflow::Tensor(exp.reward));
            actions.push_back(tensorflow::Tensor(exp.action));
            done_flags.push_back(tensorflow::Tensor(exp.done));
        }

        // Compute the target Q-values using the Bellman equation
        std::vector<tensorflow::Tensor> target_q_values_outputs;
        TF_CHECK_OK(session.Run({{next_states_input, next_states}}, {target_model}, &target_q_values_outputs));

        // Compute predicted Q-values from the main model
        std::vector<tensorflow::Tensor> predicted_q_values_outputs;
        TF_CHECK_OK(session.Run({{states_input, states}}, {main_model}, &predicted_q_values_outputs));

        // Compute loss
        tensorflow::Tensor loss = 
            computeLoss(target_q_values_outputs, predicted_q_values_outputs, actions, rewards, done_flags);

        // Perform backpropagation using the optimizer
        TF_CHECK_OK(session.Run({{states_input, states}, {actions_input, actions}, 
                                    {rewards_input, rewards}, {next_states_input, next_states}, 
                                    {done_flags_input, done_flags}}, {optimizer, loss}, nullptr));

        global_step++;
    }

    // Method to update the agent's state after an action has been taken
    void updateState(const Environment& env, const Action& action, double reward, const Environment& new_env) {
        // Store the experience in the replay memory
        Experience experience;
        experience.state = env;
        experience.action = action;
        experience.reward = reward;
        experience.new_state = new_env;

        // If replay memory is full, remove the oldest experience
        if (experience_replay.size() == MAX_REPLAY_MEMORY_SIZE) {
            experience_replay.pop_front();
        }

        // Add the new experience to the replay memory
        experience_replay.push_back(experience);
    }    
};


class ExperienceReplay {
private:
    std::deque<Experience> memory;   // Deque to store experiences
    int capacity;                    // Maximum number of experiences to store

public:
    // Constructor to initialize ExperienceReplay with a given capacity
    ExperienceReplay(int capacity) : capacity(capacity) {}

    // Method to add an experience to the replay memory
    void add(const Experience& experience) {
        // If the memory is full, remove the oldest experience
        if (memory.size() == capacity) {
            memory.pop_front();
        }

        // Add the new experience
        memory.push_back(experience);
    }

    // Method to sample a batch of experiences
    std::vector<Experience> sample(int batch_size) {
        if (batch_size > memory.size()) {
            throw std::runtime_error("Batch size is larger than current memory size.");
        }

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, memory.size() - 1);

        std::vector<Experience> batch;
        for (int i = 0; i < batch_size; ++i) {
            int idx = distribution(generator);
            batch.push_back(memory[idx]);
        }

        return batch;
    }

    // Method to get the current size of the memory
    int size() const {
        return memory.size();
    }
};


int main() {
    // Parameters
    int total_episodes = 10000;  // Total episodes for training
    double epsilon = 1.0;  // Exploration rate
    double min_epsilon = 0.01;  // Minimum exploration rate
    double epsilon_decay = 0.995;  // Decay rate for exploration
    int training_interval = 10;  // Train the agent every x episodes

    // Initialize the DRL agent and the simulated trading environment
    DRLAgent agent;
    SimulatedEnvironment env;

    // Main execution loop
    for (int episode = 0; episode < total_episodes; episode++) {
        // Reset the environment for a new episode
        State current_state = env.reset();

        // Initialize episode reward
        double episode_reward = 0.0;

        // Continue until the episode is done
        bool done = false;
        while (!done) {
            Action action;

            // Decide action based on epsilon-greedy strategy
            if (((double) rand() / (RAND_MAX)) < epsilon) {
                action = agent.explore_action();  // Explore
            } else {
                action = agent.exploit_action(current_state);  // Exploit
            }

            // Perform the action and get the new state and reward
            State new_state;
            double reward;
            tie(new_state, reward, done) = env.perform_action(action);

            // Store this experience in the agent's memory
            agent.store_experience(current_state, action, reward, new_state, done);

            // Update the current state and accumulate the reward
            current_state = new_state;
            episode_reward += reward;
        }

        // Train the agent at intervals
        if (episode % training_interval == 0) {
            agent.train();
        }

        // Decay the exploration rate
        if (epsilon > min_epsilon) {
            epsilon *= epsilon_decay;
        }

        // Log episode reward (for illustration purposes)
        std::cout << "Episode " << episode + 1 << ": Reward = " << episode_reward << std::endl;
    }

    return 0;
}
