#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>


typedef int signum; //To hold the +/-1s, and indicate return type. (binary history --> history)
//The Old
std::vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);
std::vector<signum> binaryMarketHistoryGenerator(int NumIndicesInStrategy, int seed);
std::vector<int> marketHistoryGenerator(const std::vector<int>& source, int agentPopulation, int seed);

template <typename Iter>
int BinaryArrayToStrategyIndex (Iter begin, Iter end) {
    assert ((end - begin) <= 32);
    int output = 0;
    for (auto iter = begin; iter != end; ++iter) {
        if (*iter == 1) output += 1 << iter - begin;
    }
    return output;
}

//The New
struct Strategy {
    int strategy_score;
    int num_indicies_in_strategy;
};

struct Agent {
    std::vector<Strategy> strategies;
    int relatively_unique_identifer;

    signum predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history) const;
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

    std::vector<Agent> initialize_agents();
    Experiment(int, int, int, int);

    void run_minority_game(int number_of_runs);
};

