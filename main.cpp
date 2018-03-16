#include "Evolutionary_Minority_Game.h"
#include "Agents and Evolution Strategies.h"

enum {
    PREHISTORY_LENGTH = 32, //I just make it the maximum possible (as int is 4 bytes) to prevent unnecessary edits
    MAX_AGENT_POP = 1001,
    MIN_AGENT_POP = 101,
    AGENT_POP_INTERVAL = 100,
    MAX_MEMORY = 16,
    MIN_MEMORY = 2,
    MEMORY_INTERVAL = 1,
    RUN_TIME = 5000 + PREHISTORY_LENGTH, //+32 to account for prehistory
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_STRATEGY_SETS = 10,
    STRATEGY_SET_INCREMENTOR = 50000,
    EVOLUTIONARY_LENGTH = 20, //Must be greater than 1
};
double Evolutionary_win_threshold = 0.6;
double Evolutionary_lose_threshold = 1-Evolutionary_win_threshold;

/*
int convert (double x) {
    return (int) x;
}
int foo (std::function<int(double)> f) {
    return f (2.0);
}
*/

int main() {
    /*
    write_thermal_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED,
                                    NUM_DIFF_STRATEGY_SETS, MAX_AGENT_POP, MIN_AGENT_POP,
                                    AGENT_POP_INTERVAL, MAX_MEMORY, MIN_MEMORY, MEMORY_INTERVAL);
    */
    ExperimentState experiment {basic_pre_history(PREHISTORY_LENGTH, RNG_SEED, MIN_AGENT_POP),
                                std::unique_ptr<EvolutionStrategy> {new Darwinism {Evolutionary_win_threshold, Evolutionary_lose_threshold}}, darwinian_agents(MAX_AGENT_POP, NUM_STRATEGIES_PER_AGENT, 3, STRATEGY_SET_INCREMENTOR, EVOLUTIONARY_LENGTH)};
    experiment.simulate (RUN_TIME);
    //experiment.print();
    experiment.write_last_n_market_history(RUN_TIME);
    experiment.write_memory_frequencies(RUN_TIME);
    return 0;
}