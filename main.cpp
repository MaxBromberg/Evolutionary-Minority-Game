#include "Evolutionary_Minority_Game.h"

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 7,
    RUN_TIME = 1000,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_POPULATIONS = 7, //101 to 701
    NUM_DIFF_MEMORY_LENGTHS = 4, //2 to 16
};

int main() {
    Experiment environment{ AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, NUM_INDICIES_IN_STRATEGY, RNG_SEED};
    //environment.run_minority_game(RUN_TIME);
    //environment.write_attendance_history();

    //hopeully all new constructions are accurate?
    environment.outputMinorityGameObservables(RUN_TIME, NUM_DIFF_POPULATIONS, NUM_DIFF_MEMORY_LENGTHS);
    return 0;
}
