#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <memory>
#include <cstdio>
#include <random>


typedef int signum; //To hold the +/-1s, and indicate return type. (binary history --> history)

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

class MarketHistory;

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


class Agent {
public:
    virtual ~Agent() {}
    virtual signum get_prediction(const MarketHistory &history) = 0;
    virtual void update(const MarketHistory &history, signum market_result) = 0;
    virtual void print() = 0;
};

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
 */

typedef std::vector<std::unique_ptr<Agent>> AgentPool;

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

    void add_day (MarketDay new_day) {
        history.emplace_back (std::move (new_day));
    }

    void print() {
        for (int i = 0; i < history.size(); ++i) {
            history[i].print();
        }
    }
};


class ExperimentState {
    AgentPool agent_pool;
    std::unique_ptr<EvolutionStrategy> evolution_strategy;
    MarketHistory history;

public:
    ExperimentState (std::vector<MarketDay> pre_history, std::unique_ptr<EvolutionStrategy> evolution, AgentPool agents) :
            agent_pool (std::move (agents)),
            evolution_strategy (std::move (evolution)),
            history (std::move (pre_history))
    {
        std::vector<Agent*> first_generation;
        for (auto& a : agent_pool) {
            first_generation.push_back (a.get());
        }
        history.last_day().reset_agents (std::move (first_generation));
    }

    void simulate_day() {
        const int index_of_day = history.index_of_current_day();
        // Evolution
        auto agent_generation = evolution_strategy->select_next_generation (history, agent_pool);
        //assert (agent_generation.size() == history.last_day().agents().size());
        assert (agent_generation.empty() == false);

        // Agent prediction
        int market_count = 0;
        for (auto a : agent_generation) {
            market_count += a->get_prediction(history);
        }

        // Market decision
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
        signum binary_market_result = market_count > 0 ? -1 : 1;

        // Agent updates
        for (auto a : agent_generation) {
            a->update(history, binary_market_result);
        }

        // Finalizing
        history.add_day (MarketDay {history.index_of_current_day(), std::move (agent_generation), market_count, market_count > 0 ? -1 : 1});
    }
    void simulate (int num_days) { for (;num_days > 0; --num_days) simulate_day(); }

    void print() {
        printf ("Market History:\n");
        history.print();
        printf ("Agents:\n");
        for (auto& a : agent_pool) {
            a->print();
        }
    }
};

// ***************************************************************************
//  Agents and Evolution Strategies
// ***************************************************************************

struct Strategy {
    int id;
    int num_indicies_in_strategy;
    int strategy_score;

    Strategy (int id, int num_indicies_in_strategy) : id (id), num_indicies_in_strategy (num_indicies_in_strategy), strategy_score (0) {}
};

struct AlphaAgent : public Agent {
    std::vector<Strategy> strategies;
    std::vector<signum> last_n_actions;
    int total_wins;
    int id;

    signum predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history) const;
    signum sin_predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum> &history) const;
    signum sin_predict2 (int strategy_index, int num_indicies_in_strategy, int history_index) const;
    signum sin_predict3 (int strategy_index, int num_indicies_in_strategy, int history_index) const;
    signum high_resolution_predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history) const;
    void update (const std::vector<signum>& history, const int& market_prediction);
    void weighted_update (const std::vector<signum>& history, const int& last_market_value);
    //Evolutionary Update Conditions
    int streak(const int& streak_length);
    int percentage_pass(const int& streak_length, const double& threshold);
    //Updates
    void agent_memory_boost(int test_result);
    void agent_add_strategy(int test_result);

    AlphaAgent (std::vector<Strategy> s, std::vector<signum> a, int w, int i) :
            strategies {s}, last_n_actions {a}, total_wins {w}, id {i} { };

    AlphaAgent (int id, int num_strategies, int num_indicies_in_strategy) :
            total_wins {0}, id {id} {
        for (int i = 0; i < num_strategies; ++i) {
            strategies.push_back (Strategy {id * num_strategies + i, num_indicies_in_strategy});
        }
    }

    virtual signum get_prediction(const MarketHistory &history) override {
        auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();

        //printf ("best strategy %li \n", index_of_best_strategy);
        auto prediction = sin_predict3 (index_of_best_strategy, strategies[index_of_best_strategy].num_indicies_in_strategy,
                                        history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
        last_n_actions.push_back (prediction);
        return prediction;
    }

    virtual void update(const MarketHistory &history, signum binary_market_result) override {
        for (int i = 0; i < strategies.size(); ++i) {
            if (sin_predict3 (i, strategies[i].num_indicies_in_strategy, history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
                ++strategies[i].strategy_score;
                ++total_wins;
            }
            else {
                --strategies[i].strategy_score;
            }
        }
    }

    virtual void print() override {
        printf ("Agent %i total_wins %i\n", id, total_wins );
        for (auto& s : strategies) {
            printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
        }
        printf ("\n");
    }
};

class Creationism : public EvolutionStrategy {
public:
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        return history.last_day().agents();
    }
};

// Old Experiment
struct Experiment {
    int evolutionary_history_length;
    int agent_count;
    int strategies_per_agent;
    int num_indicies_in_strategy;
    std::vector<signum> history;
    std::vector<int> nonbinary_history;
    std::vector<AlphaAgent> agents;

    //Agent Memory Length Distributions
    std::vector<AlphaAgent> initialize_agents(int agents_identifier);
    std::vector<AlphaAgent> linear_memory_dist_agent_init(int agent_identifier);
    std::vector<AlphaAgent> exponential_memory_dist_agent_init(double base, double exp_factor, int agents_identifier);
    std::vector<AlphaAgent> weighted_rnd_memory_dist_agent_init(double weight, int seed, int agents_identifier); //weighting in range (-1,1)
    //std::vector<Agent> bell_curve_memory_dist_agent_init(double kurtosis, int agents_identifier);

    Experiment(int, int, int, int, int, int);
    int market_evaluation();
    void run_minority_game(int number_of_runs);
    void one_minority_game_run(int market_count, int i);
    void write_minority_game_observables(int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS,
                                         int NUM_DIFF_MEMORY_LENGTHS, int NUM_STRATEGIES_PER_AGENT,
                                         int EVOLUTIONARY_MEMORY, int NUM_DIFF_STRATEGY_SETS);
    void write_attendance_history();
    void write_memory_distribution();

    //Evolutionary Updates
    void del_agent(int agent_index);
    void add_agent(int num_strategies_for_agent, int num_indicies_per_strategy);
    
    //Evolutionary Analysis
    void print_memory_distribution();
};

