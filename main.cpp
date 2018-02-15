#include "Evolutionary_Minority_Game.h"

enum {
    MAX_AGENT_POP = 1001,
    MIN_AGENT_POP = 101,
    AGENT_POP_INTERVAL = 100,
    MAX_MEMORY = 16,
    MIN_MEMORY = 2,
    MEMORY_INTERVAL = 1,
    RUN_TIME = 1032, //+32 to account for prehistory
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_STRATEGY_SETS = 10,
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
    write_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED,
                                    NUM_DIFF_STRATEGY_SETS, MAX_AGENT_POP, MIN_AGENT_POP,
                                    AGENT_POP_INTERVAL, MAX_MEMORY, MIN_MEMORY, MEMORY_INTERVAL);

    //ExperimentState experiment {basic_pre_history(32, RNG_SEED, MIN_AGENT_POP), std::unique_ptr<EvolutionStrategy> {new Creationism {}}, random_agents(MAX_AGENT_POP, 10000)};
    //experiment.simulate (RUN_TIME);
    //experiment.write_last_n_market_history(5000);
    //experiment.print();
    //write_market_histories(10000, RNG_SEED, MIN_AGENT_POP, MAX_AGENT_POP, AGENT_POP_INTERVAL, RUN_TIME);
    return 0;
}