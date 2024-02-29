
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


#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/ops/standard_ops.h>
#include <tensorflow/core/framework/tensor.h>
#include <algorithm>
#include <random>


// Define a single piece of market data.
struct DataPoint {
    double open;         // Opening price
    double close;        // Closing price
    double high;         // Highest price in a given period
    double low;          // Lowest price in a given period
    double volume;       // Trading volume
    double adjustedClose; // Adjusted closing price (for dividends, stock splits, etc.)    
};

// MarketData class definition
class MarketData {
private:
    std::map<std::string, std::vector<DataPoint>> data; // Key: Ticker symbol, Value: Data points
    size_t currentPosition; // Current position in the data vector

public:
    // Constructor that initializes the market data from a given source (e.g., a file or a database)
    MarketData(const std::map<std::string, std::vector<DataPoint>>& inputData)
        : data(inputData), currentPosition(0) {}

    // Assuming all tickers have the same length of data for simplicity
    void next() {
        if (currentPosition < data.begin()->second.size() - 1) {
            ++currentPosition;
            for (auto &[ticker, dataPoints] : data) {
                // Assuming all tickers have the same data length for now
                // This will move all ticker data positions forward
                ++currentPosition;
            }
        }
    }

    // Get the current data point for a specific ticker
    DataPoint getCurrentData(const std::string& ticker) const {
        return data.at(ticker)[currentPosition];
    }

    // Check if there's more data available
    bool hasMoreData() const {
        return currentPosition < data.begin()->second.size() - 1;
    }

    // Reset to start position (e.g., for a new episode in reinforcement learning)
    void reset() {
        currentPosition = 0;
    }
    
    // For demonstration purposes, return the last 'days' data points for the ticker
    std::vector<DataPoint> getHistoricalData(const std::string& ticker, size_t days) const {
        if (data.at(ticker).size() < days) {
            return data.at(ticker);
        } else {
            return std::vector<DataPoint>(data.at(ticker).end() - days, data.at(ticker).end());
        }
    }

};


class Portfolio { // Data structure to hold the assets and their quantities.
private:    
    std::map<std::string, double> assets; // Key: Ticker symbol, Value: Quantity    
    double cashBalance; // Cash balance in the portfolio.
public:
    // Constructor to initialize the portfolio with a starting cash balance.
    Portfolio(double initialCash) : cashBalance(initialCash) {}
    // Method to add an asset to the portfolio.
    void addAsset(const std::string& ticker, double quantity) {
        assets[ticker] += quantity;
    }
    // Method to remove an asset from the portfolio.
    void removeAsset(const std::string& ticker, double quantity) {
        if (assets[ticker] < quantity) {
            throw std::runtime_error("Not enough assets to remove.");
        }
        assets[ticker] -= quantity;
    }

    // Method to get the quantity of a specific asset.
    double getAssetQuantity(const std::string& ticker) const {
        if (assets.find(ticker) != assets.end()) {
            return assets.at(ticker);
        }
        return 0.0;
    }
    // Method to adjust the cash balance when buying/selling assets.
    void adjustCashBalance(double amount) {
        cashBalance += amount;
    }
    // Method to get the current cash balance.
    double getCashBalance() const {
        return cashBalance;
    }
    void buy(const std::string& ticker, double quantity, const MarketData& marketData) {
        double cost = marketData.getCurrentData(ticker).close * quantity;
        if (cashBalance < cost) {
            throw std::runtime_error("Not enough cash to buy.");
        }
        addAsset(ticker, quantity);
        adjustCashBalance(-cost);
    }
    void sell(const std::string& ticker, double quantity, const MarketData& marketData) {
        double earnings = marketData.getCurrentData(ticker).close * quantity;
        if (getAssetQuantity(ticker) < quantity) {
            throw std::runtime_error("Not enough assets to sell.");
        }
        removeAsset(ticker, quantity);
        adjustCashBalance(earnings);
    }
    // Method to compute the portfolio's total value using the market data.
    double computeTotalValue(const MarketData& marketData) const {
        double totalValue = cashBalance;
        for (const auto& [ticker, quantity] : assets) {
            if (marketData.getCurrentData(ticker).close != 0) { // Check for non-zero close price
                totalValue += quantity * marketData.getCurrentData(ticker).close;
            }
        }
        return totalValue;
    }
};

class State {
private:
    Portfolio currentPortfolio;            // The current status of our portfolio.
    MarketData currentMarketData;          // The latest market data.
    double historicalPerformance;          // Past performance metric of the portfolio.
    // ... any other relevant state-related data members ...

public:
    // Constructor initializing the state with a portfolio and market data.
    State(const Portfolio& portfolio, const MarketData& marketData)
        : currentPortfolio(portfolio), currentMarketData(marketData), historicalPerformance(0.0) {}

    // Getter for current portfolio.
    Portfolio& getPortfolio() {
        return currentPortfolio;
    }

    // Getter for current market data.
    MarketData& getMarketData() {
        return currentMarketData;
    }

    // Getter for historical performance.
    double getHistoricalPerformance() const {
        return historicalPerformance;
    }

    // Setter for historical performance.
    void setHistoricalPerformance(double performance) {
        historicalPerformance = performance;
    }

    // Method to update the state based on new market data.
    void updateState(const MarketData& newMarketData) {
        currentMarketData.next();
    }

    tensorflow::Tensor toTensor() const {
        // Convert state to Tensor representation
        // For this example, we'll just use historical data of a single ticker
        auto historicalData = currentMarketData.getHistoricalData("TICKER", 10); // last 10 days
        tensorflow::Tensor tensor(tensorflow::DT_FLOAT, {1, 60}); // 10 days * 6 data points
        auto tensor_mapped = tensor.tensor<float, 2>();
        int idx = 0;
        for (const auto& dataPoint : historicalData) {
            tensor_mapped(0, idx++) = dataPoint.open;
            tensor_mapped(0, idx++) = dataPoint.close;
            tensor_mapped(0, idx++) = dataPoint.high;
            tensor_mapped(0, idx++) = dataPoint.low;
            tensor_mapped(0, idx++) = dataPoint.volume;
            tensor_mapped(0, idx++) = dataPoint.adjustedClose;
        }
        return tensor;
    }

};

class Action {
public:
    enum ActionType {
        BUY,
        SELL,
        HOLD
    };

private:
    ActionType type;       // The type of action.
    std::string ticker;   // The ticker symbol of the asset involved in the action.
    double quantity;      // The quantity of the asset to buy/sell.

public:
    // Constructor initializing the action with its type, ticker, and quantity.
    Action(ActionType actionType, const std::string& assetTicker, double assetQuantity)
        : type(actionType), ticker(assetTicker), quantity(assetQuantity) {}

    // Getter for the action type.
    ActionType getType() const {
        return type;
    }

    // Getter for the ticker symbol.
    std::string getTicker() const {
        return ticker;
    }

    // Getter for the quantity.
    double getQuantity() const {
        return quantity;
    }
};

class Experience {
public:
    State state;
    Action action;
    double reward;
    State nextState;
    bool done;

    Experience(const State& s, const Action& a, double r, const State& ns, bool d)
        : state(s), action(a), reward(r), nextState(ns), done(d) {}
};


class TradingEnvironment {
private:
    State currentState;            // Current state of the trading system.
    double initialBalance;         // Initial balance for the trading system.
    int currentDay;                // Current trading day.
    const int maxTradingDays = 252; // Assumed number of trading days in a year.

public:
    // Constructor to initialize the environment.
    TradingEnvironment(const State& startingState, double startingBalance)
        : currentState(startingState), initialBalance(startingBalance), currentDay(0) {}


    // Execute an action in the environment.
    std::tuple<State, double, bool> step(const Action& action) {
        double previousPortfolioValue = currentState.getPortfolio().totalValue(currentState.getMarketData());

        // Apply the action to the current state.
        switch(action.getType()) {
            case Action::BUY:
                currentState.getPortfolio().buy(action.getTicker(), action.getQuantity(), currentState.getMarketData());
                break;

            case Action::SELL:
                currentState.getPortfolio().sell(action.getTicker(), action.getQuantity(), currentState.getMarketData());
                break;

            case Action::HOLD:
                // Do nothing.
                break;
        }

        // Update market data for the next day (for simplicity, we'll assume the market data is updated elsewhere).
        currentDay++;
        currentState.updateState();

        double newPortfolioValue = currentState.getPortfolio().totalValue(currentState.getMarketData());

        // Reward is the change in portfolio value.
        double reward = newPortfolioValue - previousPortfolioValue;

        // Check if the trading period is over.
        bool done = (currentDay >= maxTradingDays);

        return std::make_tuple(currentState, reward, done);
    }

    // Reset the environment to its initial state.
    State reset() {
        currentState = State(Portfolio(initialBalance), MarketData({{"TICKER", {}}}));
        currentDay = 0;
        return currentState;
    }
    int stateSize() const {
        return 60;
    }

    int actionSize() const {
        return 3;
    }
  
};


class DQNAgent {
private:
    tensorflow::Scope scope;
    tensorflow::ClientSession session;

    tensorflow::Output modelInput;  // Input placeholder for the neural network.
    tensorflow::Output qValues;     // Q-values computed by the neural network.

    tensorflow::Output targetQValues; // Placeholder for target Q-values.
    tensorflow::Output loss;          // Loss function.
    tensorflow::Operation trainOp;    // Training operation.
    tensorflow::Operation initOp;

    double epsilon;  // Exploration rate.
    const double discountFactor = 0.95; // Define hyperparameters as needed.
    std::deque<Experience> memory; // Experience replay memory
    const size_t memoryCapacity = 10000;
    const size_t batchSize = 64;

public:
    DQNAgent(int inputSize, int outputSize) 
        : scope(tensorflow::Scope::NewRootScope()), session(scope) {
        
        // Initialize a simple neural network with one hidden layer for demonstration.
        modelInput = tensorflow::ops::Placeholder(scope.WithOpName("input"), tensorflow::DT_FLOAT);
        
        auto weightsHidden = tensorflow::ops::Variable(scope.WithOpName("weightsHidden"), {inputSize, 128}, tensorflow::DT_FLOAT);
        auto biasesHidden = tensorflow::ops::Variable(scope.WithOpName("biasesHidden"), {128}, tensorflow::DT_FLOAT);
        auto hidden = tensorflow::ops::Relu(scope.WithOpName("hidden"), modelInput * weightsHidden + biasesHidden);

        auto weightsOutput = tensorflow::ops::Variable(scope.WithOpName("weightsOutput"), {128, outputSize}, tensorflow::DT_FLOAT);
        auto biasesOutput = tensorflow::ops::Variable(scope.WithOpName("biasesOutput"), {outputSize}, tensorflow::DT_FLOAT);
        qValues = hidden * weightsOutput + biasesOutput;

        targetQValues = tensorflow::ops::Placeholder(scope.WithOpName("target"), tensorflow::DT_FLOAT);
        loss = tensorflow::ops::ReduceMean(scope, tensorflow::ops::Square(scope, qValues - targetQValues));

        // Using Adam Optimizer for this demonstration.
        trainOp = tensorflow::ops::ApplyAdam(scope, 
            {weightsHidden, biasesHidden, weightsOutput, biasesOutput}, 
            {0, 0, 0, 0, 0, 0, 0, 0}, 
            {0, 0, 0, 0}, 
            0.001, 0.9, 0.999, 1e-8, loss);

        // Define the experience replay memory
        memory = std::deque<Experience>();

        // Global variables initializer for Tensorflow
        initOp = tensorflow::ops::GlobalVariablesInitializer(scope);
        session.Run({}, {}, {initOp});

        // Initialize epsilon for exploration.
        epsilon = 1.0;
    }

    // Select an action based on the current state.
    Action selectAction(const State& state) {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        std::random_device rd;
        std::mt19937 generator(rd());

        if (distribution(generator) < epsilon) {
            // Explore: select a random action.
            return Action(/* random action */);
        } else {
            // Exploit: select the action with the highest Q-value.
            std::vector<tensorflow::Tensor> outputs;
            TF_CHECK_OK(session.Run({{modelInput, state.toTensor()}}, {qValues}, &outputs));
            int bestActionIndex = /* index of maximum value in outputs[0] */;
            return Action(bestActionIndex);
        }
    }
    
    void train(const std::vector<Experience>& experienceBatch) {
        // Experience is a structure that holds state, action, reward, next state, and done flag.

        std::vector<tensorflow::Tensor> currentStates;
        std::vector<tensorflow::Tensor> targetQValuesBatch;

        for (const auto& experience : experienceBatch) {
            currentStates.push_back(experience.state.toTensor());

            // Compute the target Q-value for the action taken.
            tensorflow::Tensor targetQValue;
            if (experience.done) {
                targetQValue = tensorflow::Tensor(experience.reward);
            } else {
                std::vector<tensorflow::Tensor> qValuesNext;
                TF_CHECK_OK(session.Run({{modelInput, experience.nextState.toTensor()}}, {qValues}, &qValuesNext));

                // We use the Q-value of the best action as the target.
                double maxQValueNext = qValuesNext[0].vec<float>().maximum();
                targetQValue = tensorflow::Tensor(experience.reward + discountFactor * maxQValueNext);
            }

            targetQValuesBatch.push_back(targetQValue);
        }

        // Train the neural network.
        session.Run({{modelInput, currentStates}, {targetQValues, targetQValuesBatch}}, {trainOp}, nullptr);

        // Gradually reduce epsilon over time.
        epsilon *= 0.995;
    }
    void remember(const State& state, const Action& action, double reward, const State& nextState, bool done) {
        if (memory.size() == memoryCapacity) {
            memory.pop_front();
        }
        memory.push_back(Experience(state, action, reward, nextState, done));
    }

    Action act(const State& state) {
        if (static_cast<float>(rand()) / RAND_MAX < epsilon) {
            // Explore: Return a random action
            // For simplicity, let's assume there are three actions: BUY, SELL, and HOLD for a single ticker.
            // You can expand this to handle multiple tickers or more complex actions.
            int randomAction = rand() % 3;
            switch(randomAction) {
                case 0:
                    return Action(Action::BUY, "TICKER", rand() % 100 + 1);  // Random quantity between 1 and 100
                case 1:
                    return Action(Action::SELL, "TICKER", rand() % 100 + 1); // Random quantity between 1 and 100
                default:
                    return Action(Action::HOLD, "TICKER", 0);
            }
        } else {
            // Exploit: Return the action with the highest Q-value based on the neural network
            std::vector<tensorflow::Tensor> outputs;
            TF_CHECK_OK(session.Run({{modelInput, state.toTensor()}}, {qValues}, &outputs));
            
            // Assume Q-values tensor is 1D. Find the index of the maximum Q-value which corresponds to the best action.
            auto q_values_t = outputs[0].flat<float>();
            int bestActionIndex = std::distance(&q_values_t(0), std::max_element(&q_values_t(0), &q_values_t(0) + q_values_t.size()));
            
            // Convert bestActionIndex to an actual Action object. For simplicity, we'll consider only 3 actions like before.
            switch(bestActionIndex) {
                case 0:
                    return Action(Action::BUY, "TICKER", rand() % 100 + 1);  // Random quantity for demonstration
                case 1:
                    return Action(Action::SELL, "TICKER", rand() % 100 + 1); // Random quantity for demonstration
                default:
                    return Action(Action::HOLD, "TICKER", 0);
            }
        }
    }
    size_t memorySize() const {
        return memory.size();
    }

    size_t batchSize() const {
        return batchSize;
    }
    std::vector<Experience> sample() {
        std::vector<Experience> batch;
        std::sample(memory.begin(), memory.end(), std::back_inserter(batch), batchSize, std::mt19937{std::random_device{}()});
        return batch;
    }
    void saveModel(const std::string& path) {
        // Placeholder: Save the model weights to the specified path
        // You would implement this using TensorFlow's save functionality
    }    
};

int main() {
    // Initialize environment and agent
    TradingEnvironment env(State(Portfolio(1000.0), MarketData({{"TICKER", {}}}));
    DQNAgent agent(env.stateSize(), env.actionSize());

    int numberOfEpisodes = 1000;
    int maxStepsPerEpisode = 200;

    for (int episode = 0; episode < numberOfEpisodes; ++episode) {
        State state = env.reset();  // Reset the environment to start a new episode
        double episodeReward = 0;

        for (int step = 0; step < maxStepsPerEpisode; ++step) {
            Action action = agent.act(state); // Let the agent decide on an action based on current state

            // Perform the action in the environment
            auto [nextState, reward, done] = env.step(action);
            
            // Store the experience in the agent's memory
            agent.remember(state, action, reward, nextState, done);

            // Update the state and accumulate reward
            state = nextState;
            episodeReward += reward;

            // Train the agent with a batch of experiences
            if (agent.memorySize() > agent.batchSize()) {
                std::vector<Experience> batch = agent.sample();
                agent.train(batch);
            }

            // If the episode is done, break out of the loop
            if (done) {
                break;
            }
        }

        std::cout << "Episode: " << episode + 1 << " | Total reward: " << episodeReward << std::endl;

        // Optionally, you can periodically save the model
        if ((episode + 1) % 100 == 0) {
            agent.saveModel("path_to_save_directory/model_" + std::to_string(episode + 1) + ".ckpt");
        }
    }

    return 0;
}
