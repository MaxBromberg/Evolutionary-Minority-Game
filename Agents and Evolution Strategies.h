#pragma once
#include "Evolutionary_Minority_Game.h"
#include "Minority_Game_Utilities.h"

// ***************************************************************************
//  Agents and Evolution Strategies
// ***************************************************************************

struct Strategy {
    int id;
    int num_indicies_in_strategy;
    int strategy_score;

    Strategy (int id, int num_indicies_in_strategy) :
            id (id), num_indicies_in_strategy (num_indicies_in_strategy), strategy_score (0) {}
};

// ***************************************************************************
//                               Agents Types
// ***************************************************************************

struct AlphaAgent : public Agent {
    std::vector<Strategy> strategies;
    int total_wins;
    int id;

    AlphaAgent (std::vector<Strategy> s, int w, int i);

    AlphaAgent (int id, int num_strategies, int num_indicies_in_strategy);

    signum sin_predict(const int strategy_index, int history_index) const;

    virtual signum get_prediction(const MarketHistory &history) override;

    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);

    virtual void update(const MarketHistory &history, signum binary_market_result) override;

    virtual void print() override;

    virtual void agent_memory_boost();
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual double win_percentage_of_streak();
};

struct DarwinianAgent : public Agent{
    std::vector<Strategy> strategies;
    std::vector<signum> last_n_actions;
    int evolution_period;
    int total_wins;
    int id;

    DarwinianAgent (std::vector<Strategy>& strategies, int evol_period, int tot_wins, int i);
    DarwinianAgent (int id, int evol_period, int num_strategies, int num_indicies_in_strategy);
    signum sin_predict(const int strategy_index, int history_index) const;
    virtual signum get_prediction(const MarketHistory &history) override;
    virtual void update(const MarketHistory &history, signum binary_market_result) override;
    virtual void print() override;
    virtual void agent_memory_boost();
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);
    virtual double win_percentage_of_streak();
};

class RandomAgent : public Agent {
    int seed;
    int rng_resolution;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
    //int num_indicies_in_strategy;
public:
    RandomAgent (int seed, int rng_resolution);

    virtual signum get_prediction(const MarketHistory &history);

    virtual void print();

    /*
    RandomAgent (int seed, int num_indicies_in_strategy) : seed {seed}, num_indicies_in_strategy {num_indicies_in_strategy} {}
    virtual signum get_prediction(const MarketHistory &history) {
        return ((int)(sin((seed*history.last_n_results_as_bits(num_indicies_in_strategy)))*1000000.0) & 1) ? 1 : -1;
    }
    virtual void print() {printf ("RandomAgent seed %i, num_indicies_in strategy = %i\n", seed, num_indicies_in_strategy); }
    */

    //the following are not used; just but it in because it's the superclass.
    virtual void agent_memory_boost();
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual double win_percentage_of_streak();
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);
    virtual void update(const MarketHistory &history, signum market_result);
};

// **********************************************************************
//                           Evolution Methodologies
// **********************************************************************

class Creationism : public EvolutionStrategy {
public:
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override;
    void evolutionary_update();
}; //Simply returns the previous state

class Darwinism: public EvolutionStrategy {
    double threshold = 0.9;
public:
    Darwinism (double win_threshold_percentage);
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override;
    virtual std::vector<Agent*> evolutionary_update(const MarketHistory& history, AgentPool& agent_pool, double threshold);

}; //Simply returns the previous state

class CyclicEvolution : public EvolutionStrategy {
    int last_used_agent_id;
public:
    CyclicEvolution (int initial_agent_population);
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override;
    void evolutionary_update();
}; //deletes the last agent and generates a new one at the other end.

/*
class CloningEvolution: public EvolutionStrategy {
    int last_used_agent_id;
public:
    CloningEvolution (int initial_agent_population) : last_used_agent_id (initial_agent_population - 1) {}

    virtual bool evolutionary_threshold(const MarketHistory& history, AgentPool& agent_pool, double threshold){
        double evaluation = (double) accumulate(last_n_actions.end()-evolution_history_length, last_n_actions.end(), 0)/ (double) evolution_history_length;
        agent_pool.back()
    }
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        std::vector<Agent*> next_gen = history.last_day().agents();

        next_gen.pop_back();

        next_gen.insert (next_gen.begin(), create_agent (agent_pool, new RandomAgent {last_used_agent_id + 1}));
        ++last_used_agent_id;

        return std::move (next_gen);
    }
}; //Should spawn a slightly modified strategy set, same preferences
*/