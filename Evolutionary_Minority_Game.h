#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <memory>
#include <cstdio>
#include <random>
#include <fstream>
#include "analysis_utilities.h"
#include "Minority_Game_Utilities.h"

typedef int signum; //To hold the +/-// 1s, and indicate return type. (binary history --> history)

// ***************************************************************************
//  Minority Game Engine
// ***************************************************************************

class MarketHistory;

class Agent {
public:
    virtual ~Agent() {}
    virtual signum get_prediction(const MarketHistory &history) = 0;
    virtual void update(const MarketHistory &history, signum market_result) = 0;
    virtual void print() = 0;

//Strategies should be their own class and inherent to Agents, rather than the repeat virtual declarations below
    virtual int return_num_strategies() = 0;
    virtual int return_memory(int strategy_index) = 0;
    virtual double win_percentage_of_streak() = 0;
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result) = 0;
    virtual void agent_memory_boost() = 0;
    virtual void agent_add_strategy(int num_indicies_in_new_strategy) = 0;
};

typedef std::vector<std::unique_ptr<Agent>> AgentPool;

/*
class StrategyAgent : public Agent {
public:
    std::vector<std::unique_ptr<IStrategy>> strategies;
    IStrategy& get_best_strategy() {
        return *std::max_element (strategies.begin(), strategies.end(), [] (const std::unique_ptr<IStrategy>& lhs, const std::unique_ptr<IStrategy>& rhs) {return lhs->get_score() < rhs->get_score();})
    }

    void score_strategies (signum market_result) {
        for (auto &s : strategies) {
            if (s->get_last_prediction() == market_result) {
                s->add_to_score(1);
            }
            else {
                s->add_to_score(-1);
            }
        }

    }
};
 */ //Blueprint for generalizing the functionality of strategies in the agent
/*
class IStrategy {
    int score = 0;
    signum last_prediction = 0;
    virtual signum do_prediction (const MarketHistory& history) = 0;
public:
    virtual ~IStrategy() = 0;
    void add_to_score (int value) {score += value;}
    int get_score() {return score;}
    signum get_last_prediction() {return last_prediction;}
    signum predict(const MarketHistory& history) {
        last_prediction = do_prediction (history);
        return last_prediction;
    }
};
 */

static inline Agent* create_agent (AgentPool& pool, Agent* new_agent) {
    pool.push_back (std::unique_ptr<Agent> {new_agent});
    return new_agent;
} //Does not let itself to be put in the cpp file, for whatever reason...
static inline void delete_agent (AgentPool& pool, int agent_index);

class EvolutionStrategy {
public:
    virtual ~EvolutionStrategy() {}
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) = 0;
};

class MarketDay {
    int m_index;
    int m_market_prediction;
    signum m_result;
    std::vector<Agent *> m_agents;
public:
    MarketDay(int index, int market_prediction, signum result);
    MarketDay(int index, std::vector<Agent *> agents, int market_prediction, signum result);
    signum result() const;
    int market_count() const;
    const std::vector<Agent *> &agents() const;
    void print();
    void reset_agents(std::vector<Agent *> agents);
    const vector<int> generate_memory_vector() const;
};

class MarketHistory {
    std::vector<MarketDay> history;
    int num_days_pre_history;
public:
    explicit MarketHistory (std::vector<MarketDay> pre_history);
    int index_of_current_day() const;
    uint64_t last_n_results_as_bits(int n) const;
    const MarketDay& last_day() const;
    MarketDay& last_day();
    signum last_result() const;
    int market_count() const;
    int market_result() const;
    int market_count_at_day_i(int i) const;
    int population_at_n(int n) const;
    int history_size() const;
//Neither market history functions work, as for whatever reason the history.size inside evaluates to prehistory size. Not sure why.
    vector<int> nonbinary_market_history() const;
    vector<signum > binary_market_history() const;
    void add_day (MarketDay new_day);
    void print();
    void write_market_history(int last_num_days_printed);
    vector<int> return_memories_at_date(int date) const;
};

class ExperimentState {
    AgentPool agent_pool;
    std::unique_ptr<EvolutionStrategy> evolution_strategy;
    MarketHistory market_history;

public:
    ExperimentState (std::vector<MarketDay> pre_history, std::unique_ptr<EvolutionStrategy> evolution, AgentPool agents);
    MarketHistory* return_market_history();
    void simulate_day();
    void simulate (int num_days);
    void print();
    void write_last_n_market_history(int num_days_printed);
    void write_agent_populations();
    void write_memory_frequencies(int date);
};

//*****************************Initializations*************************************
std::vector<MarketDay> basic_pre_history(int size, int seed, int num_agents);
//Main Functions
void write_market_histories(int rng_resolution, int rng_seed, int min_agent_pop, int max_agent_pop, int pop_interval, int runtime);
void write_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int min_memory, int max_memory, int memory_interval);
