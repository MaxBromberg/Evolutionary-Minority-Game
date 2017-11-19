#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <pair>
#include <cassert>

using namespace std;

//Header Declarations
enum {
    NUM_INDICES_IN_STRATEGY = 4,
    AGENT_POPLUATION = 11,
    NUM_STRATEGIES_PER_AGENT = 2,
    NUM_DAYS_AGENTS_PLAY = 10,
    MARKET_HISTORY_INIT_SEED = 123,
};

class StrategyVector {
};

class BinaryManipulation {
public:
    //Member Functions
    static vector<int> binaryConverter(int NumIndicesInStrategy, unsigned int long long index); // perhaps it's be better not to have them in a class?
    static int reverseBinaryConverter(int NumIndicesInStrategy, vector<int> strategyDomain); // Just trying to organize correctly
};

struct StrategyManipulation {
    //there's no need to assign strategies initially, as they will be produced procedurally with the index serving as the RNG seed
    static void deployment (int NumIndicesInStrategy, int agentPopulation, int strategiesPerAgent);
    static uint8_t evaluateStrategy (int indexI, int indexJ, int strategiesPerAgent, int NumIndicesInStrategy, vector<int> marketHistory);
    static vector<vector<int>> strategyScoreInitialization(int agentPopulation, int strategiesPerAgent);
    static vector<vector<int>> strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy, vector<int> binaryMarketHistory);
};

struct MarketInitialization{
    static vector<int> binaryMarketHistoryGenerator(int NumIndicesInStrategy);
    static vector<int> marketHistoryGenerator(int NumIndicesInStrategy);
};

vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
vector<vector<int>> TwoDimensionalVector (int x_size, int y_size, int value);

int main(){
    auto strategy_scores = TwoDimensionalVector (AGENT_POPLUTAION, NUM_STRATEGIES_PER_AGENT, 0);
    auto binary_history = MarketInitialization::binaryMarketHistoryGenerator (NUM_INDICES_IN_STRATEGY);
    auto market_history = MarketInitialization::marketHistoryGenerator (binary_history, NUM_INDICES_IN_STRATEGY, AGENT_POPLUATION);

    int attendence;
    for(int i = 0; i < 10; ++i){
        StrategyManipulation.deployment(NumIndicesInStrategy, agentPopulation, strategiesPerAgent);
        StrategyManipulation.StrategyScoreUpdate(agentPopulation, strategiesPerAgent, NumIndicesInStrategy, binaryMarkethistory);
    }
    return 0;
}

// FILE : api.cpp
//MarketInitialization Function Declaration
// should also be operating on ref vales, not globals

vector<vector<int>> TwoDimensionalVector (int x_size, int y_size, int value) {
    std::vector<vector<int>> result {};
    result.resize (x_size, std::vector<int> {y_size, value});
}

vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0) {
    mt19937 generator {seed};
    uniform_int_distribution<int> bitDistribution {0, 1};

    vector<int> v {NumIndicesInStrategy};
    std::generate (v.begin(), v.end(), [&generator, true_value, false_value] () {return bitDistribution (generator) & true_value : false_value;});
    return v;
}

vector<int> MarketInitialization::binaryMarketHistoryGenerator (int NumIndicesInStrategy){
    return RandomBoolVector (NumIndicesInStrategy, MARKET_HISTORY_INIT_SEED, 1, -1);
}

vector<int> MarketInitialization::marketHistoryGenerator(const vector<int>& source, int NumIndicesInStrategy, int agentPopulation){
    mt19937 generator {seed};
    uniform_int_distribution<int> distribution {0, agentPopulation};

    vector<int> result {source.size()};
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
            int rand = distribution (generator);
            return i * rand;
            });
    return result;
}

vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy, vector<int> binaryMarketHistory){
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation.evaluateStrategy(i,j,strategiesPerAgent, NumIndicesInStrategy, binaryMarkethistory) == binaryMarkethistory.back()){
                strategyScores[i][j]--;
            strategyScores[i][j]++;
            }
        }
    }
    return strategyScores;
}

int StrategyManipulation::market_count (const vector<vector<int>>& strategy_scores, const vector<int>& binary_history, int NumIndicesInStrategy){
    //Finds best preforming strategy and uses it.
    int marketCount = 0;
    for(int agent_idx = 0; agent_idx < strategy_scores.size(); ++agent_idx) {
        auto& agent_strategies = strategy_scores[i];
        auto index_of_best_strategy = std::max_element (agent_strategies.begin(), agent_strategies.end()) - agent_strategies.begin();
        marketCount += StrategyManipulation.evaluateStrategy (agent_idx, index_of_best_strategy, NumIndicesInStrategy, binary_history);
    }

    return marketCount;
    /*
    markethistory.push_back(marketCount);
    binaryMarkethistory.push_back(marketCount > 0 ? 1 : -1); //here the 50% threshold is hardcoded
    //I just want the latest elements, so need to continually push back the whole history...
    */
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

int StrategyManipulation::evaluateStrategy (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy, const vector<int>& binaryMarketHistory){
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    //I'm taking enum consts. as arguments as I expect to use others later
    int convertedBinaryStrategy = BinaryManipulation::reverseBinaryConverter(NumIndicesInStrategy, binaryMarketHistory);
    int strategyIndex = BinaryVectorLastNToStrategyIndex (binaryMarketHistory, NumIndicesInStrategy);
    int indexSeed = (agent_idx *strategiesPerAgent) + index_of_best_strategy; //converts to unique 1d array index

    //produce a unique number to seed the bit generator with dependence on both the above
    uniform_int_distribution<uint64_t> solutionDomain(0, pow(2, pow(2, NumIndicesInStrategy)));
    uniform_int_distribution<uint64_t> seedRandomizer(0, pow(2, pow(2, NumIndicesInStrategy))-pow(2,NumIndicesInStrategy)); //the subtraction is to ensure that the
    // solution domain is not exceeded by the subsequent addition of converted binary string, which is as large as 2^m
    unsigned long long int randomizedSeedInput = seedRandomizer(indexSeed)
    int strategySolutionSeed = solutionDomain(randomizedSeedInput+convertedBinaryStrategy) //domain of 2^2^m,

    uniform_int_distribution<uint8_t> overkillBitGenerator(0, 1000);
    uint8_t bitToBe = overkillBitGenerator(strategySolutionSeed); //deterministically produces bit from strategy domain element
    bitToBe = (bitToBe < 500 ? 1 : -1); //cast will happen regardless, and will fit (highest unsigned int 1024>1000, no?)

    return (bitToBe);
}

//BinaryManipulation Function Declaration
vector<int> BinaryManipulation::binaryConverter(int NumIndicesInStrategy, unsigned int long long index){ //converts number and NumIndicesInStrategy to strategy domain sequence
    bitset<64> binary (index); //the 64 is the length converted to binary, the index the number. length must be > NumIndicesInStrategy
    vector<int> strategyDomain;
    for(int i = 0; i < NumIndicesInStrategy; i++) {
        if(binary[i] == 0){ //reads elements of the binary conversion, from largest power of 2 to smallest
            strategyDomain.push_back(-1); //cannot simply change 0 -> -1, as I'm using pushback (though I could use an array, and return an array pointer
        }else{
            strategyDomain.push_back(1);
        }
    }
    return strategyDomain;
}

int BinaryManipulation::reverseBinaryConverter(int NumIndicesInStrategy, vector<int> strategyDomain) {
//the point is to create a strategy on the fly based only on the index corresponding to its binary  in that form.
    int strategyIndex = 0;
    for(int i = 0; i < NumIndicesInStrategy; i++) {
        if(strategyDomain[i] == 1){
            strategyIndex+= 1 << i;
        }
    }
    return strategyIndex;
}
