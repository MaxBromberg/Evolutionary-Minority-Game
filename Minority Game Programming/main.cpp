#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include "Evolutionary_Minority_Game_Functions.h"
#include <fstream>

using namespace std;

//Header Declarations
enum {
    NUM_STRATEGIES_PER_AGENT = 2,
    NUM_DAYS_AGENTS_PLAY = 10000,
    MARKET_HISTORY_INIT_SEED = 123,
    RNG_RESOLUTION = 10000,
    NUM_DIFF_AGENT_POPS = 4, //Log10 range
    NUM_DIFF_MEMORY_LENGTHS = 5, //Log2 range
};

int main() {
    //File Data Details
    ofstream file("Variance for Memory from 2 to 32, Pop from 11 to 1001, and 1001 Iterations.txt");
    for (int a = 1; a <= NUM_DIFF_MEMORY_LENGTHS; a++) {
        int NUM_INDICES_IN_STRATEGY = 0;
        NUM_INDICES_IN_STRATEGY = int(floor(pow(2, a) + .5));
        cout << "Just started " << NUM_INDICES_IN_STRATEGY << "th memory length run" << endl;
        for (int b = 1; b < NUM_DIFF_AGENT_POPS; b++) {
            int AGENT_POPULATION = 0;
            for (int c = 0;
                 c < 2; c++) { //I know this a really ugly, can't think of a better implementation at the moment
                (c == 0) ? AGENT_POPULATION = int(floor(pow(10, b) + 0.5) + 1) : AGENT_POPULATION *= 5;
                if (AGENT_POPULATION > 2000) {
                    break;
                }

                //initialization
                auto strategy_scores = TwoDimensionalVector(AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, 0);
                auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(NUM_INDICES_IN_STRATEGY);
                auto market_history = MarketInitialization::marketHistoryGenerator(binary_history, AGENT_POPULATION);

                for (int i = 0; i < NUM_DAYS_AGENTS_PLAY; ++i) {
                    market_history.push_back(
                            StrategyManipulation::market_count(strategy_scores,
                                                               binary_history,
                                                               NUM_INDICES_IN_STRATEGY,
                                                               NUM_STRATEGIES_PER_AGENT));
                    StrategyManipulation::binaryHistoryUpdate(binary_history,
                                                              market_history.back()); //Could just be written out as the one line fct. it is.
                    strategy_scores = StrategyManipulation::strategyScoreUpdate(AGENT_POPULATION,
                                                                                NUM_STRATEGIES_PER_AGENT,
                                                                                NUM_INDICES_IN_STRATEGY,
                                                                                binary_history,
                                                                                strategy_scores);
                    if(i % 100 == 0 ) {
                        debugOutputMatrix(strategy_scores);
                        cout << "Strategy Score matrix after " << i << " days printed" << endl;
                    }
                }
                debugOutputVector(binary_history);

                vector<int> attendance = Analysis::attendance(market_history, AGENT_POPULATION);
                double Alpha = pow(2, double(NUM_INDICES_IN_STRATEGY)) / AGENT_POPULATION;
                double Variance = Analysis::variance(market_history);
                double literatureVariance = Analysis::literatureVariance(market_history);
                double successRate = Analysis::successRate(market_history, AGENT_POPULATION);

                file << AGENT_POPULATION << ", "
                     << NUM_INDICES_IN_STRATEGY << ", "
                     << Alpha << ", "
                     << Variance << ", "
                     << literatureVariance << ", "
                     << Variance / AGENT_POPULATION << ", "
                     << literatureVariance / AGENT_POPULATION << ", "
                     << successRate << endl;

                //reseting: though the redeclaration should do this too, right?
                strategy_scores.clear();
                binary_history.clear();
                market_history.clear();
            }
        }
    }
    return 0;
}