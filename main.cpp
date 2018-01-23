#include "Evolutionary_Minority_Game.h"

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 7,
    RUN_TIME = 10000+NUM_INDICIES_IN_STRATEGY,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_POPULATIONS = 8, //101 to 701
    NUM_DIFF_MEMORY_LENGTHS = 16 , //2 to 16
    AGENTS_IDENTIFIER = 0,
    NUM_DIFF_STRATEGY_SETS = 10,
    EVOLUTION_MEMORY = 25,
};

int main() {
    Experiment environment{AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT,
                           NUM_INDICIES_IN_STRATEGY, AGENTS_IDENTIFIER,
                           EVOLUTION_MEMORY, RNG_SEED};
    environment.run_minority_game(RUN_TIME);
    environment.write_attendance_history();
    environment.write_memory_distribution();
    /*
    environment.write_minority_game_observables(RUN_TIME, NUM_DIFF_POPULATIONS,
                                                NUM_DIFF_MEMORY_LENGTHS, NUM_STRATEGIES_PER_AGENT,
                                                EVOLUTION_MEMORY, NUM_DIFF_STRATEGY_SETS);
                                                */
    return 0;
}