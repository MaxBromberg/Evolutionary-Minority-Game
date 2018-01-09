#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

//The Old
std::vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);
std::vector<std::vector<int>> TwoDimensionalVector (unsigned int x_size, int y_size, int value);

typedef int signum; //To hold the +/-1s, and indicate return type. (binary history --> history)

std::vector<int> binaryMarketHistoryGenerator(int NumIndicesInStrategy);
std::vector<int> marketHistoryGenerator(const std::vector<int>& source, int agentPopulation);

//The New
struct Strategy {
    int strategy_score;
    int num_indicies_in_strategy;
};

struct Agent {
    std::vector<Strategy> strategies;
    int relatively_unique_identifer;

    static signum predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history);
    void update (const std::vector<signum>& history, const int& market_prediction);
    void weighted_update (const std::vector<signum>& history, const int& last_market_value);
};

struct Experiment {
    int agent_count;
    int strategies_per_agent;
    int num_indicies_in_strategy;
    std::vector<signum> history;
    std::vector<int> nonbinary_history;
    std::vector<Agent> agents;

    static std::vector<Agent> initialize_agents();
    Experiment(int, int, int, int);

    static void run_minority_game(int number_of_runs);
};

