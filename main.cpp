#include "Evolutionary_Minority_Game.h"

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 3,
    RUN_TIME = 1000,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
};

int main() {
    Experiment{AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, NUM_INDICIES_IN_STRATEGY, RNG_SEED};
    Experiment::run_minority_game(RUN_TIME);
    return 0;
}
