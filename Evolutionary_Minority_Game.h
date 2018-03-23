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
    virtual signum get_thermal_prediction(const MarketHistory &history) = 0;
    virtual void update(const MarketHistory &history, signum market_result) = 0;
    virtual void thermal_update(const MarketHistory &history, signum market_result) = 0;
    virtual void print() = 0;

//Strategies should be their own class and inherent to Agents, rather than the repeat virtual declarations below
    virtual int return_num_strategies() = 0;
    virtual int return_memory(int strategy_index) = 0;
    virtual double win_percentage_of_streak() = 0;
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result) = 0;
    virtual void weighted_thermal_update(const MarketHistory &history, signum binary_market_result) = 0;
    virtual void agent_memory_boost(int max_memory) = 0;
    virtual void agent_memory_deduction(int min_memory) = 0;
    virtual void agent_add_strategy(int num_indicies_in_new_strategy) = 0;
    virtual void agent_subtract_strategy(int strategy_index) = 0;
    virtual int return_evolutionary_period() = 0;
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
    double memory_threshold = -1;
    double strategy_threshold = -1;
    double population_threshold = -1;
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
    const vector<int> generate_strategy_vector() const;
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
    double ave_memory_at_n(int n) const;
    double ave_strategy_count_at_n(int n) const;
    int history_size() const;
//Neither market history functions work, as for whatever reason the history.size inside evaluates to prehistory size. Not sure why.
//    vector<int> nonbinary_market_history() const;
//    vector<signum > binary_market_history() const;
    void add_day (MarketDay new_day);
    void print();
    void write_market_history(int last_num_days_printed);
    void labeled_write_market_history(double memory_delta, int evolutionary_period, int last_num_days_printed);
    void strategy_labeled_write_market_history(double strategy, int evolutionary_period, int last_num_days_printed);
    void breeding_labeled_write_market_history(double strategy, int evolutionary_period, int last_num_days_printed);
    void thermal_labeled_write_market_history(double memory_delta, int evolutionary_period, int last_num_days_printed);
    void thermal_strategy_labeled_write_market_history(double strategy, int evolutionary_period, int last_num_days_printed);
    void thermal_breeding_labeled_write_market_history(double strategy, int evolutionary_period, int last_num_days_printed);
    vector<int> return_memories_at_date(int date) const;
    vector<int> return_strategies_at_date(int date) const;
};

class ExperimentState {
    AgentPool agent_pool;
    std::unique_ptr<EvolutionStrategy> evolution_strategy;
    MarketHistory market_history;

public:
    ExperimentState (std::vector<MarketDay> pre_history, std::unique_ptr<EvolutionStrategy> evolution, AgentPool agents);
    MarketHistory* return_market_history();
    void simulate_day();
    void thermal_simulate_day();
    void simulate (int num_days);
    void thermal_simulate (int num_days);
    void print();
    void write_last_n_market_history(int num_days_printed);
    void write_agent_populations();
    int return_agent_pop(int date);
    double return_ave_memory(int date);
    double return_ave_strategy(int date);
    double return_memory_threshold();
    double return_strategy_threshold();
    double return_population_threshold();
    void write_memory_frequencies(int date);
    void labeled_write_last_n_market_history(double memory_delta, int evolutionary_period, int num_days_printed);
    void thermal_labeled_write_last_n_market_history(double memory_delta, int evolutionary_period, int num_days_printed);
    void strategy_labeled_write_last_n_market_history(double strategy_delta, int evolutionary_period, int num_days_printed);
    void population_labeled_write_last_n_market_history(double strategy_delta, int evolutionary_period, int num_days_printed);
    void labeled_write_memory_frequencies(double memory_delta, int evolutionary_period, int date);
    void thermal_strategy_labeled_write_last_n_market_history(double strategy_delta, int evolutionary_period, int num_days_printed);
    void thermal_labeled_write_memory_frequencies(double memory_delta, int evolutionary_period, int date);
    void write_mean_memories(int end_of_range);
    void labeled_write_mean_memories(double memory_delta, int evolutionary_period,int end_of_range);
    void thermal_labeled_write_mean_memories(double memory_delta, int evolutionary_period, int end_of_range);
    void labeled_write_strategy_frequencies(double strategy_delta, int evolutionary_period, int date);
    void labeled_write_mean_strategies(double strategy_delta, int evolutionary_period, int end_of_range);
    void thermal_labeled_write_strategy_frequencies(double strategy_delta, int evolutionary_period, int date);
    void thermal_labeled_write_mean_strategies(double strategy_delta, int evolutionary_period, int end_of_range);
    void labeled_write_populations(double breeding_delta, int evolutionary_period, int end_of_range);
    void thermal_labeled_write_populations(double breeding_delta, int evolutionary_period, int end_of_range);
};

//*****************************Initializations*************************************
std::vector<MarketDay> basic_pre_history(int size, int seed, int num_agents);
//Main Functions
void write_market_histories(int rng_resolution, int rng_seed, int min_agent_pop, int max_agent_pop, int pop_interval, int runtime);
void write_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int min_memory, int max_memory, int memory_interval);

void write_thermal_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int min_memory, int max_memory, int memory_interval);


//These actions are for evolutionary methods that allow for the evolutionary threshold to evolve with the distribution.
////Memory Evolution
void write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length,
                                              int init_evolutionary_length, int num_evolutionary_lengths, int iterated_evolutionary_length,
                                              double memory_delta, double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);


void thermal_write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length,
                                              int init_evolutionary_length, int num_evolutionary_lengths, int iterated_evolutionary_length,
                                              double memory_delta, double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

////Strategy Evolution
void write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                int agent_pop, int memory_length, int num_memory_iterations,
                                                int starting_evolutionary_length,
                                                int evolutionary_length, int num_evolutionary_lengths,
                                                double memory_delta, double strategy_delta, double breeding_delta,
                                                int max_evol_memory, int min_evol_memory,
                                                int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void thermal_write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                int agent_pop, int memory_length, int num_memory_iterations,
                                                int starting_evolutionary_length,
                                                int evolutionary_length, int num_evolutionary_lengths,
                                                double memory_delta, double strategy_delta, double breeding_delta,
                                                int max_evol_memory, int min_evol_memory,
                                                int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

//These methods are for definite ranges, varying over evolutionary threshold without an evolutionary mechanism inducing it.
void write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length, int num_memory_delta_values,
                                              int init_evolutionary_length, int num_evolutionary_lengths, int iterated_evolutionary_length,
                                              double memory_delta, double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void thermal_write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length, int num_memory_delta_values,
                                               int init_evolutionary_length, int num_evolutionary_lengths, int iterated_evolutionary_length,
                                              double memory_delta, double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                int agent_pop, int memory_length, int num_memory_iterations,
                                                int num_strategy_delta_values, int starting_evolutionary_length,
                                                int evolutionary_length, int num_evolutionary_lengths,
                                                double memory_delta, double strategy_delta, double breeding_delta,
                                                int max_evol_memory, int min_evol_memory,
                                                int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void thermal_write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length, int num_memory_iterations,
                                                int num_strategy_delta_values, int starting_evolutionary_length,
                                              int evolutionary_length, int num_evolutionary_lengths,
                                                double memory_delta, double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void write_population_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                int agent_pop, int memory_length, int num_memory_iterations,
                                                int num_breeding_delta_values, int starting_evolutionary_length,
                                                int evolutionary_length, int num_evolutionary_lengths,
                                                double memory_delta, double strategy_delta, double breeding_delta,
                                                int max_evol_memory, int min_evol_memory,
                                                int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);

void thermal_write_population_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                  int agent_pop, int memory_length, int num_memory_iterations,
                                                  int num_breeding_delta_values, int starting_evolutionary_length,
                                                  int evolutionary_length, int num_evolutionary_lengths,
                                                  double memory_delta, double strategy_delta, double breeding_delta,
                                                  int max_evol_memory, int min_evol_memory,
                                                  int max_num_strategies, int min_num_strategies, int max_pop, int min_pop);
