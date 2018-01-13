#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "Evolutionary_Minority_Game.h"
#include "analysis_utilities.h"

using namespace std;

void outputMinorityGameObservables(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS,
                                   int NUM_DIFF_MEMORY_LENGTHS) {
    //File Data Details
    ofstream file("Variance for Memory from 2 to 32, Pop from 11 to 1001, and 1000 Iterations.txt");
    for (int a = 1; a <= NUM_DIFF_MEMORY_LENGTHS; a++) {
        int NUM_INDICES_IN_STRATEGY = int(floor(pow(2, a) + .5));
        cout << "Just started " << NUM_INDICES_IN_STRATEGY << "th memory length run" << endl;
        for (int b = 1; b < NUM_DIFF_AGENT_POPS; b++) {
            int AGENT_POPULATION = 0;
            for (int c = 0;
                 c < 2; c++) { //I know this a really ugly, can't think of a better implementation at the moment
                if (c == 0) {
                    AGENT_POPULATION = int(floor(pow(10, b) + 0.5) + 1);
                } else {
                    AGENT_POPULATION *= 5;
                }
                if (AGENT_POPULATION > 2000) {
                    break;
                }
                Experiment::Experiment{AGENT_POPULATION, NUM_INDICES_IN_STRATEGY, a, 42};
                Experiment::run_minority_game(NUM_DAYS_AGENTS_PLAY);

                double Alpha = pow(2, double(NUM_INDICES_IN_STRATEGY)) / AGENT_POPULATION;
                double Variance = Analysis::variance(Experiment.nonbinary_history);
                double successRate = Analysis::successRate(Experiment.nonbinary_history, AGENT_POPULATION);
                double elementRange = ((double) Analysis::numberOfUniqueElements(Experiment.nonbinary_history) /
                                       Experiment.nonbinary_history.size());

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
}

