#include "Agents and Evolution Strategies.h"

//Strategy Struct has only declaration and constructor

// ***************************************************************************
//                               Agents Methods
// ***************************************************************************

//----------Initialization Mutator Methods for different Memory distributions-----------

vector<Strategy> linear_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           (min_memory + ((int)floor((i*agent_increment*(max_memory-min_memory)/(agent_pop*num_strategies))+0.5)))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}

vector<Strategy> exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment, double alpha){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    double exp_increment = (log(max_memory/min_memory)/(alpha*num_strategies*agent_pop));
    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           (min_memory + ((int)floor(exp(alpha*i*agent_increment*exp_increment))))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}//Allots strategies in exponential distribution

vector<Strategy> poisson_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment, double alpha){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    double exp_increment = (log(max_memory/min_memory)/(alpha*num_strategies*agent_pop));
    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           (min_memory + ((int)floor(exp(alpha*i*agent_increment*exp_increment))))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}//Allots strategies in exponential distribution

vector<Strategy> weighted_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, int agent_pop, int agent_increment, double weight){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back (Strategy {(id * num_strategies) + i,
                                           random_generate(weight, min_memory, max_memory, ((id * num_strategies) + i))});
    }//Agent increment is the agent index, counting up from 1
    return strategies;
}//Allots strategies in exponential distribution

vector<Strategy> stocastic_exponential_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory, double Lambda){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    std::default_random_engine generator;
    std::exponential_distribution<double> distribution(Lambda);

    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        double exp_distributed_number = distribution(generator);
        strategies.emplace_back (Strategy {(id*num_strategies) + i,
                                           (int)floor(min_memory + ((max_memory-min_memory)*exp_distributed_number)) } );
        }
    return strategies;
}//This will stochastically allot strategies along an exponential dist, but not confine agents to particular range of memory length

vector<Strategy> stocastic_poisson_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    std::default_random_engine generator;
    std::poisson_distribution<int> distribution((max_memory - min_memory)/2);

    vector<Strategy> strategies;
    for (int i = 0; i < num_strategies; ++i) {
        double poisson_distributed_number = distribution(generator);
        while (poisson_distributed_number < min_memory || poisson_distributed_number > max_memory) { poisson_distributed_number = distribution(generator); }
            strategies.emplace_back(Strategy {(id * num_strategies) + i,
                                              (int) floor(min_memory +
                                                          ((max_memory - min_memory) * poisson_distributed_number))});
        }
    return strategies;
}//This will stochastically allot strategies along a poisson dist, but not confine agents to particular range of memory length

vector<Strategy> stocastic_random_mem_dist_init(int id, int num_strategies, int max_memory, int min_memory){
    assert(max_memory < 32);
    assert(min_memory <= min_memory);
    std::default_random_engine generator;
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

void AlphaAgent::print() {
    printf ("Agent %i total_wins %i\n", id, total_wins);
    for (auto& s : strategies) {
        printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
    }
    printf ("\n");
}

void AlphaAgent::agent_memory_boost(){};
void AlphaAgent::agent_add_strategy(int num_indicies_in_new_strategy){};
double AlphaAgent::win_percentage_of_streak(){};

//*****************************Darwinian Agent*************************************
DarwinianAgent::DarwinianAgent (std::vector<Strategy>& strategies, int evol_period, int tot_wins, int i) :
        strategies {std::move(strategies)}, evolution_period {evol_period}, total_wins {tot_wins}, id {i} {};

DarwinianAgent::DarwinianAgent (int id, int evol_period, int num_strategies, int num_indicies_in_strategy) :
        total_wins {0}, id {id}, evolution_period {evol_period} {
    for (int i = 0; i < num_strategies; ++i) {
        strategies.emplace_back(Strategy {(id * num_strategies) + i, num_indicies_in_strategy});
    }
}

signum DarwinianAgent::sin_predict(const int strategy_index, int history_index) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    double sin_seed = sin((double) ( strategies[strategy_index].id * (history_index + 1)));
    //plus one to prevent 0 case problems
    auto result = ((int)(sin_seed*10000000.0) & 1) ? 1 : -1;
    return result;
}

signum DarwinianAgent::get_prediction(const MarketHistory &history) {
    auto index_of_best_strategy = std::max_element (strategies.begin(), strategies.end(), [] (const Strategy& lhs, const Strategy& rhs) {return lhs.strategy_score < rhs.strategy_score;}) - strategies.begin();
    //printf ("best strategy %li \n", index_of_best_strategy);
    auto prediction = sin_predict (index_of_best_strategy, (int) history.last_n_results_as_bits(strategies[index_of_best_strategy].num_indicies_in_strategy));
    return prediction;
}

void DarwinianAgent::update(const MarketHistory &history, signum binary_market_result) {
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

void DarwinianAgent::print() {
    printf ("Agent %i total_wins %i\n", id, total_wins);
    for (auto& s : strategies) {
        printf ("\tStrategy %i : score %i\n", s.id, s.strategy_score);
    }
    printf ("\n");
}

void DarwinianAgent::agent_memory_boost(){
    //does this work? -- will the strategy score update without adding
    //for_each(strategies.begin(), strategies.end(), [](int strategy_score, int num_indicies_in_strategy){strategy_score++; num_indicies_in_strategy++;});
    for(auto e : strategies){
        e.num_indicies_in_strategy++; //upping the memory
        e.strategy_score = 0; //reseting strategy score
    }
}

void DarwinianAgent::agent_add_strategy(int num_indicies_in_new_strategy) { strategies.emplace_back(Strategy{0, num_indicies_in_new_strategy}); }

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

double DarwinianAgent::win_percentage_of_streak() { return ((double)accumulate(last_n_actions.cbegin(), last_n_actions.cend(), 0) / (double)last_n_actions.size()); }

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

//the following are not used; just but it in because it's the superclass.
void RandomAgent::agent_memory_boost(){};
void RandomAgent::agent_add_strategy(int num_indicies_in_new_strategy){};
double RandomAgent::win_percentage_of_streak(){};
void RandomAgent::weighted_update(const MarketHistory &history, signum binary_market_result){};
void RandomAgent::update(const MarketHistory &history, signum market_result) {};

// **********************************************************************
//                           Evolution Methodologies
// **********************************************************************

//*****************************Creationist Evolutionary Methodology*************************************
//**********************************(i.e. No change occurs)*********************************************

std::vector<Agent*> Creationism::select_next_generation (const MarketHistory& history, AgentPool& agent_pool) { return history.last_day().agents(); }
void Creationism::evolutionary_update(){};

//*****************************Darwinistic Evolutionary Methodology*************************************
//*************************(i.e. Modification of memory and agent pool)*********************************

Darwinism::Darwinism (double win_threshold_percentage) : threshold (win_threshold_percentage) {};
std::vector<Agent*> Darwinism::select_next_generation (const MarketHistory& history, AgentPool& agent_pool) {
    std::vector<Agent*> next_gen = evolutionary_update(history, agent_pool, threshold);
    return std::move (next_gen);
}
std::vector<Agent*> Darwinism::evolutionary_update(const MarketHistory& history, AgentPool& agent_pool, double threshold) {
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

