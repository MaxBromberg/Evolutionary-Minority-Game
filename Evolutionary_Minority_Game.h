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


typedef int signum; //To hold the +/-// 1s, and indicate return type. (binary history --> history)

// ***************************************************************************
//  Utilities
// ***************************************************************************

std::vector<int> random_bool_vector(int size, int seed, int true_value = 1, int false_value = 0);
uint64_t signum_vector_to_bits(const std::vector<signum> &v, int n);
std::vector<signum> random_signum_vector(int size, int seed);
std::vector<int> market_history_generator(const std::vector<signum> &source, int agentPopulation, int seed);
int random_generate(double weight, int range, int seed);

// ***************************************************************************
//  Minority Game Engine
// ***************************************************************************

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

class MarketHistory;

class Agent {
public:
    virtual ~Agent() {}
    virtual signum get_prediction(const MarketHistory &history) = 0;
    virtual void update(const MarketHistory &history, signum market_result) = 0;
    virtual void print() = 0;
};

typedef std::vector<std::unique_ptr<Agent>> AgentPool;
typedef std::unique_ptr<int> fptr;
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

static inline Agent* create_agent (AgentPool& pool, Agent* new_agent) {
    pool.push_back (std::unique_ptr<Agent> {new_agent});
    return new_agent;
}

class EvolutionStrategy {
public:
    virtual ~EvolutionStrategy() {}
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) = 0;
};

class MarketDay {
    int m_index;
    int m_market_prediction;
    signum m_result;
    std::vector<Agent*> m_agents;
public:
    MarketDay(int index, int market_prediction, signum result) :
            m_index {index}, m_market_prediction {market_prediction}, m_result {result}, m_agents{} { }

    MarketDay (int index, std::vector<Agent*> agents, int market_prediction, signum result) :
        m_index {index}, m_market_prediction {market_prediction}, m_result {result}, m_agents {std::move (agents)} {}

    signum result() const {return m_result;}
    int market_count() const {return m_market_prediction;}
    const std::vector<Agent*>& agents() const {return m_agents;};

    void print() {
        printf ("Day %i : market_predicition %i result %i\n", m_index, m_market_prediction, m_result);
    }

    void reset_agents (std::vector<Agent*> agents) {
        m_agents = std::move (agents);
    }
};

class MarketHistory {
    std::vector<MarketDay> history;
    int num_days_pre_history;
public:
    MarketHistory (std::vector<MarketDay> pre_history) :
            history(std::move(pre_history)),
            num_days_pre_history(history.size())
    {}

    int index_of_current_day() const {
      return history.size() - num_days_pre_history;
    }

    uint64_t last_n_results_as_bits(int n) const {
        assert (n < 32);
        assert (history.size() >= n);

        unsigned int output = 0;
        auto begin = history.end() - n;
        for (auto iter = begin; iter != history.end(); ++iter) {
            if (iter->result() == 1) output += 1 << (iter - begin);
        }
        return output;
    }

    const MarketDay& last_day() const {return history.back();}
    MarketDay& last_day() {return history.back();}
    signum last_result() const { return history.back().result(); }
    int market_count() const {return history.back().market_count();}
    int market_result() const {return history.back().result();}
    int market_count_at_day_i(int i) const {return history[i].market_count();}

//Neither market history functions work, as for whatever reason the history.size inside evaluates to prehistory size. Not sure why.
    vector<int> nonbinary_market_history() const {
        vector<int> nonbinary_history;
        printf("history size = %i, num days of prehistory = %i", history.size(), num_days_pre_history);
        for (int i = num_days_pre_history; i < history.size() - num_days_pre_history; ++i) {
            nonbinary_history.push_back(history[i].market_count());
        }
        return nonbinary_history;
    }
    vector<signum > binary_market_history() const {
        vector<signum> binary_history;
        for (int i = num_days_pre_history; i < history.size()-num_days_pre_history; ++i) {
            binary_history.push_back(history[i].result());
        }
        return binary_history;
    }

    void add_day (MarketDay new_day) {history.emplace_back (std::move (new_day)); }

    void print() {
        for (int i = 0; i < history.size(); ++i) {
            history[i].print();
        }
    }

    void write_market_history(int last_num_days_printed) {
        ofstream attendance_history("Market History.txt");
        for (int i = history.size() + num_days_pre_history - last_num_days_printed; i < history.size(); ++i) {
            attendance_history << i << ", "
                               << history[i].market_count() << endl;
        }
    }

};

class ExperimentState {
    AgentPool agent_pool;
    std::unique_ptr<EvolutionStrategy> evolution_strategy;
    MarketHistory market_history;

public:
    ExperimentState (std::vector<MarketDay> pre_history, std::unique_ptr<EvolutionStrategy> evolution, AgentPool agents) :
            agent_pool (std::move (agents)),
            evolution_strategy (std::move (evolution)),
            market_history (std::move (pre_history))
    {
        std::vector<Agent*> first_generation;
        for (auto& a : agent_pool) {
            first_generation.push_back (a.get());
        }
        market_history.last_day().reset_agents (std::move (first_generation));
    }

    void simulate_day() {
        const int index_of_day = market_history.index_of_current_day();
        // Evolution
        auto agent_generation = evolution_strategy->select_next_generation (market_history, agent_pool);
        //assert (agent_generation.size() == history.last_day().agents().size());
        assert (agent_generation.empty() == false);

        // Agent prediction
        int market_count = 0;
        for (auto a : agent_generation) {
            market_count += a->get_prediction(market_history);
        }

        // Market decision if 0 (only relevant for evolutionary models with even agent populations)
        if (market_count == 0) {
            printf("market count == 0 !!! -> %i\n", index_of_day);
            std::mt19937 gen(index_of_day);
            while (market_count == 0) {
                auto agent_count = agent_generation.size();
                std::uniform_int_distribution<int> dist(-agent_count, agent_count);
                market_count = dist(gen);
            }
        }
        assert (market_count != 0);
        signum binary_market_result = market_count > 0 ? -1 : 1; // As we're registering the minority

        // Agent updates
        for (auto a : agent_generation) {
            a->update(market_history, binary_market_result);
        }

        // Finalizing
        market_history.add_day (MarketDay {market_history.index_of_current_day(), std::move (agent_generation), market_count, market_count > 0 ? -1 : 1});
    }
    void simulate (int num_days) { for (;num_days > 0; --num_days) simulate_day(); }

    void print() {
        printf ("Market History:\n");
        market_history.print();
        printf ("Agents:\n");
        for (auto& a : agent_pool) {
            a->print();
        }
    }

    void write_last_n_market_history(int num_days_printed) { market_history.write_market_history(num_days_printed); }

    void write_mg_observables(int num_days, int num_strategies_per_agent, int seed, int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop, int agent_pop_interval, int min_memory, int max_memory, int memory_interval);
};

std::vector<MarketDay> basic_pre_history(int size, int seed, int num_agents);
AgentPool alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor);
AgentPool random_agents(int agent_pop);

