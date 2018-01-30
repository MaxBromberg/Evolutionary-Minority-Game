#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "Evolutionary_Minority_Game.h"
#include "Agents and Evolution Strategies.h"
#include "analysis_utilities.h"

using namespace std;

// ***************************************************************
//  Utilities Implementation
// ***************************************************************

template <typename Iter>
unsigned int sign_array_to_bits (Iter begin, Iter end) {
    int output = 0;
    for (auto iter = begin; iter != end; ++iter) {
        if (*iter == 1) output += 1 << (iter - begin);
    }
    return output;
}

vector<int> random_bool_vector(int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

uint64_t signum_vector_to_bits(const std::vector<signum> &v, int n) {
    return sign_array_to_bits (v.end() - n, v.end());
}

vector<signum> random_signum_vector(int NumIndicesInStrategy, int seed){ //fills a vector with +/-1
    return random_bool_vector(NumIndicesInStrategy, seed, 1, -1);
}

vector<int> market_history_generator(const vector<signum> &source, int agentPopulation, int seed){
    mt19937 generator {static_cast<unsigned int>(seed)};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop). Keeping it for conformity

int random_generate(double weight, int range, int seed){
    mt19937 generator(seed);
    uniform_int_distribution<int> memory_dist (1, range);
    auto memory = (int) (floor(memory_dist(generator) + weight*memory_dist(generator)));
    if( memory < 1){
        return random_generate(weight, range, seed + 1);
    }else if (memory > range){
        return random_generate(weight, range, seed + 1);
    }
    return memory;
}

//Old code, just for reference
/*
vector<AlphaAgent> Experiment::initialize_agents(int agents_identifier) {
    vector<AlphaAgent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<signum> empty_last_n_actions;
        vector<Strategy> Strategies;
        int empty_total_wins = 0;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, num_indicies_in_strategy});
        }
        Agents.push_back(AlphaAgent{Strategies, empty_last_n_actions, empty_total_wins, i + agents_identifier});
        /*
        i is a perfectly unique identifier; we only have to deal in complexity after initialization, where we could
        get random numbers in the range of 1 billion, and still be fine for memories of up to 15 bits.
         //Note on identifier range, allocation
    }
    return Agents;
}

vector<AlphaAgent> Experiment::linear_memory_dist_agent_init(int agents_identifier) {
    assert(num_indicies_in_strategy < 31);
    vector<AlphaAgent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        vector<signum> last_n_actions;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, (i % num_indicies_in_strategy)+1});//1 is the minimum memory
        }
        Agents.push_back(AlphaAgent{Strategies, last_n_actions, 0,  i + agents_identifier});
    }
    return Agents;
}//gives even memory composition per agent (all strategies of a given agent have same memory length)

//likely ought to eliminate the base component. and just have it fixed to e.
vector<AlphaAgent> Experiment::exponential_memory_dist_agent_init(double base, double exp_factor, int agents_identifier) {
    assert(num_indicies_in_strategy < 31);
    vector<AlphaAgent> Agents;
    double exponential_increment = (log(num_indicies_in_strategy)/log(base))/(agent_count*exp_factor);
    cout<<"Total memory ought to equal = " << pow(base, exponential_increment*exp_factor*agent_count)<<endl;
    cout<<"exponential increment = " << exponential_increment<<endl;
    for(int i = 0; i < agent_count; i++){
        vector<signum> last_n_actions;
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, (int) floor(pow(base, (i*exponential_increment))+0.5)} );
        }
        Agents.push_back(AlphaAgent{Strategies, last_n_actions, 0, i + agents_identifier});
    }
    return Agents;
} //Presently over emphasizes the last drop, and under emphasizes the first allocation. Also the exp_factor adjusts the memory range

vector<AlphaAgent> Experiment::weighted_rnd_memory_dist_agent_init(double weight, int seed, int agents_identifier) {
    assert(num_indicies_in_strategy < 31);
    vector<AlphaAgent> Agents;
    mt19937 gen (seed);
    uniform_int_distribution<int> seed_generator;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        vector<signum> last_n_actions;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy{0, random_generate(weight, num_indicies_in_strategy, seed_generator(gen))});
        }
        Agents.push_back(AlphaAgent{Strategies, last_n_actions, 0, i + agents_identifier});
    }
    return Agents;
} //weight designed to range between -1 and 1, but can go really anywhere... (0 neutral)
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

Experiment::Experiment(int agent_population, int num_strategies_per_agent, int memory, int agents_identifier, int evolution_memory, int seed) {
    agent_count = agent_population;
    strategies_per_agent = num_strategies_per_agent;
    num_indicies_in_strategy = memory;
    evolutionary_history_length = evolution_memory;
    history = random_signum_vector(memory, seed);
    nonbinary_history = market_history_generator(history, agent_population, seed);
    agents = Experiment::initialize_agents(agents_identifier);
}

void Experiment::run_minority_game(int number_of_runs){
    for (int i = 0; i < history.size(); ++i) {
        printf ("PreHistory %i %i %i\n", i, history[i], nonbinary_history[i]);
    }
    for (int i = 0; i < number_of_runs; i++) {
        auto market_count = market_evaluation();
        printf ("Day %i : market_prediction %i ", i, market_count);
        if (market_count != 0) {
            one_minority_game_run(market_count, i);
        }else{
            printf ("market count == 0 !!! -> %i\n", i);
            mt19937 gen(i);
            uniform_int_distribution<int> dist(-agent_count , agent_count);
            one_minority_game_run(dist(gen), i);}
        //we hope this will not make a substantial difference, but this function structure allows us to see how many times it happens (if we want)
    }
}

void Experiment::one_minority_game_run(int market_count, int i) {
    auto binary_market_value = market_count > 0 ? -1 : 1;
    printf ("result %i\n", binary_market_value);
    for (int j = 0; j < agents.size(); j++) {
        agents[j].weighted_update(history, binary_market_value);
        if (i > evolutionary_history_length) {
            agents[j].agent_memory_boost(agents[j].streak(evolutionary_history_length));
        }
    }
    nonbinary_history.push_back(market_count);
    history.push_back(binary_market_value);
}

void Experiment::write_attendance_history(){
    ofstream Attendance("Evolutionary Market History for 10000 runs.txt");
    for(int i = num_indicies_in_strategy; i < nonbinary_history.size(); i++) {
        Attendance << i << ", "
                   << nonbinary_history[i] << ", "
                   << history[i] << endl;
    }
}

void Experiment::write_memory_distribution() {
    vector<int> memory_counts;
    for (int i = 0; i < agent_count; i++) {
        for (int j = 0; j < strategies_per_agent; j++) {
            memory_counts.push_back(agents[i].strategies[j].num_indicies_in_strategy);
        }
    }
    vector<vector<int>> memory_frequencies = Analysis::frequency_histogram(memory_counts);
    debug_print(memory_frequencies);

    int sum_of_changes = 0;
    double default_random_changes = agent_count*(pow(2, -evolutionary_history_length));
    ofstream memory_distribution("Evolutionary Memory Distribution.txt");
    for (int i = 0; i < memory_frequencies.size(); i++) {
        if(i > 2) {
            sum_of_changes += memory_frequencies[i][1];
        }
        memory_distribution << memory_frequencies[i][0] << ", "
                            << memory_frequencies[i][1] << endl;
    }
    //Applicable print outs only for streak applications
    cout << "Sum of memory shifts = " << sum_of_changes << endl;
    cout << "default random changes = " << default_random_changes << endl;
    cout << "their ratio: = " << (double) sum_of_changes/default_random_changes << endl;
}

void Experiment::write_minority_game_observables(int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS, int NUM_DIFF_MEMORY_LENGTHS, int NUM_STRATEGIES_PER_AGENT, int EVOLUTIONARY_MEMORY, int NUM_DIFF_STRATEGY_SETS) {

    ofstream file("Sin_predict Variance for Memory from 2 to 16, Pop from 101 to 701, 10 Strategy Sets and 5000 Iterations.txt");
    int agent_pop = 0;
    for (int a = 2; a <= NUM_DIFF_MEMORY_LENGTHS; a++) {
        int NUM_INDICES_IN_STRATEGY = a; //int(floor(pow(2, a) + .5));
        cout << "Just started " << NUM_INDICES_IN_STRATEGY << "th memory length run" << endl;
        for (int b = 1; b < NUM_DIFF_AGENT_POPS; b++) {
                agent_pop = (100 * b) + 1;
                double Alpha = 0;
                double Variance = 0;
                double Variance_over_agent_pop = 0;
                double successRate = 0;
                double elementRange = 0;
                for (int strategy_set_iterator = 1; strategy_set_iterator < NUM_DIFF_STRATEGY_SETS; strategy_set_iterator++) {
                    int agents_strategy_set = 5000*strategy_set_iterator;
                    //should be shifted at least the space of agents * strategies_per_agents, to give all new mappings.
                    //assert(((agents_strategy_set + NUM_STRATEGIES_PER_AGENT) * pow(2, NUM_INDICES_IN_STRATEGY) + 1) < pow(2, 31));
                    //assert shouldn't even be necessary foe sin_predict, as an overflown bit is still a bit

                Experiment environment{agent_pop, NUM_STRATEGIES_PER_AGENT, NUM_INDICES_IN_STRATEGY, agents_strategy_set, EVOLUTIONARY_MEMORY, 42};
                //Use agent pop as agents_identifier; so long as it changes each time, it will assign different strategies.
                environment.run_minority_game(NUM_DAYS_AGENTS_PLAY);
                /*
                running the simulation on this line over time means we are successfully taking the variance of the time
                and then averaging over the total number of simulations.

                Alpha += pow(2, double(NUM_INDICES_IN_STRATEGY)) / agent_pop;
                Variance += Analysis::unexpanded_variance(environment.nonbinary_history);
                Variance_over_agent_pop += Analysis::unexpanded_variance(environment.nonbinary_history)/agent_pop;
                successRate += Analysis::success_rate(environment.nonbinary_history, agent_pop);
                elementRange += ((double) Analysis::number_of_unique_elements(environment.nonbinary_history) /
                                       pow(2, double(NUM_INDICES_IN_STRATEGY)));
            }
            file << Alpha / NUM_DIFF_STRATEGY_SETS << ", "
                 << agent_pop << ", "
                 << Variance / NUM_DIFF_STRATEGY_SETS<< ", "
                 << Variance_over_agent_pop / NUM_DIFF_STRATEGY_SETS << ", "
                 << successRate / NUM_DIFF_STRATEGY_SETS << ", "
                 << elementRange / NUM_DIFF_STRATEGY_SETS<< endl;
        }
    }

}
*/

void ExperimentState::write_mg_observables(int num_days, int num_strategies_per_agent, int seed, int num_diff_strategy_sets, int max_agent_pop, int min_agent_pop, int agent_pop_interval, int max_memory, int min_memory, int memory_interval){
    assert(max_memory < 32);
    assert(min_agent_pop % 2 == 1);
    ofstream Observables("MG Observables for Memory from 2 to 16, Pop from 101 to 701, 25 Strategy Sets and 10000 Iterations.txt");
    for (int memory = min_memory; memory <= max_memory; memory += memory_interval) {
        printf("Started %i memory run of %i total \n", memory, max_memory);
        for (int agent_pop = min_agent_pop; agent_pop <= max_agent_pop; agent_pop += agent_pop_interval) {
            double Alpha = pow(2, double(memory)) / agent_pop;;
            double Variance = 0;
            double Variance_over_agent_pop = 0;
            double unexpanded_Variance_over_agent_pop = 0;
            double successRate = 0;
            double elementRange = 0;
            for (int strategy_set = 1; strategy_set < num_diff_strategy_sets*agent_pop*num_strategies_per_agent; strategy_set += agent_pop*num_strategies_per_agent) {
                ExperimentState experiment{basic_pre_history(32, seed, agent_pop),
                                           std::unique_ptr<EvolutionStrategy> {new Creationism{}},
                                           alpha_agents(agent_pop, num_strategies_per_agent, memory, strategy_set)};
                experiment.simulate(num_days);
                vector<int> non_binary_history;
                for (int i = 0; i < experiment.market_history.index_of_current_day(); ++i) {
                   non_binary_history.emplace_back(experiment.market_history.market_count_at_day_i(i));
                }
                assert(!non_binary_history.empty());
                Variance += Analysis::unexpanded_variance(non_binary_history);
                unexpanded_Variance_over_agent_pop += Variance/agent_pop;
                Variance_over_agent_pop += Analysis::variance(non_binary_history)/agent_pop;
                successRate += Analysis::success_rate(non_binary_history, agent_pop);
                elementRange += ((double) Analysis::number_of_unique_elements(non_binary_history) / Alpha);
            }
            Observables << Alpha<< ", "
                        << agent_pop << ", "
                        << memory << ", "
                        << Variance / num_diff_strategy_sets<< ", "
                        << Variance_over_agent_pop / num_diff_strategy_sets << ", "
                        << unexpanded_Variance_over_agent_pop / num_diff_strategy_sets << ", "
                        << successRate / num_diff_strategy_sets  << ", "
                        << elementRange / num_diff_strategy_sets << endl;
        }
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

AgentPool random_agents(int agent_pop) {
    AgentPool agents;
    for (int i = 0; i < agent_pop; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new RandomAgent{i}});
    }
    return std::move (agents);
} //

