#include "Evolutionary_Minority_Game.h"
#include "Agents and Evolution Strategies.h"

enum {
    PREHISTORY_LENGTH = 32, //I just make it the maximum possible (as int is 4 bytes) to prevent unnecessary edits
    MAX_AGENT_POP = 5001,
    MIN_AGENT_POP = 101,
    INIT_AGENT_POP = 101,
    AGENT_POP_INTERVAL = 100,
    MAX_MEMORY = 16,
    MIN_MEMORY = 2,
    MEMORY_INTERVAL = 1,
    RUN_TIME = 20000 + PREHISTORY_LENGTH, //+32 to account for prehistory
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_STRATEGY_SETS = 10,
    STRATEGY_SET_INCREMENTOR = 50000,
    ITERATED_EVOLUTIONARY_LENGTH = 10, //Must be greater than 1
    STARTING_EVOLUTIONARY_LENGTH = 10, //Must be greater than 1
    NUM_EVOL_LENGTHS = 5, //Greater than 1
    MAX_EVOL_MEMORY = 20,
    MIN_EVOL_MEMORY = 1,
    MAX_NUM_STRATEGIES = 20,
    MIN_NUM_STRATEGIES = 2,
    INIT_MEMORY = 3,
    NUM_MEMORY_ITERATIONS = 4,
    DELTA_FINAL_MULTIPLE = 30,
};

double memory_delta = 0.025;
double strategy_delta = 0.025;
double breeding_delta = 0.0125;

int main() {
//    write_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED,
//                         NUM_DIFF_STRATEGY_SETS, MAX_AGENT_POP, MIN_AGENT_POP,
//                         AGENT_POP_INTERVAL, MAX_MEMORY, MIN_MEMORY, MEMORY_INTERVAL);
//    write_thermal_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED,
//                                    NUM_DIFF_STRATEGY_SETS, MAX_AGENT_POP, MIN_AGENT_POP,
//                                    AGENT_POP_INTERVAL, MAX_MEMORY, MIN_MEMORY, MEMORY_INTERVAL);
//    ExperimentState experiment {basic_pre_history(PREHISTORY_LENGTH, RNG_SEED, MAX_AGENT_POP),
//                                std::unique_ptr<EvolutionStrategy> {new Darwinism {memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES}}, darwinian_agents(MAX_AGENT_POP, NUM_STRATEGIES_PER_AGENT, INIT_MEMORY, STRATEGY_SET_INCREMENTOR, STARTING_EVOLUTIONARY_LENGTH)};

//    experiment.simulate (RUN_TIME);
//    experiment.write_last_n_market_history(RUN_TIME);
//    experiment.write_memory_frequencies(RUN_TIME);
//    experiment.write_mean_memories(RUN_TIME);

    //**************************************Evolutionary Methods*********************************************
//    write_memory_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, MAX_AGENT_POP, INIT_MEMORY, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, NUM_EVOL_LENGTHS, ITERATED_EVOLUTIONARY_LENGTH,
//                                             memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);
//    thermal_write_memory_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, MAX_AGENT_POP, INIT_MEMORY, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, NUM_EVOL_LENGTHS, ITERATED_EVOLUTIONARY_LENGTH,
//                                             memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);

//    write_strategy_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, MAX_AGENT_POP, INIT_MEMORY, NUM_MEMORY_ITERATIONS, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, ITERATED_EVOLUTIONARY_LENGTH,
//                                               NUM_EVOL_LENGTHS, memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);
//    thermal_write_strategy_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, MAX_AGENT_POP, INIT_MEMORY, NUM_MEMORY_ITERATIONS, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, ITERATED_EVOLUTIONARY_LENGTH,
//                                                       NUM_EVOL_LENGTHS, memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);

    write_population_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, INIT_AGENT_POP, INIT_MEMORY, NUM_MEMORY_ITERATIONS, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, ITERATED_EVOLUTIONARY_LENGTH,
                                               NUM_EVOL_LENGTHS, memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);
    thermal_write_population_evolutionary_mg_observables(RUN_TIME, NUM_STRATEGIES_PER_AGENT, RNG_SEED, INIT_AGENT_POP, INIT_MEMORY, NUM_MEMORY_ITERATIONS, DELTA_FINAL_MULTIPLE, STARTING_EVOLUTIONARY_LENGTH, ITERATED_EVOLUTIONARY_LENGTH,
                                                 NUM_EVOL_LENGTHS, memory_delta, strategy_delta, breeding_delta, MAX_EVOL_MEMORY, MIN_EVOL_MEMORY, MAX_NUM_STRATEGIES, MIN_NUM_STRATEGIES, MAX_AGENT_POP, MIN_AGENT_POP);
    return 0;
}