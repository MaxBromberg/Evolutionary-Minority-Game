#include "Evolutionary_Minority_Game.h"

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 3,
    RUN_TIME = 1000,
    NUM_STRATEGIES_PER_AGENT = 2,
};

int main() {
    //outputMinorityGameObservables(2, 1000, 4, 5);
    //output_Minority_Game_Attendance_History(2, 10000, 301, 15); //strategies per agent, run length, agent pop, memory length
    strategy_test_run (NUM_STRATEGIES_PER_AGENT, RUN_TIME, AGENT_POPULATION, NUM_INDICIES_IN_STRATEGY);
    return 0;
}