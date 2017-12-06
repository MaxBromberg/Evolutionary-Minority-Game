#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
//#include <pair>
#include <cassert>

using namespace std;

//Header Declarations
enum {
    NUM_INDICES_IN_STRATEGY = 4,
    AGENT_POPULATION = 11,
    NUM_STRATEGIES_PER_AGENT = 2,
    NUM_DAYS_AGENTS_PLAY = 10,
    MARKET_HISTORY_INIT_SEED = 123,
    RNG_RESOLUTION = 10000
};

struct StrategyManipulation {
    //there's no need to assign strategies initially, as they will be produced procedurally with the index serving as the RNG seed
    static vector<vector<int>> TwoDimensionalVector (int x_size, int y_size, int value);

    static vector<vector<int>> strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                                   const vector<int>& binaryMarketHistory, vector<vector<int>> strategy_scores);

    static int market_count (const vector<vector<int>>& strategy_scores, const vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent);

    static int evaluateStrategy (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                            int strategiesPerAgent, const vector<int>& binaryMarketHistory);
};

struct MarketInitialization{
    static vector<int> binaryMarketHistoryGenerator(int NumIndicesInStrategy);
    static vector<int> marketHistoryGenerator(const vector<int>& source, int agentPopulation);
};

    //general function declarations
    vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
    int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);

int main(){
    //initialization
    auto strategy_scores = StrategyManipulation::TwoDimensionalVector (AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT, 0);
    auto binary_history = MarketInitialization::binaryMarketHistoryGenerator (NUM_INDICES_IN_STRATEGY);
    auto market_history = MarketInitialization::marketHistoryGenerator (binary_history, AGENT_POPULATION);

    
    for(int i = 0; i < NUM_DAYS_AGENTS_PLAY; ++i){
        market_history.push_back(StrategyManipulation::market_count (strategy_scores, binary_history,
                                                                     NUM_INDICES_IN_STRATEGY, NUM_STRATEGIES_PER_AGENT));
        strategy_scores = StrategyManipulation::strategyScoreUpdate(AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT,
                                                  NUM_INDICES_IN_STRATEGY, binary_history, strategy_scores);
    }
    return 0;
}

// FILE : api.cpp
//MarketInitialization Function Declaration
vector<vector<int>> StrategyManipulation::TwoDimensionalVector (int x_size, int y_size, int value) { //Generates a empty vector of corresponding size
    std::vector<vector<int>> result {};
    result.resize (x_size, std::vector<int> {y_size, value});
}
// Not working for some reason, couldn't get it to work myself...
vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);

    vector<int> v {size};
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

/*
vector<int> RandomBoolVector (int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    mt19937 generator (seed);
    uniform_int_distribution<int> bitDistribution (0, 1);
    vector<int> v {size};
    for(int i = 0; i < v.size(); i++){
        v.push_back(bitDistribution(generator));
        if(v[i] == 0)
            v[i] = true_value;
        v[i] = false_value;
    return v;
}
*/
vector<int> MarketInitialization::binaryMarketHistoryGenerator (int NumIndicesInStrategy){ //fills a vector with +/-1
    return RandomBoolVector (NumIndicesInStrategy, MARKET_HISTORY_INIT_SEED, 1, -1);
}

vector<int> MarketInitialization::marketHistoryGenerator(const vector<int>& source, int agentPopulation){
    mt19937 generator {MARKET_HISTORY_INIT_SEED};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result {source.size()};
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
            int rand = distribution (generator);
            return i * rand;
            });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop)

vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                                              const vector<int>& binaryMarketHistory, vector<vector<int>> strategy_scores){
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation::evaluateStrategy(i,j,strategiesPerAgent, NumIndicesInStrategy, binaryMarketHistory) == binaryMarketHistory.back()){
                strategy_scores[i][j]--;
            strategy_scores[i][j]++;
            }
        }
    }
    return strategy_scores;
}

int StrategyManipulation::market_count (const vector<vector<int>>& strategy_scores, const vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent){
    //Finds best preforming strategy and uses it.
    int marketCount = 0;
    for(int agent_idx = 0; agent_idx < strategy_scores.size(); ++agent_idx) {
        auto& agent_strategies = strategy_scores[agent_idx];
        auto index_of_best_strategy = std::max_element (agent_strategies.begin(), agent_strategies.end()) - agent_strategies.begin();
        marketCount += StrategyManipulation::evaluateStrategy (agent_idx, index_of_best_strategy, NumIndicesInStrategy, strategiesPerAgent, binary_history);
    }
    return marketCount;
}

int StrategyManipulation::evaluateStrategy (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                            int strategiesPerAgent, const vector<int>& binaryMarketHistory){
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    //I'm taking enum consts. as arguments as I expect to use others later
    auto metaStrategyIndex = BinaryVectorLastNToStrategyIndex (binaryMarketHistory, NumIndicesInStrategy);
    auto indexSeed = (agent_idx*strategiesPerAgent) + index_of_best_strategy; //converts to unique 1d array index

    //produce a unique number to seed the bit generator with dependence on both the above (index seed and strategy index)
    mt19937 indexDependentGenerator (indexSeed);
    mt19937 metaStrategyindexDependentGenerator (metaStrategyIndex);
    uniform_int_distribution<int> solutionDomain(0,RNG_RESOLUTION);
    uniform_int_distribution<int> seedRandomizer(0,RNG_RESOLUTION);
    int randomizedSeedInput = seedRandomizer(indexDependentGenerator);
    int strategySolutionSeed = solutionDomain(metaStrategyindexDependentGenerator); //domain of 2^2^m
    int bitToBe = randomizedSeedInput+strategySolutionSeed; //deterministically produces bit from strategy domain element
    bitToBe = bitToBe < RNG_RESOLUTION ? 1 : -1; //cast will happen regardless, and will fit (highest unsigned int 1024>1000, no?)
    //Ideally takes less time than simply recalling from memory?
    return (bitToBe);
}

template <typename Iter>
int BinaryArrayToStrategyIndex (Iter begin, Iter end) {
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