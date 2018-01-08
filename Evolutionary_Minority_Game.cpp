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

enum {
    MARKET_HISTORY_INIT_SEED = 123,
    RNG_RESOLUTION = 10000
};

using namespace std;

vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

template <typename Iter>
static int BinaryArrayToStrategyIndex (Iter begin, Iter end) {
    assert (end - begin <= 32);
    int output = 0;
    for (auto iter = begin; iter != end; ++iter) {
        if (*iter == 1) output += 1 << iter - begin;
    }
    return output;
}

int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n) {
    return BinaryArrayToStrategyIndex (v.end() - n, v.end());
}

vector<vector<int>> TwoDimensionalVector (unsigned int x_size, int y_size, int value) { //Generates a 2D vector and fills it with value
    vector<vector<int>> result;
    result.resize (x_size, vector<int> (y_size, value));
    return (result);
}

vector<int> MarketInitialization::binaryMarketHistoryGenerator (int NumIndicesInStrategy){ //fills a vector with +/-1
    return RandomBoolVector (NumIndicesInStrategy, MARKET_HISTORY_INIT_SEED, 1, -1);
}

vector<int> MarketInitialization::marketHistoryGenerator(const vector<int>& source, int agentPopulation){
    mt19937 generator {MARKET_HISTORY_INIT_SEED};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop)
//Strategy Manipulation Function Definitions
vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int actual_value, int NumIndicesInStrategy,
                                                              const vector<int>& binaryMarketHistory, vector<vector<int>> strategy_scores) {
    for(int i = 0; i < strategy_scores.size(); ++i){
        for(int j = 0; j < strategy_scores[i].size(); ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation::evaluateStrategy(i,j, NumIndicesInStrategy, strategy_scores[i].size(), binaryMarketHistory) == actual_value) { //
                //Strategy scores are not quantitatively updated, only qualitatively. Can readily be changed to accommodate quantitative results.
                strategy_scores[i][j]++;
            }else{
                strategy_scores[i][j]--;
            }
        }
    }
    return strategy_scores;
}

vector<vector<int>> StrategyManipulation::Alternate_strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
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

int StrategyManipulation::market_count (const vector<vector<int>>& strategy_scores, const vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent){
    //Finds best preforming strategy for each agent and uses it.
    int marketCount = 0;
    for(int agent_idx = 0; agent_idx < strategy_scores.size(); ++agent_idx) {
        auto& agent_strategies = strategy_scores[agent_idx];
        auto index_of_best_strategy = std::max_element (agent_strategies.begin(), agent_strategies.end()) - agent_strategies.begin();
        marketCount += StrategyManipulation::evaluateStrategy (agent_idx, index_of_best_strategy, NumIndicesInStrategy, strategiesPerAgent, binary_history);
        }
    assert(abs(marketCount) <= strategy_scores.size());
    return marketCount;
}

int StrategyManipulation::evaluateStrategy (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                            int strategiesPerAgent, const vector<int>& binaryMarketHistory){
    assert(index_of_best_strategy < strategiesPerAgent);
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    //
    int64_t input = ((agent_idx * strategiesPerAgent) + index_of_best_strategy) * BinaryVectorLastNToStrategyIndex (binaryMarketHistory, NumIndicesInStrategy);

    mt19937 generator (input);
    uniform_int_distribution<int> bitdistribution(0,1);

    /*
    mt19937 indexDependentGenerator ((agent_idx*strategiesPerAgent) + index_of_best_strategy); //leads to independent numbers for each call, which is problematic
    mt19937 metaStrategyIndexDependentGenerator (BinaryVectorLastNToStrategyIndex (binaryMarketHistory, NumIndicesInStrategy));
    uniform_int_distribution<int> bitdistribution(0,RNG_RESOLUTION);
    */

    return bitdistribution(generator) ? 1 : -1;
}

void StrategyManipulation::binaryHistoryUpdate(vector<int>& binaryHistory, int marketCount){
    binaryHistory.push_back((marketCount > 0) ? -1 : 1); //outputs the minority; i.e. the winning group
}

double Analysis::literatureVariance(const vector<int>& obv){
    vector<double> sqObvMinusObvMean(obv.size()); //implement generate
    double variance;
    double obvMean = Analysis::mean(obv);
    for(int i = 0; i < sqObvMinusObvMean.size(); i++){
        sqObvMinusObvMean[i] = (obv[i]-obvMean)*(obv[i]-obvMean);
    }
    variance = Analysis::mean(sqObvMinusObvMean);
    return variance;
}

double Analysis::successRate(const vector<int>& obv, int agentPop){
    double successRate = 0;
    for(auto e : obv){
        successRate += (agentPop - abs(e))/(double)(2*agentPop);
    }
    return successRate/obv.size();
}

vector<int> Analysis::attendance(const vector<int>& obv, int agentPop){
    vector<int> attendance;
    for(int i = 0; i < obv.size(); i++){
        attendance.push_back((agentPop/2)-obv[i]);
    }
    return attendance;
}//yields abs. attendance vector
