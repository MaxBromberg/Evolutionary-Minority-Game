#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "Evolutionary_Minority_Game.h"
#include "analysis_utilities.h"

enum {
    MARKET_HISTORY_INIT_SEED = 123,
    RNG_RESOLUTION = 10000
};

using namespace std;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
//The Old
vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

template <typename Iter>
static int BinaryArrayToStrategyIndex (Iter begin, Iter end) {
    assert (end - begin <= 32);
    int output = 0;
    for (auto iter = begin; iter != end; ++iter) {
        if (*iter == 1) output += 1 << iter - begin;
    }
    return output;
}

int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n) {
    return BinaryArrayToStrategyIndex (v.end() - n, v.end());
}

vector<vector<int>> TwoDimensionalVector (unsigned int x_size, int y_size, int value) { //Generates a 2D vector and fills it with value
    vector<vector<int>> result;
    result.resize (x_size, vector<int> (y_size, value));
    return (result);
}

vector<int> binaryMarketHistoryGenerator (int NumIndicesInStrategy, int seed){ //fills a vector with +/-1
    return RandomBoolVector (NumIndicesInStrategy, seed, 1, -1);
}

vector<int> marketHistoryGenerator(const vector<int>& source, int agentPopulation, int seed){
    mt19937 generator {seed};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop). Keeping it for conformity

//The New
static signum Agent::predict(const int strategy_index, const int num_indicies_in_strategy, const std::vector<signum>& history) {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    mt19937 generator ((unsigned int) ( (relatively_unique_identifer + strategy_index) * BinaryVectorLastNToStrategyIndex (history, num_indicies_in_strategy)));
    uniform_int_distribution<int> bit_distribution(0,1);
    return bit_distribution(generator) ? 1 : -1;
    //Perhaps I could alternatively take a bit from a ctan distribution for faster rng generation.
}

void Agent::update(const std::vector<signum> &history, const int& market_prediction) {
    for(int i = 0; i < strategies.size(); i++){
        if (Agent::predict(i, strategies[i].num_indicies_in_strategy, history) == market_prediction) {
            strategies[i].strategy_score++;
        } else {
            strategies[i].strategy_score--;
        }
    }
} //Updates all strategies in vector

void Agent::weighted_update(const std::vector<signum> &history, const int& last_market_value) {
    for(int i = 0; i < strategies.size(); i++){
        if (Agent::predict(i, strategies[i].num_indicies_in_strategy, history) == sgn(last_market_value)) {
            strategies[i].strategy_score += last_market_value;
        } else {
            strategies[i].strategy_score -= last_market_value;
        }
    }
} //Updates all strategies in vector with the amount they were in/correct by.

int market_evaluation(const std::vector<Agent>& Agents, const std::vector<signum> &history){
    int market_evaluation = 0;
    for(int agent_index = 0; agent_index < Agents.size(); agent_index++){
        auto index_of_best_strategy = std::max_element (Agents[agent_index].strategies.begin(), Agents[agent_index].strategies.end());
        //int num_indicies_in_strategy = Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy;
        market_evaluation += Agent::predict(index_of_best_strategy, Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy, history);
    }
    assert(abs(market_evaluation) <= Agents.size()); //failed before, though seemingly without reason
    return market_evaluation;
}

vector<Agent> Experiment::initialize_agents() {
    vector<Agent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy{0, num_indicies_in_strategy});
        }
        Agents.push_back(Agent{Strategies, i});
        /*
        i is a perfectly unique identifier; we only have to deal in complexity after initialization, where we could
        get random numbers in the range of 1 billion, and still be fine for memories of up to 15 bits.
        */
    }
    return Agents;
}

Experiment::Experiment(int agent_population, int num_strategies_per_agent, int memory, int seed) {
    agent_count = agent_population;
    strategies_per_agent = num_strategies_per_agent;
    num_indicies_in_strategy = memory;
    history = binaryMarketHistoryGenerator(memory, seed);
    nonbinary_history = marketHistoryGenerator(history, agent_population, seed);
    agents = Experiment::initialize_agents();
}

static void Experiment::run_minority_game(int number_of_runs){
    for(int i = 0; i < number_of_runs; i++){
        auto market_count = market_evaluation(agents, history);
        assert (market_count >= -agent_count || market_count <= agent_count);
        auto binary_market_value = market_count > 0 ? -1 : 1;
        Agent::update(history, binary_market_value);
        nonbinary_history.push_back(market_count);
        history.push_back(binary_market_value);
    }
}
