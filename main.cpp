#include "Evolutionary_Minority_Game.h"

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 3,
    RUN_TIME = 100,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
};

int main() {
    Experiment environment1{AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, NUM_INDICIES_IN_STRATEGY, RNG_SEED};
    environment1.run_minority_game(RUN_TIME);
    return 0;
}
