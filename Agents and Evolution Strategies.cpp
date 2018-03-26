#include "Agents and Evolution Strategies.h"

//Strategy Struct has only declaration and constructor

// ***************************************************************************
//                               Agents Methods
// ***************************************************************************

//*****************************Alpha Agent*************************************

AlphaAgent::AlphaAgent (std::vector<Strategy> s, int w, int i) :
        strategies {move(s)}, total_wins {w}, id {i} { };

AlphaAgent::AlphaAgent (int id, int num_strategies, int num_indicies_in_strategy) :
        total_wins {0}, id {id} {
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i, num_indicies_in_strategy});
    }
}

signum AlphaAgent::sin_predict(const int strategy_index, int history_index) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    double sin_seed = sin((double) ( strategies[strategy_index].id * (history_index + 1)));
    //plus one to prevent 0 case problems
    auto result = ((int)(sin_seed*10000000.0) & 1) ? 1 : -1;
    //printf ("sin_predict2 %i %i %i %i\n", strategy_index, num_indicies_in_strategy, history_index + 1, result);
    return result;
}

signum AlphaAgent::get_prediction(const MarketHistory &history) {
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    //printf ("best strategy %li \n", index_of_best_strategy);
    auto prediction = sin_predict (index_of_best_strategy, (int) history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
    return prediction;
}
signum AlphaAgent::get_thermal_prediction(const MarketHistory &history) {
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    auto random_history_index = (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[index_of_best_strategy].num_indicies_in_strategy))));
    //printf ("best strategy %li \n", index_of_best_strategy);
    auto prediction = sin_predict (index_of_best_strategy, random_history_index);
    //Hopefully this all is just compiled to the one return line. Everything is kept separate for conceptual clarity
    return prediction;
}

void AlphaAgent::weighted_update(const MarketHistory &history, signum binary_market_result) {
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
void AlphaAgent::weighted_thermal_update(const MarketHistory &history, signum binary_market_result) {
    for (int i = 0; i < strategies.size(); ++i) {
        if (sin_predict (i, (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[i].num_indicies_in_strategy))))) == binary_market_result) {
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

void AlphaAgent::update(const MarketHistory &history, signum binary_market_result) {
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
void AlphaAgent::thermal_update(const MarketHistory &history, signum binary_market_result) {
    for (int i = 0; i < strategies.size(); ++i) {
        if (sin_predict (i, (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[i].num_indicies_in_strategy))))) == binary_market_result) {
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

void AlphaAgent::print() {
    printf ("Agent %i total_wins %i\n", id, total_wins);
    for (auto& s : strategies) {
        printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
    }
    printf ("\n");
}

int AlphaAgent::return_num_strategies(){return strategies.size(); }
int AlphaAgent::return_memory(int strategy_index) { return strategies[strategy_index].num_indicies_in_strategy; }
void AlphaAgent::agent_memory_boost(int max_memory){};
void AlphaAgent::agent_memory_deduction(int min_memory) {};
void AlphaAgent::agent_add_strategy(int num_indicies_in_new_strategy){};
void AlphaAgent::agent_subtract_strategy(int strategy_index) {};
double AlphaAgent::win_percentage_of_streak() {};
int AlphaAgent::return_evolutionary_period() {};



//--------------------Alpha Agent Memory Distribution Initializer-----------------------
AgentPool alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{i*strategy_set_incrementor, num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
} //initializes agent pool with alpha agents

AgentPool linear_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{linear_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, i), 0, i}});
    }
    return std::move (agents);
}

AgentPool exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, agent_population, i, alpha), 0, i}});
    }
    return std::move (agents);
}

AgentPool weighted_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{weighted_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, alpha), 0, i}});
    }
    return std::move (agents);
}

AgentPool stochastic_exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double lambda) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, lambda), 0, i}});
    }
    return std::move (agents);
}

AgentPool stocastic_poisson_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_poisson_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), 0, i}});
    }
    return std::move (agents);
}

AgentPool stochastic_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), 0, i}});
    }
    return std::move (agents);
}
//--------------------------------------------------------------------------------------//


//*****************************Darwinian Agent*************************************
DarwinianAgent::DarwinianAgent (std::vector<Strategy> strategies, int tot_wins, int id, int evolutionary_length) :
        strategies {std::move(strategies)}, total_wins {tot_wins}, id {id}, evolution_period{evolutionary_length} {};

DarwinianAgent::DarwinianAgent (int id, int num_strategies, int num_indicies_in_strategy, int evolutionaty_length) :
        total_wins {0}, id {id}, evolution_period{evolutionaty_length}{
    vector<signum> temp;
    last_n_actions = temp;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back(Strategy {(id * num_strategies) + i, num_indicies_in_strategy});
    }
}

signum DarwinianAgent::sin_predict(const int strategy_index, int history_index) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    double sin_seed = sin((double) ( strategies[strategy_index].id * (history_index + 1)));
    //plus one to history index to prevent 0 case problems
    auto result = ((int)(sin_seed*10000000.0) & 1) ? 1 : -1;
    return result;
}

signum DarwinianAgent::get_prediction(const MarketHistory &history) {
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    //printf ("best strategy %li \n", index_of_best_strategy);
    auto prediction = sin_predict (index_of_best_strategy, (int) history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
    return prediction;
}
signum DarwinianAgent::get_thermal_prediction(const MarketHistory &history) {
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    auto random_history_index = (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[index_of_best_strategy].num_indicies_in_strategy))));
    auto prediction = sin_predict (index_of_best_strategy, random_history_index);
    //Hopefully this all is just compiled to the one return line. Everything is kept separate for conceptual clarity
    return prediction;
}

void DarwinianAgent::update(const MarketHistory &history, signum binary_market_result) {
    for (int i = 0; i < strategies.size(); ++i) {
        if (sin_predict (i, (int) history.last_n_results_as_bits (strategies[i].num_indicies_in_strategy)) == binary_market_result) {
            ++strategies[i].strategy_score;
            if(i == std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin()){
                ++total_wins;
                if( last_n_actions.size() < evolution_period){
                    last_n_actions.emplace_back(1);
                }else{
                    last_n_actions.pop_back();
                    last_n_actions.insert(last_n_actions.begin(), 1);
                }
            }else{
                if (last_n_actions.size() < evolution_period) {
                    last_n_actions.emplace_back(0);
                } else {
                    last_n_actions.pop_back();
                    last_n_actions.insert(last_n_actions.begin(), 0);
                }
            }
        }
        else {
            --strategies[i].strategy_score;
        }
    }
    //debug_print(last_n_actions);
}
void DarwinianAgent::thermal_update(const MarketHistory &history, signum binary_market_result) {
    for (int i = 0; i < strategies.size(); ++i) {
        if (sin_predict (i, (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[i].num_indicies_in_strategy))))) == binary_market_result) {
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

void DarwinianAgent::weighted_update(const MarketHistory &history, signum binary_market_result) {
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
void DarwinianAgent::weighted_thermal_update(const MarketHistory &history, signum binary_market_result) {
    for (int i = 0; i < strategies.size(); ++i) {
        if (sin_predict (i, (int)(sin((double)history.index_of_current_day())*((unsigned int) pow(2,(strategies[i].num_indicies_in_strategy))))) == binary_market_result) {
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

void DarwinianAgent::print() {
    printf ("Agent %i total_wins %i\n", id, total_wins);
    for (auto& s : strategies) {
        printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
    }
    printf ("\n");
}

void DarwinianAgent::agent_memory_boost(int max_memory){

    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    if(strategies[index_of_best_strategy].num_indicies_in_strategy < max_memory) {
        strategies[index_of_best_strategy].num_indicies_in_strategy++;
        strategies[index_of_best_strategy].strategy_score = 0;
    }

/*
    for (int i = 0; i < strategies.size(); ++i) {
        if(strategies[i].num_indicies_in_strategy < max_memory) {
            strategies[i].num_indicies_in_strategy++; //upping the memory
            strategies[i].strategy_score = 0; //reseting strategy score
        }
    }
    */


}
void DarwinianAgent::agent_memory_deduction(int min_memory){
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    if(strategies[index_of_best_strategy].num_indicies_in_strategy < min_memory) {
        strategies[index_of_best_strategy].num_indicies_in_strategy--;
        strategies[index_of_best_strategy].strategy_score = 0;
    }

//    for (int i = 0; i < strategies.size(); ++i) {
//        if(strategies[i].num_indicies_in_strategy > min_memory) {
//            strategies[i].num_indicies_in_strategy--; //memory reduction
//            strategies[i].strategy_score = 0; //reseting strategy score
//        }
//    }

}

void DarwinianAgent::agent_add_strategy(int num_indicies_in_new_strategy) {
    if(num_indicies_in_new_strategy == -1){
        int max_element_mem_indicie = strategies[(std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin())].num_indicies_in_strategy;
        strategies.emplace_back(Strategy{0, max_element_mem_indicie});
    }else {
        strategies.emplace_back(Strategy{0, num_indicies_in_new_strategy});
    }
}
void DarwinianAgent::agent_subtract_strategy(int strategy_index) {
    if (strategy_index == -1) {
        int max_element_indicie = (std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin());
        strategies.erase(strategies.begin() + max_element_indicie);
    } else {
        strategies.erase(strategies.begin() + strategy_index);
//        printf("num_strategies = %i, strategy = %i \n",strategies.size(), strategy_index);
    }
}
double DarwinianAgent::win_percentage_of_streak() { return ((double)accumulate(last_n_actions.cbegin(), last_n_actions.cend(), 0) / (double)last_n_actions.size()); }
int DarwinianAgent::return_num_strategies(){return strategies.size(); }
int DarwinianAgent::return_memory(int strategy_index) { return strategies[strategy_index].num_indicies_in_strategy; }
int DarwinianAgent::return_evolutionary_period() { return evolution_period; }

//--------------------Darwinian Agent Memory Distribution Initializer-----------------------
AgentPool darwinian_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{i*strategy_set_incrementor, num_strategies_per_agent, num_indicies_in_strategy, evoltionary_period}});
    }
    return std::move (agents);
} //initializes agent pool with alpha agents

AgentPool linear_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{linear_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, i), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}

AgentPool exponential_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, agent_population, i, alpha), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}

AgentPool weighted_random_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double alpha, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{weighted_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, alpha), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}

AgentPool stochastic_exponential_mem_darwinian_agent(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, double lambda, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{stocastic_exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, lambda), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}

AgentPool stocastic_poisson_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{stocastic_poisson_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}

AgentPool stochastic_random_mem_darwinian_agents(int agent_population, int num_strategies_per_agent, int strategy_set_incrementor, int max_memory, int min_memory, int evoltionary_period) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new DarwinianAgent{stocastic_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), 0, i, evoltionary_period}});
    }
    return std::move (agents);
}
//--------------------------------------------------------------------------------------//

//*****************************Random Agent*************************************
 
RandomAgent::RandomAgent (int seed, int rng_resolution) : seed {seed}, rng_resolution{rng_resolution}, generator {(unsigned int)seed}, dist {0, rng_resolution} {}

signum RandomAgent::get_prediction(const MarketHistory &history) { return dist(generator) < (rng_resolution/2) ? -1 : 1; }

void RandomAgent::print() {printf ("RandomAgent seed %i\n", seed); }
/*
RandomAgent (int seed, int num_indicies_in_strategy) : seed {seed}, num_indicies_in_strategy {num_indicies_in_strategy} {}
virtual signum get_prediction(const MarketHistory &history) {
    return ((int)(sin((seed*history.last_n_results_as_bits(num_indicies_in_strategy)))*1000000.0) & 1) ? 1 : -1;
}
virtual void print() {printf ("RandomAgent seed %i, num_indicies_in strategy = %i\n", seed, num_indicies_in_strategy); }
*/

//the following are not used (or implemented); just put it in because it's in the superclass.
signum RandomAgent::get_thermal_prediction(const MarketHistory &history){};
void RandomAgent::update(const MarketHistory &history, signum market_result){};
void RandomAgent::thermal_update(const MarketHistory &history, signum market_result){};
int RandomAgent::return_num_strategies(){};
int RandomAgent::return_memory(int strategy_index){};
double RandomAgent::win_percentage_of_streak(){};
void RandomAgent::weighted_update(const MarketHistory &history, signum binary_market_result){};
void RandomAgent::weighted_thermal_update(const MarketHistory &history, signum binary_market_result){};
void RandomAgent::agent_memory_boost(int max_memory){};
void RandomAgent::agent_memory_deduction(int min_memory){};
void RandomAgent::agent_add_strategy(int num_indicies_in_new_strategy){};
void RandomAgent::agent_subtract_strategy(int strategy_index) {};
int RandomAgent::return_evolutionary_period() {};
/*
AgentPool random_agents(int agent_pop, int num_indicies_in_strategy) {
    AgentPool agents;
    for (int i = 0; i < agent_pop; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new RandomAgent{i, num_indicies_in_strategy}});
    }
    return std::move (agents);
}
*/ //Alternative random_agents fct

AgentPool random_agents(int agent_pop, int rng_resolution) {
    AgentPool agents;
    for (int i = 0; i < agent_pop; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new RandomAgent{i+123, rng_resolution}});
    }
    return std::move (agents);
}


// **********************************************************************
//                           Evolution Methodologies
// **********************************************************************

//*****************************Creationist Evolutionary Methodology*************************************
//**********************************(i.e. No change occurs)*********************************************

std::vector<Agent*> Creationism::select_next_generation (const MarketHistory& history, AgentPool& agent_pool) { return history.last_day().agents(); }
void Creationism::evolutionary_update(){};

//*****************************Darwinistic Evolutionary Methodology*************************************
//*************************(i.e. Modification of memory and agent pool)*********************************

Darwinism::Darwinism (const double memory_threshold, const double strategy_threshold, const double breeding_threshold, const int max_mem, const int min_mem, const int max_strategies, const int min_strategies, const int max_pop, const int min_pop)
        : memory_threshold {memory_threshold},  strategy_threshold {strategy_threshold}, population_threshold{breeding_threshold}, max_memory {max_mem}, min_memory {min_mem}, max_strategies {max_strategies}, min_strategies {min_strategies}, max_pop{max_pop}, min_pop{min_pop}{
    assert(memory_threshold <= 1 && memory_threshold >= 0);
    assert(strategy_threshold <= 1 && strategy_threshold >= 0);
    assert(breeding_threshold <=1 && strategy_threshold >= 0);
    EvolutionStrategy::memory_threshold = memory_threshold;
    EvolutionStrategy::strategy_threshold = strategy_threshold;
    EvolutionStrategy::population_threshold = population_threshold;
};


/////THE RETURN OF THIS FUNCTION MUST BE REPLACED WITH THE BELOW METHODS FOR EVOLUTIONARY IMPLEMENTATIONS/////
std::vector<Agent*> Darwinism::select_next_generation (const MarketHistory& history, AgentPool& agent_pool) {
    return population_update(history, agent_pool);
}

/*
std::vector<Agent*> Darwinism::memory_update(const MarketHistory& history, AgentPool& agent_pool) {
    std::vector<Agent*> next_gen = history.last_day().agents();
    for (auto& agent : next_gen){
        auto win_percentage = agent->win_percentage_of_streak();
        if(win_percentage > memory_threshold && win_percentage != 1) {
            agent->agent_memory_boost(max_memory);
            memory_threshold = win_percentage;
        }else if(agent->win_percentage_of_streak() < memory_threshold){
            agent->agent_memory_deduction(min_memory);
        }
        cout<<"Memory threshold = "<<memory_threshold<<endl;
        EvolutionStrategy::memory_threshold = memory_threshold;
    }
    return std::move(next_gen);
}
*/

//Methods for evolutionary thresholds evolving with time
//std::vector<Agent*> Darwinism::memory_update(const MarketHistory& history, AgentPool& agent_pool) {
//    std::vector<Agent*> next_gen = history.last_day().agents();
//    double success_rate = ((double)(next_gen.size() - history.market_count())/(double)(2*next_gen.size()));
//    if(memory_threshold < success_rate){
//        memory_threshold = success_rate;
//        EvolutionStrategy::memory_threshold = memory_threshold;
//    }
//    for (auto& agent : next_gen){
//        auto win_percentage = agent->win_percentage_of_streak();
//        if(win_percentage > memory_threshold && win_percentage != 1) {
//            agent->agent_memory_boost(max_memory);
//        }else{
//            agent->agent_memory_deduction(min_memory);
//        }
//    }
//    return std::move(next_gen);
//}

std::vector<Agent*> Darwinism::strategy_update(const MarketHistory& history, AgentPool& agent_pool) {
    std::vector<Agent*> next_gen = history.last_day().agents();
    double success_rate = ((double)(next_gen.size() - history.market_count())/(double)(2*next_gen.size()));
    if(strategy_threshold < success_rate && success_rate != 1){
        strategy_threshold = success_rate;
        EvolutionStrategy::strategy_threshold = strategy_threshold;
    }
    for (auto& agent : next_gen){
        auto win_percentage = agent->win_percentage_of_streak();
        if(win_percentage > strategy_threshold && win_percentage != 1) {
            if(agent->return_num_strategies() < max_strategies) {
                agent->agent_add_strategy(-1);
            }
        }else{
            if(agent->return_num_strategies() > min_strategies) {
                agent->agent_subtract_strategy(agent->return_num_strategies()-1);
            }
        }
    }
    return std::move(next_gen);
}


//__________________________________Evolutionary threshold through static range fcts____________________________________
//Methods for varying over evolutionary threshold range
std::vector<Agent*> Darwinism::memory_update(const MarketHistory& history, AgentPool& agent_pool) {
    std::vector<Agent*> next_gen = history.last_day().agents();
    for (auto& agent : next_gen){
        if(agent->win_percentage_of_streak() > memory_threshold) {
            agent->agent_memory_boost(max_memory);
        }else if(agent->win_percentage_of_streak() <= 1-memory_threshold){
            agent->agent_memory_deduction(min_memory);
        }
    }
    return std::move(next_gen);
}

//std::vector<Agent*> Darwinism::strategy_update(const MarketHistory& history, AgentPool& agent_pool) {
//    std::vector<Agent *> next_gen = history.last_day().agents();
//    for (auto &agent : next_gen) {
//        if (agent->win_percentage_of_streak() > strategy_threshold) {
//            if(agent->return_num_strategies() < max_strategies) {
////                agent->agent_add_strategy(agent->return_memory(agent->return_num_strategies() - 1));
//                //Adds a strategy with memory length = to the last strategy's (for static mem distributions)
//                agent->agent_add_strategy(-1); //creates strategy with the memory length of that agent's best performing strategy
//            }
//        } else if (agent->win_percentage_of_streak() <= strategy_threshold) {
//            if(agent->return_num_strategies() > min_strategies) {
//                agent->agent_subtract_strategy(agent->return_num_strategies()-1); //Just subtracts the last strategy.
////                auto strategy_to_erase = ((int)lround(abs(sin(history.index_of_current_day()*agent->win_percentage_of_streak()))*agent->return_num_strategies()))-1;
////                agent->agent_subtract_strategy(strategy_to_erase); //Subtracts a random(ish) strategy.
////                agent->agent_subtract_strategy(-1); //deletes best performing strategy
//            }
//        }
//    }
//    return std::move(next_gen);
//}
std::vector<Agent*> Darwinism::population_update(const MarketHistory& history, AgentPool& agent_pool){
    std::vector<Agent *> next_gen = history.last_day().agents();
    double success_rate = ((double)(next_gen.size() - history.market_count())/(double)(2*next_gen.size()));
    auto last_element = next_gen.size()-1;
    if(success_rate > population_threshold) {
//        printf("Before simulation agent pop= %i \n", next_gen.size());
        if(next_gen.size() < max_pop) {
            auto random_number = random_generate(0, 2, 16, 42);
            next_gen.insert(next_gen.begin(), create_agent(agent_pool, new DarwinianAgent
                    {(int) (last_element + next_gen.size()),
                     next_gen[next_gen.size()-1]->return_num_strategies(),
                     random_number,
                     next_gen[last_element]->return_evolutionary_period()}));
        } else if (success_rate <= population_threshold) {
            if(next_gen.size() > min_pop) {
//            delete_agent(agent_pool, i);
                next_gen.erase(next_gen.begin() + last_element);
            }
        }
//        population_threshold = success_rate; //Creates a dynamic evolutionary regime
//        EvolutionStrategy::population_threshold = population_threshold;
//        printf("After simulation agent pop= %i \n", next_gen.size());
    }
    return std::move(next_gen);
}

//std::vector<Agent*> Darwinism::population_update(const MarketHistory& history, AgentPool& agent_pool){
//    std::vector<Agent *> next_gen = history.last_day().agents();
//    double success_rate = ((double)(next_gen.size() - history.market_count())/(double)(2*next_gen.size()));
//    auto last_element = next_gen.size()-1;
//    if(success_rate > population_threshold) {
////        printf("Before simulation agent pop= %i \n", next_gen.size());
//        if(next_gen.size() < max_pop) {
//            auto random_number = random_generate(0, 0, next_gen.size(), 42);
//            next_gen.insert(next_gen.begin(), create_agent(agent_pool, new DarwinianAgent
//                    {(int) (last_element + next_gen.size()),
//                     next_gen[next_gen.size()-1]->return_num_strategies(),
//                     next_gen[last_element]->return_memory(next_gen[last_element]->return_num_strategies() - 1),
//                     next_gen[last_element]->return_evolutionary_period()}));
//        } else if (success_rate <= population_threshold) {
//            if(next_gen.size() > min_pop) {
////            delete_agent(agent_pool, i);
//                next_gen.erase(next_gen.begin() + last_element);
//            }
//        }
////        population_threshold = success_rate; //Creates a dynamic evolutionary regime
////        EvolutionStrategy::population_threshold = population_threshold;
////        printf("After simulation agent pop= %i \n", next_gen.size());
//    }
//    return std::move(next_gen);
//}

//creation is done without similarity, just same num. strategies and memory lengths

//*****************************Cyclic Evolutionary Methodology*************************************
//*****************(i.e. oldest agent discarded, new agent created each run)***********************

CyclicEvolution::CyclicEvolution (int initial_agent_population) : last_used_agent_id (initial_agent_population - 1) {}

std::vector<Agent*> CyclicEvolution::select_next_generation (const MarketHistory& history, AgentPool& agent_pool) {
    std::vector<Agent*> next_gen = history.last_day().agents();

    next_gen.pop_back();

    next_gen.insert (next_gen.begin(), create_agent (agent_pool, new RandomAgent {last_used_agent_id + 1, 10000}));
    ++last_used_agent_id;

    return std::move (next_gen);
}
void CyclicEvolution::evolutionary_update(){};

// *****************************************************************
//  Initializations
// *****************************************************************

//----------Initialization Mutator Methods for different Memory distributions-----------

vector<Strategy> linear_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_increment) {
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    vector<Strategy> strategies;
    for (int i = (num_strategies - 1); i >= 0; --i) {
        strategies.emplace_back(Strategy {(id * num_strategies) + i,
                                          min_memory + ((agent_increment + (((num_strategies - 1) * agent_increment) - i)) % (max_memory - min_memory))});
    }
    return strategies;
}

vector<Strategy> exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment, double alpha){
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    double exp_increment = (log(max_memory/min_memory))/(alpha*num_strategies*agent_pop);
    vector<Strategy> strategies;
    for (int i = (num_strategies-1); i >= 0; --i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           (int)(floor(min_memory + (exp(alpha*(agent_increment+(((num_strategies-1)*agent_increment) - i))*exp_increment))))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}//Allots strategies in exponential distribution

vector<Strategy> weighted_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, double weight){
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           random_generate(weight, min_memory, max_memory, ((id * num_strategies) + i))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}//Allots strategies in weighted random distribution

vector<Strategy> stocastic_exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, double Lambda){
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    std::mt19937 generator(id);
    std::exponential_distribution<double> distribution(Lambda);

    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        double exp_distributed_number = distribution(generator);
        while(exp_distributed_number > 1.0) { exp_distributed_number = distribution(generator); }
        strategies.emplace_back(Strategy {(id * num_strategies) + i,
                                          (int) floor(min_memory + ((max_memory - min_memory) * exp_distributed_number))});
    }
    return strategies;
}//This will stochastically allot strategies along an exponential dist, but not confine agents to particular range of memory length

vector<Strategy> stocastic_poisson_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory){
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    std::default_random_engine generator(id);
    std::poisson_distribution<int> distribution((max_memory - min_memory)/2);

    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        double poisson_distributed_number = distribution(generator);
        while (poisson_distributed_number < min_memory || poisson_distributed_number > max_memory) { poisson_distributed_number = distribution(generator); }
        strategies.emplace_back(Strategy {(id * num_strategies) + i,
                                          (int) floor(min_memory + poisson_distributed_number)});
                                        //(int) floor(min_memory + ((max_memory - min_memory) * poisson_distributed_number))});
    }
    return strategies;
}//This will stochastically allot strategies along a poisson dist, but not confine agents to particular range of memory length

vector<Strategy> stocastic_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory){
    assert(max_memory < 32);
    assert(min_memory <= max_memory);
    std::default_random_engine generator(id);
    std::uniform_int_distribution<int> distribution(min_memory, max_memory);

    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        double randomly_distributed_number = distribution(generator);
        strategies.emplace_back (Strategy {(id*num_strategies) + i, (int)randomly_distributed_number } );
    }
    return strategies;
}//This will stochastically allot strategies along an exponential dist, but not confine agents to particular range of memory length

/*
 * The above methods raise the question as to how the distribution of memory useage will work, when the overall distribution
 * of memory follows a particular distribution (as defined above, random, poisson or exponential) how will the agents
 * accommodate the evolutionarily proven to be best distribution of memory lengths.
 * I.e. How will the agents adaptation to a static initial distribution of memory lengths lead to a best fit
 * (perhaps measured by a total hamming distance from the optimal memory distribution given their best distribution)
 */


