#include <iostream>
#include <string>
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
const std::vector<int> MarketDay::generate_memory_vector() const {
    std::vector<int> memories;
    for (auto e : m_agents) {
        for (int i = 0; i < e->return_num_strategies(); ++i) {
            memories.emplace_back(e->return_memory(i));
        }
    }
    assert(!memories.empty());
    return memories;
}
const std::vector<int> MarketDay::generate_strategy_vector() const {
    std::vector<int> memories;
    for (auto e : m_agents) {
        memories.emplace_back(e->return_num_strategies());
    }
    assert(!memories.empty());
    return memories;
}

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
    assert (n > 0);

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
double MarketHistory::ave_memory_at_n(int n) const { return Analysis::mean(history[n].generate_memory_vector()); }
double MarketHistory::ave_strategy_count_at_n(int n) const { return Analysis::mean(history[n].generate_strategy_vector()); }
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

void MarketHistory::labeled_write_market_history(double memory_delta, int evolutionary_period, int last_num_days_printed) {
    string start("evolutionary period = ");
    string And(" and ");
    string base(" Market History.txt");
    ofstream attendance_history(start+to_string(evolutionary_period)+And+to_string(memory_delta)+base);
    for (int i = history.size() + num_days_pre_history - last_num_days_printed; i < history.size(); ++i) {
        attendance_history << i << ", "
                           << history[i].market_count() << endl;
    }
}
void MarketHistory::strategy_labeled_write_market_history(double strategy, int evolutionary_period, int last_num_days_printed) {
    string start("evolutionary period = ");
    string And(" and ");
    string base(" Market History.txt");
    ofstream attendance_history(start+to_string(evolutionary_period)+And+to_string(strategy)+base);
    for (int i = history.size() + num_days_pre_history - last_num_days_printed; i < history.size(); ++i) {
        attendance_history << i << ", "
                           << history[i].market_count() << endl;
    }
}

vector<int> MarketHistory::return_memories_at_date(int date) const { return (history[date].generate_memory_vector()); }
vector<int> MarketHistory::return_strategies_at_date(int date) const { return (history[date].generate_strategy_vector()); }

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
            //that 0.2 is there because that's more realistic to the actual range of values generated in simulation
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
void ExperimentState::thermal_simulate_day() {
    const int index_of_day = market_history.index_of_current_day();
    // Evolution
    auto agent_generation = evolution_strategy->select_next_generation (market_history, agent_pool);
    //assert (agent_generation.size() == history.last_day().agents().size());
    assert (!agent_generation.empty());

    // Agent prediction
    int market_count = 0;
    for (auto a : agent_generation) {
        market_count += a->get_thermal_prediction(market_history);
    }

    // Market decision if 0 (only relevant for evolutionary models with intermittently even agent populations)
    if (market_count == 0) {
        printf("market count == 0 !!! -> %i\n", index_of_day);
        std::mt19937 gen(index_of_day);
        while (market_count == 0) {
            auto agent_count = agent_generation.size();
            std::uniform_int_distribution<int> dist(-(int)floor(agent_count*0.2), (int)floor(agent_count*0.2));
            //that 0.2 is there because that's more realistic to the actual range of values generated in simulation
            market_count = dist(gen);
        }
    }
    assert (market_count != 0);
    signum binary_market_result = market_count > 0 ? -1 : 1; // As we're registering the minority

    // Agent updates
    for (auto a : agent_generation) {
        a->thermal_update(market_history, binary_market_result);
    }

    // Finalizing
    market_history.add_day (MarketDay {market_history.index_of_current_day(), std::move (agent_generation), market_count, market_count > 0 ? -1 : 1});
}

void ExperimentState::simulate (int num_days) { for (;num_days > 0; --num_days) simulate_day(); }
void ExperimentState::thermal_simulate (int num_days) { for (;num_days > 0; --num_days) thermal_simulate_day(); }

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

int ExperimentState::return_agent_pop(int date){ return market_history.population_at_n(date); }

double ExperimentState::return_ave_memory(int date){ return market_history.ave_memory_at_n(date);}

void ExperimentState::write_mean_memories(int end_of_range){
    ofstream mean_memories("Memory Means.txt");
    for (int i = 32; i < end_of_range; ++i) {
        mean_memories << i-32 << ", "
                      << market_history.ave_memory_at_n(i) << endl;
    }
}

void ExperimentState::write_memory_frequencies(int date){
    assert(date >= 32); //Because we need to take from beyond the prehistory (i.e. where agents are actually in place)
    std::cout<<"Memory Frequencies:"<<endl;
    debug_print(Analysis::frequency_histogram(market_history.return_memories_at_date(date)));
    write_frequencies(market_history.return_memories_at_date(date));
}

void ExperimentState::labeled_write_last_n_market_history(double memory_delta, int evolutionary_period, int num_days_printed) { market_history.labeled_write_market_history(memory_delta, evolutionary_period, num_days_printed); }
void ExperimentState::strategy_labeled_write_last_n_market_history(double strategy_delta, int evolutionary_period, int num_days_printed) { market_history.strategy_labeled_write_market_history(strategy_delta, evolutionary_period, num_days_printed); }

void ExperimentState::labeled_write_memory_frequencies(double memory_delta, int evolutionary_period, int date){
    assert(date >= 32); //Because we need to take from beyond the prehistory (i.e. where agents are actually in place)
    string start("evolutionary period = ");
    string And(" and ");
    string base(" memory delta on last day Memory Histogram.txt");
    ofstream Frequencies(start+to_string(evolutionary_period)+And+to_string(memory_delta)+base);
    vector<vector<int>> Frequency_values = Analysis::frequency_histogram(market_history.return_memories_at_date(date));
    for (int i = 0; i < Frequency_values.size(); ++i) {
        Frequencies << Frequency_values[i][0] << ", "
                    << Frequency_values[i][1] << endl;
    }
}

void ExperimentState::labeled_write_mean_memories(double memory_delta, int evolutionary_period, int end_of_range){
    string start("evolutionary period = ");
    string And(" and ");
    string base(" delta Memory Means.txt");
    ofstream mean_memories(start+to_string(evolutionary_period)+And+to_string(memory_delta)+base);
    for (int i = 32; i < end_of_range; ++i) {
        mean_memories << i-32 << ", "
                      << market_history.ave_memory_at_n(i) << endl;
    }
}

void ExperimentState::thermal_labeled_write_memory_frequencies(double memory_delta, int evolutionary_period, int date){
    assert(date >= 32); //Because we need to take from beyond the prehistory (i.e. where agents are actually in place)
    string start("thermal evolutionary period = ");
    string And(" and ");
    string base(" memory delta on last day Memory Histogram.txt");
    ofstream Frequencies(start+to_string(evolutionary_period)+And+to_string(memory_delta)+base);
    vector<vector<int>> Frequency_values = Analysis::frequency_histogram(market_history.return_memories_at_date(date));
    for (int i = 0; i < Frequency_values.size(); ++i) {
        Frequencies << Frequency_values[i][0] << ", "
                    << Frequency_values[i][1] << endl;
    }
}

void ExperimentState::thermal_labeled_write_mean_memories(double memory_delta, int evolutionary_period, int end_of_range){
    string start("thermal evolutionary period = ");
    string And(" and ");
    string base(" delta Memory Means.txt");
    ofstream mean_memories(start+to_string(evolutionary_period)+And+to_string(memory_delta)+base);
    for (int i = 32; i < end_of_range; ++i) {
        mean_memories << i-32 << ", "
                      << market_history.ave_memory_at_n(i) << endl;
    }
}

void ExperimentState::labeled_write_strategy_frequencies(double strategy_delta, int evolutionary_period, int date){
    assert(date >= 32); //Because we need to take from beyond the prehistory (i.e. where agents are actually in place)
    string start("evolutionary period = ");
    string And(" and ");
    string base(" strategy delta on last day Strategy Histogram.txt");
    ofstream Frequencies(start+to_string(evolutionary_period)+And+to_string(strategy_delta)+base);
    vector<vector<int>> Frequency_values = Analysis::frequency_histogram(market_history.return_strategies_at_date(date));
    for (int i = 0; i < Frequency_values.size(); ++i) {
        Frequencies << Frequency_values[i][0] << ", "
                    << Frequency_values[i][1] << endl;
    }
}

void ExperimentState::labeled_write_mean_strategies(double strategy_delta, int evolutionary_period, int end_of_range){
    string start("evolutionary period = ");
    string And(" and ");
    string base(" delta strategy Means.txt");
    ofstream mean_memories(start+to_string(evolutionary_period)+And+to_string(strategy_delta)+base);
    for (int i = 32; i < end_of_range; ++i) {
        mean_memories << i-32 << ", "
                      << market_history.ave_memory_at_n(i) << endl;
    }
}

void ExperimentState::thermal_labeled_write_strategy_frequencies(double strategy_delta, int evolutionary_period, int date){
    assert(date >= 32); //Because we need to take from beyond the prehistory (i.e. where agents are actually in place)
    string start("thermal_evolutionary period = ");
    string And(" and ");
    string base(" strategy delta on last day Strategy Histogram.txt");
    ofstream Frequencies(start+to_string(evolutionary_period)+And+to_string(strategy_delta)+base);
    vector<vector<int>> Frequency_values = Analysis::frequency_histogram(market_history.return_strategies_at_date(date));
    for (int i = 0; i < Frequency_values.size(); ++i) {
        Frequencies << Frequency_values[i][0] << ", "
                    << Frequency_values[i][1] << endl;
    }
}

void ExperimentState::thermal_labeled_write_mean_strategies(double strategy_delta, int evolutionary_period, int end_of_range){
    string start("thermal_evolutionary period = ");
    string And(" and ");
    string base(" delta strategy Means.txt");
    ofstream mean_memories(start+to_string(evolutionary_period)+And+to_string(strategy_delta)+base);
    for (int i = 32; i < end_of_range; ++i) {
        mean_memories << i-32 << ", "
                      << market_history.ave_memory_at_n(i) << endl;
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

void write_thermal_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int max_memory, int min_memory,
                          int memory_interval){
    assert(max_memory < 32);
    assert(min_agent_pop % 2 == 1);
    ofstream Observables("TMG Observables for Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
    ofstream means("TMG Means of Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
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
                experiment.thermal_simulate(num_days);
                vector<int> non_binary_history;
                for (int i = 32; i < experiment.return_market_history()->index_of_current_day(); ++i) { //from 32 to account for prehistory VERY IMPORTANT
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

void write_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                          int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop,
                          int agent_pop_interval, int max_memory, int min_memory,
                          int memory_interval, int evolutionary_length, double memory_delta,
                                       double strategy_delta, double breeding_delta,
                                       int max_evol_memory, int min_evol_memory,
                                       int max_num_strategies, int min_num_strategies){
    assert(max_memory < 32);
    assert(min_agent_pop % 2 == 1 && max_agent_pop % 2 == 1);
    ofstream Observables("Observables for Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
    ofstream means("Means of Memory from 2 to 16, Pop from 101 to 1001, 10 Strategy Sets and 1000 Iterations.txt");
    ofstream memory_histogram("Memory histograms");
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
                                           std::unique_ptr<EvolutionStrategy> {new Darwinism {memory_delta, strategy_delta, breeding_delta, max_evol_memory, min_evol_memory, max_num_strategies, min_num_strategies}},
                                           darwinian_agents(agent_pop, num_strategies_per_agent, memory, 50000, evolutionary_length)};
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

                if(strategy_set == 1){ //As I just need one sample
                    for(int date = 0; date < num_days; date+=(int)((double)num_days/10)) {
                        experiment.labeled_write_memory_frequencies(memory_delta, evolutionary_length, date);
                    }
                }
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

void write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                       int agent_pop, int memory_length, int num_memory_delta_values,
                                       int init_evolutionary_length, int num_evolutionary_lengths, double memory_delta,
                                       double strategy_delta, double breeding_delta,
                                       int max_evol_memory, int min_evol_memory,
                                       int max_num_strategies, int min_num_strategies){
    assert(max_evol_memory < 32);
    int Evolutionary_period = init_evolutionary_length;
    for (int k = 0; k < num_evolutionary_lengths ; ++k) {
        double Memory_Delta = memory_delta;
        string base("Observables for memory evolutionary with memory_delta = 0.05 to 0.5, 20000 runs, mem from 1 to 20, evol_period = ");
        string txt(".txt");
        ofstream Observables(base.append(to_string(Evolutionary_period))+txt);
        for (int i = 0; i <= num_memory_delta_values - 1; ++i) {
        printf("Started %lf memory delta run of up to %lf \n", Memory_Delta, num_memory_delta_values*memory_delta);
            double average_alpha = 0;
            double Variance_over_agent_pop = 0;
            double mean = 0;
            double mean_memory = 0;
            double successRate = 0;
            double elementRange = 0;
            ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                       std::unique_ptr<EvolutionStrategy> {
                                               new Darwinism{Memory_Delta, strategy_delta, breeding_delta,
                                                             max_evol_memory, min_evol_memory, max_num_strategies,
                                                             min_num_strategies}},
                                       darwinian_agents(agent_pop, num_strategies_per_agent, memory_length, 50000,
                                                        Evolutionary_period)};
            experiment.simulate(num_days);
            vector<int> non_binary_history;
            auto last_day = experiment.return_market_history()->index_of_current_day();
            for (int j = 32; j < last_day; ++j) { //from 32 to account for prehistory
                non_binary_history.emplace_back(experiment.return_market_history()->market_count_at_day_i(j));
                mean_memory += experiment.return_market_history()->ave_memory_at_n(j);
            }
            assert(!non_binary_history.empty());
            average_alpha =
                    pow(2, experiment.return_ave_memory(last_day)) / (double) experiment.return_agent_pop(last_day);
            mean_memory /= (last_day - 32);
            mean += Analysis::mean(non_binary_history);
            Variance_over_agent_pop += Analysis::variance(non_binary_history) / agent_pop;
            successRate += Analysis::success_rate(non_binary_history, agent_pop);
            elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / (double) agent_pop * 2);

//        for(int date = 33; date < num_days;) {
//            experiment.labeled_write_memory_frequencies(date);
//            if (date == 33) { date -= 33; }
//            date += (int) ((double) (num_days - 32) / 10);
//        }//For memory distributions over time
            if (((int) (Memory_Delta*100)) % 10 == 0) {
                experiment.labeled_write_last_n_market_history(memory_delta, Evolutionary_period, num_days);
                experiment.labeled_write_mean_memories(memory_delta, Evolutionary_period, num_days);
                experiment.labeled_write_memory_frequencies(memory_delta, Evolutionary_period, num_days);
            }

            Observables << average_alpha << ", "
                        << agent_pop << ", "
                        << Memory_Delta << ", "
                        << mean << ", "
                        << Variance_over_agent_pop << ", "
                        << mean_memory << ", "
                        << successRate << ", "
                        << elementRange << endl;
            Memory_Delta += memory_delta;
        }
        Evolutionary_period += init_evolutionary_length;
        printf("Evolutionary period = %i \n", Evolutionary_period);
    }
}

void thermal_write_memory_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                       int agent_pop, int memory_length, int num_memory_delta_values,
                                       int init_evolutionary_length, int num_evolutionary_lengths, double memory_delta,
                                       double strategy_delta, double breeding_delta,
                                       int max_evol_memory, int min_evol_memory,
                                       int max_num_strategies, int min_num_strategies){
    assert(max_evol_memory < 32);
    int Evolutionary_period = init_evolutionary_length;
    for (int k = 0; k < num_evolutionary_lengths ; ++k) {
        double Memory_Delta = memory_delta;
        string base("Observables for thermal memory evolution with memory_delta = 0.02  5 to 0.7, 20000 runs, mem from 1 to 20, evol_period = ");
        string txt(".txt");
        ofstream Observables(base.append(to_string(Evolutionary_period))+txt);
        for (int i = 0; i <= num_memory_delta_values - 1; ++i) {
        printf("Started %lf memory delta run of up to %lf \n", Memory_Delta, num_memory_delta_values*memory_delta);
            double average_alpha = 0;
            double Variance_over_agent_pop = 0;
            double mean = 0;
            double mean_memory = 0;
            double successRate = 0;
            double elementRange = 0;
            ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                       std::unique_ptr<EvolutionStrategy> {
                                               new Darwinism{Memory_Delta, strategy_delta, breeding_delta,
                                                             max_evol_memory, min_evol_memory, max_num_strategies,
                                                             min_num_strategies}},
                                       darwinian_agents(agent_pop, num_strategies_per_agent, memory_length, 50000,
                                                        Evolutionary_period)};
            experiment.thermal_simulate(num_days);
            vector<int> non_binary_history;
            auto last_day = experiment.return_market_history()->index_of_current_day();
            for (int j = 32; j < last_day; ++j) { //from 32 to account for prehistory
                non_binary_history.emplace_back(experiment.return_market_history()->market_count_at_day_i(j));
                mean_memory += experiment.return_market_history()->ave_memory_at_n(j);
            }
            assert(!non_binary_history.empty());
            average_alpha =
                    pow(2, experiment.return_ave_memory(last_day)) / (double) experiment.return_agent_pop(last_day);
            mean_memory /= (last_day - 32);
            mean += Analysis::mean(non_binary_history);
            Variance_over_agent_pop += Analysis::variance(non_binary_history) / agent_pop;
            successRate += Analysis::success_rate(non_binary_history, agent_pop);
            elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / (double) agent_pop * 2);

//        for(int date = 33; date < num_days;) {
//            experiment.labeled_write_memory_frequencies(date);
//            if (date == 33) { date -= 33; }
//            date += (int) ((double) (num_days - 32) / 10);
//        }//For memory distributions over time
            if (((int) (Memory_Delta*100)) % 10 == 0) {
                experiment.labeled_write_last_n_market_history(memory_delta, Evolutionary_period, num_days);
                experiment.thermal_labeled_write_mean_memories(memory_delta, Evolutionary_period, num_days);
                experiment.thermal_labeled_write_memory_frequencies(memory_delta, Evolutionary_period, num_days);
            }//No need for the -32?

            Observables << average_alpha << ", "
                        << agent_pop << ", "
                        << Memory_Delta << ", "
                        << mean << ", "
                        << Variance_over_agent_pop << ", "
                        << mean_memory << ", "
                        << successRate << ", "
                        << elementRange << endl;
            Memory_Delta += memory_delta;
        }
        Evolutionary_period += init_evolutionary_length;
        printf("Evolutionary period = %i \n", Evolutionary_period);
    }
}

void write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                              int agent_pop, int memory_length, int num_memory_delta_values,
                                              int init_evolutionary_length, int num_strategy_lengths, double memory_delta,
                                              double strategy_delta, double breeding_delta,
                                              int max_evol_memory, int min_evol_memory,
                                              int max_num_strategies, int min_num_strategies){
    assert(max_evol_memory < 32);
    int Evolutionary_period = init_evolutionary_length;
    for (int k = 0; k < num_strategy_lengths ; ++k) {
        double Strategy_Delta = strategy_delta;
        string base("Observables for strategy evolutionary with memory_delta = 0.05 to 0.5, 20000 runs, mem from 1 to 20, evol_period = ");
        string txt(".txt");
        ofstream Observables(base.append(to_string(Evolutionary_period))+txt);
        for (int i = 0; i <= num_memory_delta_values - 1; ++i) {
            printf("Started %lf memory delta run of up to %lf \n", Strategy_Delta, num_memory_delta_values*strategy_delta);
            double average_alpha = 0;
            double Variance_over_agent_pop = 0;
            double mean = 0;
            double mean_memory = 0;
            double successRate = 0;
            double elementRange = 0;
            ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                       std::unique_ptr<EvolutionStrategy> {
                                               new Darwinism{Strategy_Delta, memory_delta, breeding_delta,
                                                             max_evol_memory, min_evol_memory, max_num_strategies,
                                                             min_num_strategies}},
                                       darwinian_agents(agent_pop, num_strategies_per_agent, memory_length, 50000,
                                                        Evolutionary_period)};
            experiment.simulate(num_days);
            vector<int> non_binary_history;
            auto last_day = experiment.return_market_history()->index_of_current_day();
            for (int j = 32; j < last_day; ++j) { //from 32 to account for prehistory
                non_binary_history.emplace_back(experiment.return_market_history()->market_count_at_day_i(j));
                mean_memory += experiment.return_market_history()->ave_memory_at_n(j);
            }
            assert(!non_binary_history.empty());
            average_alpha =
                    pow(2, experiment.return_ave_memory(last_day)) / (double) experiment.return_agent_pop(last_day);
            mean_memory /= (last_day - 32);
            mean += Analysis::mean(non_binary_history);
            Variance_over_agent_pop += Analysis::variance(non_binary_history) / agent_pop;
            successRate += Analysis::success_rate(non_binary_history, agent_pop);
            elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / (double) agent_pop * 2);

//        for(int date = 33; date < num_days;) {
//            experiment.labeled_write_memory_frequencies(date);
//            if (date == 33) { date -= 33; }
//            date += (int) ((double) (num_days - 32) / 10);
//        }//For memory distributions over time
            if (((int) (Strategy_Delta*100)) % 10 == 0) {
                experiment.strategy_labeled_write_last_n_market_history(strategy_delta, Evolutionary_period, num_days);
                experiment.labeled_write_mean_strategies(strategy_delta, Evolutionary_period, num_days);
                experiment.labeled_write_strategy_frequencies(strategy_delta, Evolutionary_period, num_days);
            }

            Observables << average_alpha << ", "
                        << agent_pop << ", "
                        << Strategy_Delta << ", "
                        << mean << ", "
                        << Variance_over_agent_pop << ", "
                        << mean_memory << ", "
                        << successRate << ", "
                        << elementRange << endl;
            Strategy_Delta += strategy_delta;
        }
        Evolutionary_period += init_evolutionary_length;
        printf("Evolutionary period = %i \n", Evolutionary_period);
    }
}

void thermal_write_strategy_evolutionary_mg_observables(int num_days, int num_strategies_per_agent, int seed,
                                                int agent_pop, int memory_length, int num_memory_delta_values,
                                                int init_evolutionary_length, int num_strategy_lengths, double memory_delta,
                                                double strategy_delta, double breeding_delta,
                                                int max_evol_memory, int min_evol_memory,
                                                int max_num_strategies, int min_num_strategies){
    assert(max_evol_memory < 32);
    int Evolutionary_period = init_evolutionary_length;
    for (int k = 0; k < num_strategy_lengths ; ++k) {
        double Strategy_Delta = strategy_delta;
        string base("Observables for strategy evolutionary with memory_delta = 0.05 to 0.5, 20000 runs, mem from 1 to 20, evol_period = ");
        string txt(".txt");
        ofstream Observables(base.append(to_string(Evolutionary_period))+txt);
        for (int i = 0; i <= num_memory_delta_values - 1; ++i) {
            printf("Started %lf memory delta run of up to %lf \n", Strategy_Delta, num_memory_delta_values*strategy_delta);
            double average_alpha = 0;
            double Variance_over_agent_pop = 0;
            double mean = 0;
            double mean_memory = 0;
            double successRate = 0;
            double elementRange = 0;
            ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                       std::unique_ptr<EvolutionStrategy> {
                                               new Darwinism{Strategy_Delta, memory_delta, breeding_delta,
                                                             max_evol_memory, min_evol_memory, max_num_strategies,
                                                             min_num_strategies}},
                                       darwinian_agents(agent_pop, num_strategies_per_agent, memory_length, 50000,
                                                        Evolutionary_period)};
            experiment.thermal_simulate(num_days);
            vector<int> non_binary_history;
            auto last_day = experiment.return_market_history()->index_of_current_day();
            for (int j = 32; j < last_day; ++j) { //from 32 to account for prehistory
                non_binary_history.emplace_back(experiment.return_market_history()->market_count_at_day_i(j));
                mean_memory += experiment.return_market_history()->ave_memory_at_n(j);
            }
            assert(!non_binary_history.empty());
            average_alpha =
                    pow(2, experiment.return_ave_memory(last_day)) / (double) experiment.return_agent_pop(last_day);
            mean_memory /= (last_day - 32);
            mean += Analysis::mean(non_binary_history);
            Variance_over_agent_pop += Analysis::variance(non_binary_history) / agent_pop;
            successRate += Analysis::success_rate(non_binary_history, agent_pop);
            elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / (double) agent_pop * 2);

            if (((int) (Strategy_Delta*100)) % 10 == 0) {
                experiment.strategy_labeled_write_last_n_market_history(strategy_delta, Evolutionary_period, num_days);
                experiment.labeled_write_mean_strategies(strategy_delta, Evolutionary_period, num_days);
                experiment.labeled_write_strategy_frequencies(strategy_delta, Evolutionary_period, num_days);
            }

            Observables << average_alpha << ", "
                        << agent_pop << ", "
                        << Strategy_Delta << ", "
                        << mean << ", "
                        << Variance_over_agent_pop << ", "
                        << mean_memory << ", "
                        << successRate << ", "
                        << elementRange << endl;
            Strategy_Delta += strategy_delta;
        }
        Evolutionary_period += init_evolutionary_length;
        printf("Evolutionary period = %i \n", Evolutionary_period);
    }
}
