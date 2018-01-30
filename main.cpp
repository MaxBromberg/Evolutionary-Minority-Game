#include "Evolutionary_Minority_Game.h"
#include <random>
#include "Agents and Evolution Strategies.h"

enum {
    MAX_AGENT_POP = 701,
    MIN_AGENT_POP = 101,
    AGENT_POP_INTERVAL = 100,
    MAX_MEMORY = 16,
    MIN_MEMORY = 2,
    MEMORY_INTERVAL = 1,
    RUN_TIME = 10000,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_STRATEGY_SETS = 25,
};

/*
int convert (double x) {
    return (int) x;
}
int foo (std::function<int(double)> f) {
    return f (2.0);
}
*/

int main() {
    ExperimentState experiment {basic_pre_history(32, RNG_SEED, MIN_AGENT_POP), std::unique_ptr<EvolutionStrategy> {new Creationism {}}, alpha_agents(MIN_AGENT_POP, NUM_STRATEGIES_PER_AGENT, MIN_MEMORY, 1)};
    experiment.write_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, NUM_DIFF_STRATEGY_SETS, MAX_AGENT_POP, MIN_AGENT_POP, AGENT_POP_INTERVAL, MAX_MEMORY, MIN_MEMORY, MEMORY_INTERVAL);
    //experiment.simulate (RUN_TIME);
    //experiment.write_last_n_market_history(500);
    //experiment.print();
    return 0;
}