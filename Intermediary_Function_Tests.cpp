#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "Evolutionary_Minority_Game.h"
#include "Intermediary_Function_Tests.h"

enum {
    MARKET_HISTORY_INIT_SEED = 123,
    RNG_RESOLUTION = 10000
};

using namespace std;

vector<vector<int>> strategyScoreUpdate_test(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                                              const vector<int>& binaryMarketHistory, vector<vector<int>> strategy_scores, int market_Count) {
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation::evaluateStrategy(i,j, NumIndicesInStrategy, strategiesPerAgent, binaryMarketHistory) == binaryMarketHistory.back()) {
                //Strategy scores are not quantitatively updated, only qualitatively. Can readily be changed to accommodate quantitative results, but no
                strategy_scores[i][j]-=market_Count;
            }else{
                strategy_scores[i][j]+=market_Count;
            }
        }
    }
    return strategy_scores;
}

int market_count_test (const vector<vector<int>>& strategy_scores, const vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent){
    //Finds best preforming strategy for each agent and uses it.
    int marketCount = 0;
        for(int agent_idx = 0; agent_idx < strategy_scores.size(); ++agent_idx) {
        auto& agent_strategies = strategy_scores[agent_idx];
        auto index_of_best_strategy = std::max_element (agent_strategies.begin(), agent_strategies.end()) - agent_strategies.begin();
        marketCount += evaluateStrategy_test (agent_idx, index_of_best_strategy, NumIndicesInStrategy, strategiesPerAgent, binary_history);
    }
    return marketCount;
}

int evaluateStrategy_test (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                            int strategiesPerAgent, const vector<int>& binaryMarketHistory){
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    assert(index_of_best_strategy <= strategiesPerAgent);
    auto metaStrategyIndex = BinaryVectorLastNToStrategyIndex (binaryMarketHistory, NumIndicesInStrategy);
    // converts binary history unique number (base ten representation)
    auto indexSeed = (agent_idx*strategiesPerAgent) + index_of_best_strategy;
    //converts to unique 1d array index

    //produce a unique number to seed the bit generator with dependence on both the above (index seed and strategy index)
    mt19937 indexDependentGenerator (indexSeed);
    mt19937 metaStrategyIndexDependentGenerator (metaStrategyIndex);
    uniform_int_distribution<int> bitdistribution(0,RNG_RESOLUTION);

    int randomizedSeedInput = bitdistribution(indexDependentGenerator);
    int strategySolutionSeed = bitdistribution(metaStrategyIndexDependentGenerator);
    //This is here for conceptual clarity, but should be replaced with the direct value without local var instantiation
    int bitToBe = randomizedSeedInput+strategySolutionSeed; //deterministically produces bit from strategy domain
    bitToBe = bitToBe < RNG_RESOLUTION ? 1 : -1;
    //Ideally takes less time than simply recalling from memory
    return (bitToBe);
}

void binaryHistoryUpdate_test(vector<int>& binaryHistory, int marketCount){
    binaryHistory.push_back((marketCount > 0) ? 1 : -1);
}

void test_Minority_Game_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY) {
    ofstream file("Market History Test for m=3, s=2, N=301, 1000 Iterations.txt");
    //initialization
    auto strategy_scores = TwoDimensionalVector(AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, 0);
    auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(NUM_INDICES_IN_STRATEGY);
    auto market_history = MarketInitialization::marketHistoryGenerator(binary_history, AGENT_POPULATION);

    for (int i = 0; i < NUM_DAYS_AGENTS_PLAY; ++i) {
        int market_Count;
        market_history.push_back(
                market_count_test(strategy_scores,
                                  binary_history,
                                  NUM_INDICES_IN_STRATEGY,
                                  NUM_STRATEGIES_PER_AGENT));
        market_Count = market_history.back(); //Here it's faster to recall from memory than do the *entire* calculation
        binaryHistoryUpdate_test(binary_history,
                                 market_history.back()); //Could just be written out as the one line fct. it is.
        strategy_scores = strategyScoreUpdate_test(AGENT_POPULATION,
                                                   NUM_STRATEGIES_PER_AGENT,
                                                   NUM_INDICES_IN_STRATEGY,
                                                   binary_history,
                                                   strategy_scores,
                                                   market_Count);
        if(i > NUM_DAYS_AGENTS_PLAY - 500) {
            file << i << ", "
                 << market_history[i] << ", "
                 << (int) (AGENT_POPULATION / 2) + (market_history[i]) << endl;
        }
    }
}

vector<int> random_Generator_Test(int min, int max, unsigned int number_of_elements){
    vector<int> randomVector;
    for(int i = 0; i < number_of_elements; i++){
        mt19937 gen (i);
        uniform_int_distribution<int> dist (min, max);
        randomVector.push_back(dist(gen));
    }
    return randomVector;
}
