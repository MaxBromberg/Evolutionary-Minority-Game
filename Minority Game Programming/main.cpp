#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

//Header Declarations
enum constants{
    memory = 4,
    agentPopulation = 10,
    strategiesPerAgent = 2,
    marketUpdates = 10, // number of days agents play
};

class BinaryManipulation{
    public:

    //Member Functions
    static vector<int> binaryConverter(int memory, unsigned int long long index); // perhaps it's be better not to have them in a class?
    static int reverseBinaryConverter(int memory, vector<int> strategyDomain); // Just trying to organize correctly

};

class StrategyManipulation{
public:
    //Member Functions
    //there's no need to assign strategies initially, as they will be produced procedurally with the index serving as the RNG seed
    void StrategyManipulation::deployment(int memory, int agentPopulation, int strategiesPerAgent);
    uint8_t StrategyManipulation::evaluateStrategy(int indexI, int indexJ, int strategiesPerAgent, int memory, vector<int> marketHistory);
    vector<vector<int>> StrategyManipulation::strategyScoreInitialization(int agentPopulation, int strategiesPerAgent);
    vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int memory, vector<int> binaryMarketHistory);
};

class MarketInitialization{
public:
    //Member Functions
    vector<int> MarketInitialization::binaryMarketHistoryGenerator(int memory);
    vector<int> MarketInitialization::marketHistoryGenerator(int memory);
};

//Global Declarations:
int marketCount = 0; //should be replaced with a pointer to this value, but not sure how I can ensure not to over write it
vector<int> marketHistory;
vector<int> binaryMarkethistory;
vector<vector<int>> strategyScores; //Obviously needs to be in called by pointer

int main(){
    StrategyManipulation.strategyScoreInitialization(agentPopulation, memory);
    MarketInitialization.binaryMarketHistoryGenerator(memory);
    MarketInitialization.marketHistoryGenerator(memory);
    int attendence;
    for(int i = 0; i < 10; ++i){
        StrategyManipulation.deployment(memory, agentPopulation, strategiesPerAgent);
        StrategyManipulation.StrategyScoreUpdate(agentPopulation, strategiesPerAgent, memory, binaryMarkethistory);
    }
return 0;
}

//MarketInitialization Function Declaration
// should also be operating on ref vales, not globals
vector<int> MarketInitialization::binaryMarketHistoryGenerator(int memory){
    for(int i = 0; i < memory; i++){
        uniform_int_distribution<uint8_t> overkillBitGenerator(0, 1000); //here I should call one distribution, not one per function call...
        uint8_t bitToBe = overkillBitGenerator(123); //deterministically produces bit from random seed (I chose 0)
        bitToBe = (bitToBe < 500 ? 1 : -1); //cast will happen regardless, and will fit (highest unsigned int 1024>1000, no?)
        binaryMarkethistory.push_back(bitToBe);
    }
    return binaryMarkethistory;
}
vector<int> MarketInitialization::marketHistoryGenerator(int memory){
    for(int i = 0; i < memory; i++){
        uniform_int_distribution<uint8_t> marketValGenerator(-agentPopulation, agentPopulation);
        marketHistory.push_back(marketValGenerator(456);
    }
    return marketHistory;
}

//Strategy Manipulation Function Declaration
vector<vector<int>> StrategyManipulation::strategyScoreInitialization(int agentPopulation, int strategiesPerAgent){
    //Vector initialization to NxK filled with 0;
    vector<vector<int>> StrategyScores; //unnecessary due to global declaration, needs to be resolved
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            StrategyScores[i][j] = 0;//there must be a better way to initialize this...
        }
    }
    return strategyScores;
}
vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int memory, vector<int> binaryMarketHistory){
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation.evaluateStrategy(i,j,strategiesPerAgent, memory, binaryMarkethistory) == binaryMarkethistory.back()){
                strategyScores[i][j]--;
            strategyScores[i][j]++;
            }
        }
    }
    return strategyScores;
}
void StrategyManipulation::deployment(int memory, int agentPopulation, int strategiesPerAgent ){
    //Finds best preforming strategy and uses it.
    for(int i = 0; i < agentPopulation; ++i) {
        int indexJ = 0; // has to be initialized to 0 because j starts iterating at 1.
        for (int j = 1; j < strategiesPerAgent; j++) { //starting with j = 1, as we initialize tempCompare to j = 0
            int tempCompare = strategyScores[i][0];
            if (tempCompare < strategyScores[i][j]) { //here the preferred strategy (if all else equal) is the initial one)
                tempCompare = strategyScores[i][j];
                indexJ = j;
            }
        }
        //generate strategy and return +/-1 from it.
        marketCount+=StrategyManipulation.evaluateStrategy(i,indexJ,strategiesPerAgent,memory,binaryMarkethistory);
    }
    markethistory.push_back(marketCount);
    binaryMarkethistory.push_back(marketCount > 0 ? 1 : -1); //here the 50% threshold is hardcoded
    //I just want the latest elements, so need to continually push back the whole history...
}
uint8_t StrategyManipulation::evaluateStrategy(int indexI, int indexJ, int strategiesPerAgent, int memory, vector<int> binaryMarketHistory){
    //deterministically returns the strategy's value associated with the market history and strategy selection from the index
    //I'm taking enum consts. as arguments as I expect to use others later
    int convertedBinaryStrategy = BinaryManipulation::reverseBinaryConverter(memory, binaryMarketHistory);
    int indexSeed = (indexI*strategiesPerAgent)+indexJ; //converts to unique 1d array index

    //produce a unique number to seed the bit generator with dependence on both the above
    uniform_int_distribution<uint64_t> solutionDomain(0, pow(2, pow(2, memory)));
    uniform_int_distribution<uint64_t> seedRandomizer(0, pow(2, pow(2, memory))-pow(2,memory)); //the subtraction is to ensure that the
    // solution domain is not exceeded by the subsequent addition of converted binary string, which is as large as 2^m
    unsigned long long int randomizedSeedInput = seedRandomizer(indexSeed)
    int strategySolutionSeed = solutionDomain(randomizedSeedInput+convertedBinaryStrategy) //domain of 2^2^m,

    uniform_int_distribution<uint8_t> overkillBitGenerator(0, 1000);
    uint8_t bitToBe = overkillBitGenerator(strategySolutionSeed); //deterministically produces bit from strategy domain element
    bitToBe = (bitToBe < 500 ? 1 : -1); //cast will happen regardless, and will fit (highest unsigned int 1024>1000, no?)

    return (bitToBe);
}

//BinaryManipulation Function Declaration
vector<int> BinaryManipulation::binaryConverter(int memory, unsigned int long long index){ //converts number and memory to strategy domain sequence
    bitset<64> binary (index); //the 64 is the length converted to binary, the index the number. length must be > memory
    vector<int> strategyDomain;
    for(int i = 0; i < memory; i++) {
        if(binary[i] == 0){ //reads elements of the binary conversion, from largest power of 2 to smallest
            strategyDomain.push_back(-1); //cannot simply change 0 -> -1, as I'm using pushback (though I could use an array, and return an array pointer
        }else{
            strategyDomain.push_back(1);
        }
    }
    return strategyDomain;
}
int BinaryManipulation::reverseBinaryConverter(int memory, vector<int> strategyDomain) {
//the point is to create a strategy on the fly based only on the index corresponding to its binary  in that form.
    int strategyIndex = 0;
    for(int i = 0; i < memory; i++) {
        if(strategyDomain[i] == 1){
            strategyIndex+=pow(2, i);
        }
    }
    return strategyIndex;
}