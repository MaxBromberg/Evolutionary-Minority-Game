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

using namespace std;
//The Old
vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n) {
    return BinaryArrayToStrategyIndex (v.end() - n, v.end());
}

vector<signum> binaryMarketHistoryGenerator (int NumIndicesInStrategy, int seed){ //fills a vector with +/-1
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
signum Agent::predict(const int strategy_index, const int num_indicies_in_strategy, const std::vector<signum>& history) const {
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

int market_evaluation(const std::vector<Agent>& Agents, const std::vector<signum>& history){
    int market_evaluation = 0;
    for(int agent_index = 0; agent_index < Agents.size(); agent_index++){

        vector<int> strategy_scores;
        for(int i = 0; i < Agents[agent_index].strategies.size(); i++){
            strategy_scores.push_back(Agents[agent_index].strategies[i].strategy_score);
        }//Likely better way to find the max element...
        auto index_of_best_strategy = std::max_element(std::begin(strategy_scores), std::end(strategy_scores))-std::begin(strategy_scores);

        if(Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy > 32){
            cout<<"Agent_index = "<<agent_index<<" of "<<Agents.size()<<" total agents "<<endl;
            cout<<"index of best strategy = "<<index_of_best_strategy<<" of "<<Agents[agent_index].strategies.size()<<" total strategies"<<endl;
            cout<<"num_indicies reads: "<<Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy<<endl;
        }//debugging

        market_evaluation += Agents[agent_index].predict(index_of_best_strategy, Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy, history);
    }
    assert(abs(market_evaluation) <= Agents.size()); //failed before, though seemingly without reason
    return market_evaluation;
}

vector<Agent> Experiment::initialize_agents() {
    vector<Agent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, num_indicies_in_strategy});
        }
        Agents.push_back(Agent{Strategies, i});
        /*
        i is a perfectly unique identifier; we only have to deal in complexity after initialization, where we could
        get random numbers in the range of 1 billion, and still be fine for memories of up to 15 bits.
        */ //Note on identifier range, allocation
    }
    return Agents;
}

Experiment::Experiment(int agent_population, int num_strategies_per_agent, int memory, int seed) {
    assert(agent_population % 2 == 1);//compiled to bitshift
    agent_count = agent_population;
    strategies_per_agent = num_strategies_per_agent;
    num_indicies_in_strategy = memory;
    history = binaryMarketHistoryGenerator(memory, seed);
    nonbinary_history = marketHistoryGenerator(history, agent_population, seed);
    agents = Experiment::initialize_agents();
}

void Experiment::run_minority_game(int number_of_runs){
    for(int i = 0; i < number_of_runs; i++){
        auto market_count = market_evaluation(agents, history);
        assert (market_count >= -agent_count || market_count <= agent_count);

        auto binary_market_value = market_count > 0 ? -1 : 1;
        for(int j = 0; j < agents.size(); i++){
            agents[j].update(history, binary_market_value);
        }

        nonbinary_history.push_back(market_count);
        history.push_back(binary_market_value);
    }
}
