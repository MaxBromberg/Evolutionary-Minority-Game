#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "Evolutionary_Minority_Game.h"
#include "Agents and Evolution Strategies.h"

using namespace std;

//Old code, just for reference
/*
vector<Agent> Experiment::bell_curve_memory_dist_agent_init(double kurtosis, int agents_identifier) {
    assert(num_indicies_in_strategy < 32);
    vector<Agent> Agents;

    double integral = 0; //numerically compute integral
    int should_be_agent_pop = 0;
    for(int i = 0; i < num_indicies_in_strategy; i++) {
        double exponent = -((((i - num_indicies_in_strategy / 2) / ((double) num_indicies_in_strategy / 2)) *
                             ((i - num_indicies_in_strategy / 2) / ((double) num_indicies_in_strategy / 2))) *
                            3.14159265 * kurtosis);
        double unitary_normal_dist_elemt = pow(2.71828182846, exponent); //element 0,1
        integral += (int) floor(((double) num_indicies_in_strategy) * unitary_normal_dist_elemt + 0.5);
    }


    for(int i = 0; i < num_indicies_in_strategy; i++) {
        double exponent = -((((i - num_indicies_in_strategy / 2) / ((double) num_indicies_in_strategy / 2)) *
                             ((i - num_indicies_in_strategy / 2) / ((double) num_indicies_in_strategy / 2))) * 3.14159265 * kurtosis);
        double unitary_normal_dist_elemt = pow(2.71828182846, exponent); //element 0,1
        double unrounded_memory_value = (num_indicies_in_strategy * unitary_normal_dist_elemt);
        auto memory_value = (int) floor(((double) num_indicies_in_strategy) * unitary_normal_dist_elemt + 0.5);

        for (int j = 0; j < (int) floor(((memory_value/integral)*agent_count)+0.5); j++) {
            vector<Strategy> Strategies;
            for(int k = 0; k < strategies_per_agent; k++){
                Strategies.push_back(Strategy{0, (int) floor((memory_value/2)+0.5) });
            }
            Agents.push_back(Agent{Strategies, i + agents_identifier});
        }
        should_be_agent_pop += (int) floor(((memory_value/integral)*agent_count)+0.5);

        cout << "(int) floor((memory_value/integral)*agent_count) = "<<(int) floor(((memory_value/integral)*agent_count)+0.5) <<endl;
        cout << "(int) floor((memory_value/2)+0.5) = "<<(int) floor((memory_value/2)+0.5) <<endl;
        cout << "exponent = " << exponent << endl;
        cout << "unitary_normal_dist_elemt = " << unitary_normal_dist_elemt << endl;
        cout << "unfloored Memory Value = " << unrounded_memory_value << endl;
        cout << "Memory Value = " << memory_value << endl;
        cout << endl;
    }
    cout<<"should_be_agent_pop = "<<should_be_agent_pop<<endl;
    cout<<"integral = "<<integral<<endl;
    return Agents;
} //kurtosis > 1 will increase kurtosis, < 1 decrease

void Experiment::write_attendance_history(){
    ofstream Attendance("Evolutionary Market History for 10000 runs.txt");
    for(int i = num_indicies_in_strategy; i < nonbinary_history.size(); i++) {
        Attendance << i << ", "
                   << nonbinary_history[i] << ", "
                   << history[i] << endl;
    }
}
*/

typedef int signum; //To hold the +/-// 1s, and indicate return type. (binary history --> history)
typedef std::vector<std::unique_ptr<Agent>> AgentPool;

// ***************************************************************************
//  Minority Game Engine
// ***************************************************************************
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


static inline void delete_agent (AgentPool& pool, int agent_index) { pool.erase (pool.begin() + agent_index); }

// Evolution Strategy Member Implementations
//**********************************

// MarketDay Member Implementations
//**********************************
MarketDay::MarketDay(int index, int market_prediction, signum result) :
        m_index {index}, m_market_prediction {market_prediction}, m_result {result}, m_agents{} { }

MarketDay::MarketDay (int index, std::vector<Agent*> agents, int market_prediction, signum result) :
    m_index {index}, m_market_prediction {market_prediction}, m_result {result}, m_agents {std::move (agents)} {}

signum MarketDay::result() const {return m_result;}
int MarketDay::market_count() const {return m_market_prediction;}
const std::vector<Agent*>& MarketDay::agents() const {return m_agents;};
void MarketDay::print() {printf ("Day %i : market_prediction %i result %i\n", m_index, m_market_prediction, m_result); }
void MarketDay::reset_agents (std::vector<Agent*> agents) { m_agents = std::move (agents); }

// Market History Implementations
//********************************
    MarketHistory::MarketHistory (std::vector<MarketDay> pre_history) :
            history(std::move(pre_history)),
            num_days_pre_history(history.size())
    {}

    int MarketHistory::index_of_current_day() const {
      return history.size() - num_days_pre_history;
    }

    uint64_t MarketHistory::last_n_results_as_bits(int n) const {
        assert (n < 32);
        assert (history.size() >= n);

        unsigned int output = 0;
        auto begin = history.end() - n;
        for (auto iter = begin; iter != history.end(); ++iter) {
            if (iter->result() == 1) output += 1 << (iter - begin);
        }
        return output;
    }

    const MarketDay& MarketHistory::last_day() const {return history.back();}
    MarketDay& MarketHistory::last_day() {return history.back();}
    signum MarketHistory::last_result() const { return history.back().result(); }
    int MarketHistory::market_count() const {return history.back().market_count();}
    int MarketHistory::market_result() const {return history.back().result();}
    int MarketHistory::market_count_at_day_i(int i) const {return history[i].market_count();}
    int MarketHistory::population_at_n(int n) const { return history[n].agents().size(); }
    int MarketHistory::history_size() const { return history.size();}

//Neither market history functions work, as for whatever reason the history.size inside evaluates to prehistory size. Not sure why.
    vector<int> MarketHistory::nonbinary_market_history() const {
        vector<int> nonbinary_history;
        printf("history size = %i, num days of prehistory = %i", history.size(), num_days_pre_history);
        for (int i = num_days_pre_history; i < history.size() - num_days_pre_history; ++i) {
            nonbinary_history.push_back(history[i].market_count());
        }
        return nonbinary_history;
    }

    vector<signum > MarketHistory::binary_market_history() const {
        vector<signum> binary_history;
        for (int i = num_days_pre_history; i < history.size()-num_days_pre_history; ++i) {
            binary_history.push_back(history[i].result());
        }
        return binary_history;
    }

    void MarketHistory::add_day (MarketDay new_day) {history.emplace_back (std::move (new_day)); }

    void MarketHistory::print() {
        for (int i = 0; i < history.size(); ++i) {
            history[i].print();
        }
    }

    void MarketHistory::write_market_history(int last_num_days_printed) {
        ofstream attendance_history("Market History.txt");
        for (int i = history.size() + num_days_pre_history - last_num_days_printed; i < history.size(); ++i) {
            attendance_history << i << ", "
                               << history[i].market_count() << endl;
        }
    }


// Experiment State Member Implementations
//********************************
ExperimentState::ExperimentState (std::vector<MarketDay> pre_history, std::unique_ptr<EvolutionStrategy> evolution, AgentPool agents) :
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

MarketHistory* ExperimentState::return_market_history(){ return &market_history; }

void ExperimentState::simulate_day() {
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
            std::uniform_int_distribution<int> dist(-(int)floor(agent_count*0.2), (int)floor(agent_count*0.2));
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

void ExperimentState::simulate (int num_days) { for (;num_days > 0; --num_days) simulate_day(); }

void ExperimentState::print() {
    printf ("Market History:\n");
    market_history.print();
    printf ("Agents:\n");
    for (auto& a : agent_pool) {
        a->print();
    }
}

void ExperimentState::write_last_n_market_history(int num_days_printed) { market_history.write_market_history(num_days_printed); }

void ExperimentState::write_agent_populations(){
    ofstream agentpops("agent population history.txt");
    for (int i = 0; i < market_history.history_size(); ++i) {
        agentpops << i << ", "
                  << market_history.population_at_n(i) << endl;
    }
}

// *****************************************************************
//  Initializations
// *****************************************************************

std::vector<MarketDay> basic_pre_history(int size, int seed, int num_agents) {
    auto binary_pre_history = random_signum_vector (size, seed);
    auto market_pre_history = market_history_generator (binary_pre_history, num_agents, seed);
    std::vector<MarketDay> result;

    for (int i  = 0; i != size; ++i) {
        result.emplace_back (i - size, market_pre_history[i], binary_pre_history[i]);
    }
    return std::move (result);
} //Generates Prehistory (i.e. initial random market data on which the agents make their first few decisions

AgentPool alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{i*strategy_set_incrementor, num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
} //initializes agent pool with alpha agents

AgentPool linear_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory, int agent_increment) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{linear_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, agent_population, agent_increment), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

AgentPool exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory, int agent_increment, double alpha) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, agent_population, agent_increment, alpha), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

AgentPool weighted_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory, int agent_increment, double alpha) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{weighted_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, agent_population, agent_increment, alpha), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

AgentPool stochastic_exponential_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory, double lambda) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_exponential_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory, lambda), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

AgentPool stocastic_poisson_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_poisson_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

AgentPool stochastic_random_mem_alpha_agents(int agent_population, int num_strategies_per_agent, int num_indicies_in_strategy, int strategy_set_incrementor, int max_memory, int min_memory) {
    AgentPool agents;
    for (int i = 0; i < agent_population; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{stocastic_random_mem_dist_init(i*strategy_set_incrementor, num_strategies_per_agent, max_memory, min_memory), num_strategies_per_agent, num_indicies_in_strategy}});
    }
    return std::move (agents);
}

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

// *****************************************************************
// Main Functions
// *****************************************************************

void write_market_histories(int rng_resolution, int rng_seed, int min_agent_pop, int max_agent_pop, int pop_interval, int runtime){
    ofstream Market_Histories("7 Market_Histories.txt");
    vector<vector<int>> market_histories;
    for (int agent_pop = min_agent_pop; agent_pop <= max_agent_pop; agent_pop += pop_interval) {
        ExperimentState experiment {basic_pre_history(32, rng_seed, agent_pop), std::unique_ptr<EvolutionStrategy> {new Creationism {}}, random_agents(agent_pop, rng_resolution)};
        //ExperimentState experiment {basic_pre_history(32, rng_seed, agent_pop), std::unique_ptr<EvolutionStrategy> {new Creationism {}}, alpha_agents(agent_pop, 2, 15, 10000)};
        experiment.simulate(runtime);

        vector<int> market_history;
        for (int i = 32; i < runtime; ++i) {
            market_history.push_back(experiment.return_market_history()->market_count_at_day_i(i));
        }
        printf("Mean of %i pop size attendance history = %lf \n", agent_pop, Analysis::mean(market_history));
        market_histories.emplace_back(market_history);
    }

    for (int j = 32; j < runtime; ++j) {
        for (int k = 0; k < market_histories.size(); ++k) {
            Market_Histories << market_histories[k][j] << ", ";
        }
        Market_Histories << endl;
    }
}

void write_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int max_memory, int min_memory,
                          int memory_interval){
    assert(max_memory < 32);
    assert(min_agent_pop % 2 == 1);
    ofstream Observables("Observables for Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
    ofstream means("Means of Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
    for (int memory = min_memory; memory <= max_memory; memory += memory_interval) {
        printf("Started %i memory run of %i total \n", memory, max_memory);
        for (int agent_pop = min_agent_pop; agent_pop <= max_agent_pop; agent_pop += agent_pop_interval) {
            double mean = 0;
            //mean - just to test variance theory
            double Alpha = pow(2, double(memory)) / agent_pop;;
            double Variance_over_agent_pop = 0;
            double successRate = 0;
            double elementRange = 0;
            for (int strategy_set = 1; strategy_set < num_diff_strategy_sets*agent_pop*num_strategies_per_agent; strategy_set += agent_pop*num_strategies_per_agent) {
                ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                           std::unique_ptr<EvolutionStrategy> {new Creationism{}},
                                           alpha_agents(agent_pop, num_strategies_per_agent, memory, strategy_set)};
                //random_agents(agent_pop, memory)};
                //random_agents(agent_pop, 10000+strategy_set)}; //10000 rng resolution atm, and addition leads to entirely new construction, and thus new seed effect each time
                experiment.simulate(num_days);
                vector<int> non_binary_history;
                for (int i = 32; i < experiment.return_market_history()->index_of_current_day(); ++i) { //from 32 to account for prehistory
                    non_binary_history.emplace_back(experiment.return_market_history()->market_count_at_day_i(i));
                }
                assert(!non_binary_history.empty());
                mean += Analysis::mean(non_binary_history);
                Variance_over_agent_pop += Analysis::variance(non_binary_history)/agent_pop;
                successRate += Analysis::success_rate(non_binary_history, agent_pop);
                elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / (double) agent_pop*2);

                means << Alpha << ", "
                      << agent_pop << ", "
                      << memory << ", "
                      << Analysis::mean(non_binary_history) << endl; //only gives different values for diff strategy sets for non-rnd agents
            }



            Observables << Alpha << ", "
                        << agent_pop << ", "
                        << memory << ", "
                        << mean / num_diff_strategy_sets << ", "
                        << Variance_over_agent_pop / num_diff_strategy_sets << ", "
                        << successRate / num_diff_strategy_sets  << ", "
                        << elementRange / num_diff_strategy_sets << endl;
        }
    }
}

