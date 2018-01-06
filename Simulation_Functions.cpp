#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "Evolutionary_Minority_Game.h"

using namespace std;

void outputMinorityGameObservables(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS,
                                   int NUM_DIFF_MEMORY_LENGTHS) {
    //File Data Details
    ofstream file("Variance for Memory from 2 to 32, Pop from 11 to 1001, and 1000 Iterations.txt");
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
                assert(AGENT_POPULATION % 2 == 0);

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
                    /*
                    if (NUM_INDICES_IN_STRATEGY >= 8){
                        if (i % (NUM_DAYS_AGENTS_PLAY/4) == 0) {
                            cout << "Strategy Score matrix after " << i << " days printed below" << endl;
                            debug_print(strategy_scores);
                        }
                    }
                     */
                }
                //cout<<"Binary History = "<<endl;
                //debug_print(binary_history);

                vector<int> attendance = Analysis::attendance(market_history, AGENT_POPULATION);
                double Alpha = pow(2, double(NUM_INDICES_IN_STRATEGY)) / AGENT_POPULATION;
                double Variance = Analysis::variance(market_history);
                double successRate = Analysis::successRate(market_history, AGENT_POPULATION);
                double elementRange = ((double) Analysis::numberOfUniqueElements(market_history) /
                                       market_history.size());

                file << AGENT_POPULATION << ", "
                     << NUM_INDICES_IN_STRATEGY << ", "
                     << Alpha << ", "
                     << Variance << ", "
                     << Variance / AGENT_POPULATION << ", "
                     << successRate << ", "
                     << elementRange << endl;

                //reseting: though the redeclaration should do this too, right?
                strategy_scores.clear();
                binary_history.clear();
                market_history.clear();
            }
        }
    }
}


void output_Minority_Game_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                        int NUM_INDICES_IN_STRATEGY) {
    ofstream file("Market History for m=15, s=2, N=301, 10000 Iterations.txt");
    assert(AGENT_POPULATION % 2 == 1);
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
        //if(i > NUM_DAYS_AGENTS_PLAY - 500) { //gives last 500 data points
            file << i << ", "
                 << market_history[i] << ", "
                 << AGENT_POPULATION - abs(market_history[i]) << endl;
        //}
    }
}

//Uses the g(x) = A(t) reward fct in str.score update, rather than g(x) = sign(A(t))
void alt_Strategy_Score_Update_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY) {
    ofstream file("Market History Test for m=3, s=2, N=301, 1000 Iterations.txt");
    //initialization
    auto strategy_scores = TwoDimensionalVector(AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, 0);
    auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(NUM_INDICES_IN_STRATEGY);
    auto market_history = MarketInitialization::marketHistoryGenerator(binary_history, AGENT_POPULATION);

    for (int i = 0; i < NUM_DAYS_AGENTS_PLAY; ++i) {
        int market_Count;
        market_history.push_back(
                StrategyManipulation::market_count(strategy_scores,
                                  binary_history,
                                  NUM_INDICES_IN_STRATEGY,
                                  NUM_STRATEGIES_PER_AGENT));
        market_Count = market_history.back();
        StrategyManipulation::binaryHistoryUpdate(binary_history,
                                 market_history.back()); //Could just be written out as the one line fct. it is.
        strategy_scores = StrategyManipulation::Alternate_strategyScoreUpdate(AGENT_POPULATION,
                                                   NUM_STRATEGIES_PER_AGENT,
                                                   NUM_INDICES_IN_STRATEGY,
                                                   binary_history,
                                                   strategy_scores,
                                                   market_Count);
        if(i>500) {
            file << i << ", "
                 << market_history[i] << ", "
                 << (int) (AGENT_POPULATION / 2) + (market_history[i]) << endl;
        }
    }
}

void strategy_test_run(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY) {
    auto strategy_scores = TwoDimensionalVector(AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, 0);
    auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(NUM_INDICES_IN_STRATEGY);
    auto market_history = MarketInitialization::marketHistoryGenerator(binary_history, AGENT_POPULATION);

    cout << "Strategy_Scores.size = " << strategy_scores.size() << endl;
    cout << "and strategy_scores[1].size = " << strategy_scores[1].size() << endl;

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
//Where we put stuff ot test:
        ofstream file("Strategy Scores.txt");
        if(i % 100 == 0){
            cout << "Strategy Scores for " << i << "th run printed below" << endl;
            debug_print(strategy_scores); //Perhaps not fully printing because it completes the 'tests' before it finishes printing?

            for (int j = 0; j < strategy_scores.size(); j++) {
                file << strategy_scores[j][0] << ", "
                     << strategy_scores[j][1]
                     << endl; //did this hoaxy version, as neither debug print nor double for loops works
                //this also doesn't work, perhaps for the same reason as the above debug print doesn't.
            }
        }
    }
}
