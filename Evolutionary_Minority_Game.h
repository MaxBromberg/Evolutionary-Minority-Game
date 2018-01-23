#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <bitset>


typedef int signum; //To hold the +/-1s, and indicate return type. (binary history --> history)
//The Old
std::vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);
std::vector<signum> binaryMarketHistoryGenerator(int NumIndicesInStrategy, int seed);
std::vector<int> marketHistoryGenerator(const std::vector<int>& source, int agentPopulation, int seed);
int random_generate(double weight, int range, int seed);

template <typename Iter>
int BinaryArrayToStrategyIndex (Iter begin, Iter end) {
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
    std::vector<signum> last_n_actions;
    int total_wins;
    int relatively_unique_identifer;

    signum predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history) const;
    signum sin_predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum> &history) const;
    signum high_resolution_predict(int strategy_index, int num_indicies_in_strategy, const std::vector<signum>& history) const;
    void update (const std::vector<signum>& history, const int& market_prediction);
    void weighted_update (const std::vector<signum>& history, const int& last_market_value);
    //Evolutionary Update Conditions
    int streak(const int& streak_length);
    int percentage_pass(const int& streak_length, const double& threshold);
    //Updates
    void agent_memory_boost(int test_result);
    void agent_add_strategy(int test_result);
};

struct Experiment {
    int evolutionary_history_length;
    int agent_count;
    int strategies_per_agent;
    int num_indicies_in_strategy;
    std::vector<signum> history;
    std::vector<int> nonbinary_history;
    std::vector<Agent> agents;

    //Agent Memory Length Distributions
    std::vector<Agent> initialize_agents(int agents_identifier);
    std::vector<Agent> linear_memory_dist_agent_init(int agent_identifier);
    std::vector<Agent> exponential_memory_dist_agent_init(double base, double exp_factor, int agents_identifier);
    std::vector<Agent> weighted_rnd_memory_dist_agent_init(double weight, int seed, int agents_identifier); //weighting in range (-1,1)
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

