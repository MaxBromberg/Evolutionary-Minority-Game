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
#include "analysis_utilities.h"

using namespace std;
//The Old
vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n) {
    return BinaryArrayToStrategyIndex (v.end() - n, v.end());
}

vector<signum> binaryMarketHistoryGenerator (int NumIndicesInStrategy, int seed){ //fills a vector with +/-1
    return RandomBoolVector (NumIndicesInStrategy, seed, 1, -1);
}

vector<int> marketHistoryGenerator(const vector<int>& source, int agentPopulation, int seed){
    mt19937 generator {seed};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop). Keeping it for conformity

//The New
signum Agent::predict(const int strategy_index, const int num_indicies_in_strategy, const std::vector<signum>& history) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    mt19937 generator ((unsigned int) ( (relatively_unique_identifer + strategy_index) * (BinaryVectorLastNToStrategyIndex (history, num_indicies_in_strategy)+1)));
    uniform_int_distribution<int> bit_distribution(0,1);
    return bit_distribution(generator) ? 1 : -1;
}

signum Agent::high_resolution_predict(const int strategy_index, const int num_indicies_in_strategy, const std::vector<signum>& history) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    mt19937 generator ((unsigned int) ( (relatively_unique_identifer + strategy_index) * (BinaryVectorLastNToStrategyIndex (history, num_indicies_in_strategy)+1)));
    uniform_int_distribution<int> bit_distribution(0, 100000);
    return bit_distribution(generator) < 50000 ? 1 : -1;
}

signum Agent::sin_predict(const int strategy_index, const int num_indicies_in_strategy,
                          const std::vector<signum> &history) const {
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    double sin_seed = sin((double) ( (relatively_unique_identifer + strategy_index) * (BinaryVectorLastNToStrategyIndex (history, num_indicies_in_strategy)+1)));
    //plus one to prevent 0 case problems
    return ((int)(sin_seed*10000) & 1) ? 1 : -1;
}

void Agent::update(const std::vector<signum> &history, const int& market_prediction) {
    for(int i = 0; i < strategies.size(); i++){
        if (Agent::sin_predict(i, strategies[i].num_indicies_in_strategy, history) == market_prediction) {
            strategies[i].strategy_score++;
        } else {
            strategies[i].strategy_score--;
        }
    }
} //Updates all strategies in vector

void Agent::weighted_update(const std::vector<signum> &history, const int& last_market_value) {
    for(int i = 0; i < strategies.size(); i++){
        if (Agent::high_resolution_predict(i, strategies[i].num_indicies_in_strategy, history) == sgn(last_market_value)) {
            strategies[i].strategy_score += abs(last_market_value);
        } else {
            strategies[i].strategy_score -= abs(last_market_value);
        }
    }
} //Updates all strategies in vector with the amount they were in/correct by.

int market_evaluation(const std::vector<Agent>& Agents, const std::vector<signum>& history){
    int market_evaluation = 0;
    for(int agent_index = 0; agent_index < Agents.size(); agent_index++){
        vector<int> strategy_scores;
        for(int i = 0; i < Agents[agent_index].strategies.size(); i++){
            strategy_scores.push_back(Agents[agent_index].strategies[i].strategy_score);
        }//Likely better way to find the max element...

        auto index_of_best_strategy = std::max_element(std::begin(strategy_scores), std::end(strategy_scores))-std::begin(strategy_scores);
        market_evaluation += Agents[agent_index].high_resolution_predict(index_of_best_strategy, Agents[agent_index].strategies[index_of_best_strategy].num_indicies_in_strategy, history);
    }
    //assert(abs(market_evaluation) <= Agents.size()); //failed before, though seemingly without reason
    return market_evaluation;
}

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

vector<Agent> Experiment::initialize_agents() {
    vector<Agent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, num_indicies_in_strategy});
        }
        Agents.push_back(Agent{Strategies, i});
        /*
        i is a perfectly unique identifier; we only have to deal in complexity after initialization, where we could
        get random numbers in the range of 1 billion, and still be fine for memories of up to 15 bits.
        */ //Note on identifier range, allocation
    }
    return Agents;
}

vector<Agent> Experiment::linear_memory_dist_agent_init() {
    assert(num_indicies_in_strategy < 31);
    vector<Agent> Agents;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, (i % num_indicies_in_strategy)+1});//1 is the minimum memory
        }
        Agents.push_back(Agent{Strategies, i});
    }
    return Agents;
}//gives even memory composition per agent (all strategies of a given agent have same memory length)

//likely ought to eliminate the base component. and just have it fixed to e.
vector<Agent> Experiment::exponential_memory_dist_agent_init(double base, double exp_factor) {
    assert(num_indicies_in_strategy < 31);
    vector<Agent> Agents;
    double exponential_increment = (log(num_indicies_in_strategy)/log(base))/(agent_count*exp_factor);
    cout<<"Total memory ought to equal = " << pow(base, exponential_increment*exp_factor*agent_count)<<endl;
    cout<<"exponential increment = " << exponential_increment<<endl;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy {0, (int) floor(pow(base, (i*exponential_increment))+0.5)} );
        }
        Agents.push_back(Agent{Strategies, i});
    }
    return Agents;
} //Presently over emphasizes the last drop, and under emphasizes the first allocation. Also the exp_factor adjusts the memory range

vector<Agent> Experiment::weighted_rnd_memory_dist_agent_init(double weight, int seed) {
    assert(num_indicies_in_strategy < 31);
    vector<Agent> Agents;
    mt19937 gen (seed);
    uniform_int_distribution<int> seed_generator;
    for(int i = 0; i < agent_count; i++){
        vector<Strategy> Strategies;
        for(int j = 0; j < strategies_per_agent; j++){
            Strategies.push_back(Strategy{0, random_generate(weight, num_indicies_in_strategy, seed_generator(gen))});
        }
        Agents.push_back(Agent{Strategies, i});
    }
    return Agents;
} //weight designed to range between -1 and 1, but can go really anywhere... (0 neutral)

vector<Agent> Experiment::bell_curve_memory_dist_agent_init(double kurtosis) {
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

Experiment::Experiment(int agent_population, int num_strategies_per_agent, int memory, int seed) {
    assert(agent_population % 2 == 1);//compiled to bitshift
    agent_count = agent_population;
    strategies_per_agent = num_strategies_per_agent;
    num_indicies_in_strategy = memory;
    history = binaryMarketHistoryGenerator(memory, seed);
    nonbinary_history = marketHistoryGenerator(history, agent_population, seed);
    agents = Experiment::initialize_agents();
}

void Experiment::run_minority_game(int number_of_runs){
    for(int i = 0; i < number_of_runs; i++){
        auto market_count = market_evaluation(agents, history);
        auto binary_market_value = market_count > 0 ? -1 : 1;
        for(int j = 0; j < agents.size(); j++){
            agents[j].weighted_update(history, binary_market_value);
        }
        nonbinary_history.push_back(market_count);
        history.push_back(binary_market_value);
    }
}

void Experiment::write_attendance_history(){
    ofstream Attendance("Market History for 301 N, 7 m, 2 s, 5000 runs.txt");
    for(int i = num_indicies_in_strategy; i < nonbinary_history.size(); i++) {
        Attendance << i << ", "
                   << nonbinary_history[i] << ", "
                   << history[i] << endl;
    }
}

void Experiment::write_minority_game_observables(int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS, int NUM_DIFF_MEMORY_LENGTHS) {
    //File Data Details
    ofstream file("High_Res predict Variance for Memory from 2 to 16, Pop from 101 to 701, and 1000 Iterations.txt");
    for (int a = 2; a <= NUM_DIFF_MEMORY_LENGTHS; a++) {
        int NUM_INDICES_IN_STRATEGY = a; //int(floor(pow(2, a) + .5));
        cout << "Just started " << NUM_INDICES_IN_STRATEGY << "th memory length run" << endl;
        for (int b = 1; b < NUM_DIFF_AGENT_POPS; b++) {
            int AGENT_POPULATION = (100*b)+1;
            Experiment environment{AGENT_POPULATION, 2, NUM_INDICES_IN_STRATEGY, 42};
            environment.run_minority_game(NUM_DAYS_AGENTS_PLAY);
            double Alpha = pow(2, double(NUM_INDICES_IN_STRATEGY)) / AGENT_POPULATION;
            double Variance = Analysis::variance(environment.nonbinary_history);
            double successRate = Analysis::successRate(environment.nonbinary_history, AGENT_POPULATION);
            double elementRange = ((double) Analysis::numberOfUniqueElements(environment.nonbinary_history) /
                                   pow(2, double(NUM_INDICES_IN_STRATEGY)));
            file << AGENT_POPULATION << ", "
                 << NUM_INDICES_IN_STRATEGY << ", "
                 << Alpha << ", "
                 << Variance << ", "
                 << Variance / AGENT_POPULATION << ", "
                 << successRate << ", "
                 << elementRange << endl;
        }
    }
}

void Experiment::write_memory_distribution() {
    vector<int> memory_counts;
    for (int i = 0; i < agent_count; i++) {
        for (int j = 0; j < strategies_per_agent; j++) {
            memory_counts.push_back(agents[i].strategies[j].num_indicies_in_strategy);
        }
    }
    vector<vector<int>> memory_frequencies = Analysis::frequencyHistogram(memory_counts);
    debug_print(memory_frequencies);

    ofstream memory_distribution("Memory Distribution.txt");
    for (int i = 0; i < memory_frequencies.size(); i++) {
        memory_distribution << memory_frequencies[i][0] << ", "
                            << memory_frequencies[i][1] << endl;
    }
}

