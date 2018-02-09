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

    virtual void agent_memory_boost(){};
    virtual void agent_add_strategy(int num_indicies_in_new_strategy){};
    virtual double win_percentage_of_streak(){};
};

struct DarwinianAgent : public Agent{
    std::vector<Strategy> strategies;
    std::vector<signum> last_n_actions;
    int evolution_period;
    int total_wins;
    int id;

    DarwinianAgent (std::vector<Strategy>& strategies, int evol_period, int tot_wins, int i) :
    strategies {std::move(strategies)}, evolution_period {evol_period}, total_wins {tot_wins}, id {i} {};

    DarwinianAgent (int id, int evol_period, int num_strategies, int num_indicies_in_strategy) :
            total_wins {0}, id {id}, evolution_period {evol_period} {
        for (int i = 0; i < num_strategies; ++i) {
            strategies.emplace_back(Strategy {(id * num_strategies) + i, num_indicies_in_strategy});
        }
    }

    signum sin_predict(const int strategy_index, int history_index) const {
        //deterministically returns the strategy's value associated with the market history and strategy selection from the index
        double sin_seed = sin((double) ( strategies[strategy_index].id * (history_index + 1)));
        //plus one to prevent 0 case problems
        auto result = ((int)(sin_seed*10000000.0) & 1) ? 1 : -1;
        return result;
    }

    virtual signum get_prediction(const MarketHistory &history) override {
        auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
        //printf ("best strategy %li \n", index_of_best_strategy);
        auto prediction = sin_predict (index_of_best_strategy, (int) history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
        return prediction;
    }

    virtual void update(const MarketHistory &history, signum binary_market_result) override {
        for (int i = 0; i < strategies.size(); ++i) {
            if (sin_predict (i, (int) history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
                ++strategies[i].strategy_score;
                if(i == std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin()){
                    ++total_wins;
                    if(last_n_actions.size() < evolution_period){last_n_actions.emplace_back(1);
                    }else{
                        last_n_actions.pop_back();
                        last_n_actions.insert(last_n_actions.begin(), 1);
                    }
                }
            }
            else {
                --strategies[i].strategy_score;
                if(last_n_actions.size() < evolution_period){last_n_actions.emplace_back(0);
                }else{
                    last_n_actions.pop_back();
                    last_n_actions.insert(last_n_actions.begin(), 0);
                }
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

    virtual void agent_memory_boost(){
        //does this work? -- will the strategy score update without adding
        //for_each(strategies.begin(), strategies.end(), [](int strategy_score, int num_indicies_in_strategy){strategy_score++; num_indicies_in_strategy++;});
        for(auto e : strategies){
            e.num_indicies_in_strategy++; //upping the memory
            e.strategy_score = 0; //reseting strategy score
        }
    }

    virtual void agent_add_strategy(int num_indicies_in_new_strategy) { strategies.emplace_back(Strategy{0, num_indicies_in_new_strategy}); }

    virtual void weighted_update(const MarketHistory &history, signum binary_market_result) {
        for (int i = 0; i < strategies.size(); ++i) {
            if (sin_predict (i, (int) history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
                if(i == std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin()){
                    ++total_wins;
                    last_n_actions.emplace_back(1);
                }
                strategies[i].strategy_score += history.market_count();
                last_n_actions.emplace_back(0);
            }
            else {
                strategies[i].strategy_score -= history.market_count();
                last_n_actions.emplace_back(0);
            }
        }
    }

    virtual double win_percentage_of_streak() { return ((double)accumulate(last_n_actions.cbegin(), last_n_actions.cend(), 0) / (double)last_n_actions.size()); }

};

class RandomAgent : public Agent {
    int seed;
    int rng_resolution;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
    //int num_indicies_in_strategy;
public:
    RandomAgent (int seed, int rng_resolution) : seed {seed}, rng_resolution{rng_resolution}, generator {(unsigned int)seed}, dist {0, rng_resolution} {}

    virtual signum get_prediction(const MarketHistory &history) { return dist(generator) < (rng_resolution/2) ? -1 : 1; }

    virtual void print() {printf ("RandomAgent seed %i\n", seed); }

    /*
    RandomAgent (int seed, int num_indicies_in_strategy) : seed {seed}, num_indicies_in_strategy {num_indicies_in_strategy} {}
    virtual signum get_prediction(const MarketHistory &history) {
        return ((int)(sin((seed*history.last_n_results_as_bits(num_indicies_in_strategy)))*1000000.0) & 1) ? 1 : -1;
    }
    virtual void print() {printf ("RandomAgent seed %i, num_indicies_in strategy = %i\n", seed, num_indicies_in_strategy); }
    */

    //the following are not used; just but it in because it's the superclass.
    virtual void agent_memory_boost(){};
    virtual void agent_add_strategy(int num_indicies_in_new_strategy){};
    virtual double win_percentage_of_streak(){};
    virtual void weighted_update(const MarketHistory &history, signum binary_market_result){};
    virtual void update(const MarketHistory &history, signum market_result) {};
};

// **********************************************************************
//                           Evolution Methodologies
// **********************************************************************

class Creationism : public EvolutionStrategy {
public:
    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        return history.last_day().agents();
    }
    void evolutionary_update(){};
}; //Simply returns the previous state

class Darwinism: public EvolutionStrategy {
    double threshold = 0.9;
public:
    Darwinism (double win_threshold_percentage) : threshold (win_threshold_percentage) {};

    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        std::vector<Agent*> next_gen = evolutionary_update(history, agent_pool, threshold);
        return std::move (next_gen);
    }

    virtual std::vector<Agent*> evolutionary_update(const MarketHistory& history, AgentPool& agent_pool, double threshold) {
        assert(threshold <= 1 && threshold >= 0);
        std::vector<Agent*> next_gen;
        for (auto& agent : agent_pool){
            if(agent->win_percentage_of_streak() > threshold) {
                agent->agent_memory_boost();
                next_gen.push_back (agent.get());
            }
        }
    return std::move(next_gen);
    }

}; //Simply returns the previous state

class CyclicEvolution : public EvolutionStrategy {
    int last_used_agent_id;
public:
    CyclicEvolution (int initial_agent_population) : last_used_agent_id (initial_agent_population - 1) {}

    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        std::vector<Agent*> next_gen = history.last_day().agents();

        next_gen.pop_back();

        next_gen.insert (next_gen.begin(), create_agent (agent_pool, new RandomAgent {last_used_agent_id + 1, 10000}));
        ++last_used_agent_id;

        return std::move (next_gen);
    }
    void evolutionary_update(){};
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