#pragma once
#include "Evolutionary_Minority_Game.h"

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

    AlphaAgent (std::vector<Strategy> s, int w, int i) :
            strategies {s}, total_wins {w}, id {i} { };

    AlphaAgent (int id, int num_strategies, int num_indicies_in_strategy) :
            total_wins {0}, id {id} {
        for (int i = 0; i < num_strategies; ++i) {
            strategies.push_back (Strategy {(id * num_strategies) + i, num_indicies_in_strategy});
        }
    }

    signum sin_predict(const int strategy_index, int history_index) const {
        //deterministically returns the strategy's value associated with the market history and strategy selection from the index
        double sin_seed = sin((double) ( strategies[strategy_index].id * (history_index + 1)));
        //plus one to prevent 0 case problems
        auto result = ((int)(sin_seed*10000000.0) & 1) ? 1 : -1;
        //printf ("sin_predict2 %i %i %i %i\n", strategy_index, num_indicies_in_strategy, history_index + 1, result);
        return result;
    }

    void agent_memory_boost(int test_result){
        if(test_result == 1){
            //does this work?
            //for_each(strategies.begin(), strategies.end(), [](int strategy_score, int num_indicies_in_strategy){strategy_score++; num_indicies_in_strategy++;});
            for(int i = 0; i < strategies.size(); i++){
                strategies[i].num_indicies_in_strategy++; //upping the memory
                strategies[i].strategy_score = 0; //reseting strategy score
            }
        }
    }

    void agent_add_strategy(int test_result, int num_indicies_in_new_strategy) {
        if(test_result == 1) { strategies.push_back(Strategy{0, num_indicies_in_new_strategy}); }
    }

    virtual signum get_prediction(const MarketHistory &history) override {
        auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
        //printf ("best strategy %li \n", index_of_best_strategy);
        auto prediction = sin_predict (index_of_best_strategy, (int) history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
        return prediction;
    }

    virtual void weighted_update(const MarketHistory &history, signum binary_market_result) {
        for (int i = 0; i < strategies.size(); ++i) {
            if (sin_predict (i, (int) history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
                if(i == std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin()){
                    ++total_wins;
                }
                strategies[i].strategy_score += history.market_count();
            }
            else {
                strategies[i].strategy_score -= history.market_count();
            }
        }
    }

    virtual void update(const MarketHistory &history, signum binary_market_result) override {
        for (int i = 0; i < strategies.size(); ++i) {
            if (sin_predict (i, (int) history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
                ++strategies[i].strategy_score;
                if(i == std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin()){
                    ++total_wins;
                }
            }
            else {
                --strategies[i].strategy_score;
            }
        }
    }

    virtual void print() override {
        printf ("Agent %i total_wins %i\n", id, total_wins);
        for (auto& s : strategies) {
            printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
        }
        printf ("\n");
    }
};

class RandomAgent : public Agent {
    int seed;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
public:
    RandomAgent (int seed) : seed {seed}, generator {(unsigned int)seed}, dist {0, 1} {}

    virtual signum get_prediction(const MarketHistory &history) {
        return dist(generator) ? 1 : -1;
    }

    virtual void update(const MarketHistory &history, signum market_result) {};
    virtual void print() {
        printf ("RandomAgent seed %i\n", seed);
    }
};

// **********************************************************************
//                           Evolution Methodologies
// **********************************************************************

class Creationism : public EvolutionStrategy {
public:
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        return history.last_day().agents();
    }
}; //Simply returns the previous state

class BasicEvolution : public EvolutionStrategy {
    int last_used_agent_id;
public:
    BasicEvolution (int initial_agent_population) : last_used_agent_id (initial_agent_population - 1) {}

    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        std::vector<Agent*> next_gen = history.last_day().agents();

        next_gen.pop_back();

        next_gen.insert (next_gen.begin(), create_agent (agent_pool, new RandomAgent {last_used_agent_id + 1}));
        ++last_used_agent_id;

        return std::move (next_gen);
    }
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