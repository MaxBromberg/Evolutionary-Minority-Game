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
    virtual signum get_thermal_prediction(const MarketHistory &history) override;

    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);
    virtual void weighted_thermal_update(const MarketHistory &history, signum binary_market_result);

    virtual void update(const MarketHistory &history, signum binary_market_result) override;
    virtual void thermal_update(const MarketHistory &history, signum binary_market_result) override;

    virtual void print() override;

    //defined to be in compliance with the overall strategy
    virtual int return_num_strategies();
    virtual int return_memory(int strategy_index);
    virtual void agent_memory_boost(int max_memory);
    virtual void agent_memory_deduction(int min_memory);
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual void agent_subtract_strategy(int strategy_index);
    virtual double win_percentage_of_streak();
    virtual int return_evolutionary_period();
};

struct DarwinianAgent : public Agent{
    std::vector<Strategy> strategies;
    std::vector<signum> last_n_actions;
    int evolution_period;
    int total_wins;
    int id;

    DarwinianAgent (std::vector<Strategy> strategies, int tot_wins, int i, int evolutionaty_length);
    DarwinianAgent (int id, int num_strategies, int num_indicies_in_strategy, int evolutionaty_length);
    signum sin_predict(const int strategy_index, int history_index) const;
    virtual signum get_prediction(const MarketHistory &history) override;
    virtual signum get_thermal_prediction(const MarketHistory &history) override;
    virtual void update(const MarketHistory &history, signum binary_market_result) override;
    virtual void thermal_update(const MarketHistory &history, signum binary_market_result) override;
    virtual void print() override;

    virtual int return_num_strategies();
    virtual int return_memory(int strategy_index);
    virtual void agent_memory_boost(int max_memory);
    virtual void agent_memory_deduction(int min_memory);
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual void agent_subtract_strategy(int strategy_index);
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);
    virtual void weighted_thermal_update(const MarketHistory &history, signum binary_market_result);
    virtual double win_percentage_of_streak();
    virtual int return_evolutionary_period();
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
    virtual signum get_thermal_prediction(const MarketHistory &history);
    virtual void update(const MarketHistory &history, signum market_result);
    virtual void thermal_update(const MarketHistory &history, signum market_result);
    virtual int return_num_strategies();
    virtual int return_memory(int strategy_index);
    virtual double win_percentage_of_streak();
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result);
    virtual void weighted_thermal_update(const MarketHistory &history, signum binary_market_result);
    virtual void agent_memory_boost(int max_memory);
    virtual void agent_memory_deduction(int min_memory);
    virtual void agent_add_strategy(int num_indicies_in_new_strategy);
    virtual void agent_subtract_strategy(int strategy_index);
    virtual int return_evolutionary_period();
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
    double memory_delta;
    double strategy_delta;
    double breeding_delta;
    int max_memory;
    int min_memory;
    int max_strategies;
    int min_strategies;
    int max_pop;
    int min_pop;
public:
    Darwinism (double memory_delta, double strategy_delta, double breeding_delta, int max_mem, int min_mem, int max_stratgies, int min_strategies, int max_pop, int min_pop);
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> memory_update(const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> strategy_update(const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> memory_and_strategy_update(const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> population_update(const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> memory_and_strategy_and_pop_update(const MarketHistory& history, AgentPool& agent_pool);
    virtual std::vector<Agent*> memory_and_population_update(const MarketHistory& history, AgentPool& agent_pool);

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

//----------Initialization Mutator Methods for different Memory distributions-----------
vector<Strategy> linear_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_increment);
vector<Strategy> exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment, double alpha);
vector<Strategy> weighted_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, double weight);
vector<Strategy> stocastic_exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, double Lambda);
vector<Strategy> stocastic_poisson_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory);
vector<Strategy> stocastic_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory);

//*****************************Initializations******************************************

//_____________________Alpha_Agent_Initializations________________________________
AgentPool alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor);
AgentPool linear_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory);
AgentPool exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha);
AgentPool weighted_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha);
AgentPool stochastic_exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double lambda);
AgentPool stocastic_poisson_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory);
AgentPool stochastic_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory);
//_____________________Darwinian_Agent_Initializations____________________________
AgentPool darwinian_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int evolutionary_length);
AgentPool linear_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evolutionary_length);
AgentPool exponential_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha, int evolutionary_length);
AgentPool weighted_random_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha, int evolutionary_length);
AgentPool stochastic_exponential_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double lambda, int evolutionary_length);
AgentPool stocastic_poisson_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evolutionary_length);
AgentPool stochastic_random_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evolutionary_length);

//_____________________Other_Agent_Initializations________________________________
AgentPool random_agents(int agent_pop, int memory);

