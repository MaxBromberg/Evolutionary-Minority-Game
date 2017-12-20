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

void outputMinorityGameObservables(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS, int  NUM_DIFF_MEMORY_LENGTHS){
    //File Data Details
    ofstream file("Variance for Memory from 2 to 32, Pop from 11 to 1001, and 2000 Iterations.txt");
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
                    if (NUM_INDICES_IN_STRATEGY >= 8){
                        if (i % (NUM_DAYS_AGENTS_PLAY/4) == 0) {
                            cout << "Strategy Score matrix after " << i << " days printed below" << endl;
                            debug_print(strategy_scores);
                        }
                    }
                }
                cout<<"Binary History = "<<endl;
                debug_print(binary_history);

                vector<int> attendance = Analysis::attendance(market_history, AGENT_POPULATION);
                double Alpha = pow(2, double(NUM_INDICES_IN_STRATEGY)) / AGENT_POPULATION;
                double Variance = Analysis::variance(market_history);
                double successRate = Analysis::successRate(market_history, AGENT_POPULATION);

                file << AGENT_POPULATION << ", "
                     << NUM_INDICES_IN_STRATEGY << ", "
                     << Alpha << ", "
                     << Variance << ", "
                     << Variance / AGENT_POPULATION << ", "
                     << successRate << endl;

                //reseting: though the redeclaration should do this too, right?
                strategy_scores.clear();
                binary_history.clear();
                market_history.clear();
            }
        }
    }
}

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
vector<vector<int>> StrategyManipulation::strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                                              const vector<int>& binaryMarketHistory, vector<vector<int>> strategy_scores) {
    for(int i = 0; i < agentPopulation; ++i){
        for(int j = 0; j < strategiesPerAgent; ++j){
            //if the strategy's evaluation is the same as the market chose, the strategy is downgraded
            if(StrategyManipulation::evaluateStrategy(i,j, NumIndicesInStrategy, strategiesPerAgent, binaryMarketHistory) == binaryMarketHistory.back()) {
                //Strategy scores are not quantitatively updated, only qualitatively. Can readily be changed to accommodate quantitative results, but no
                strategy_scores[i][j]--;
            }else{
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
    auto indexSeed = (agent_idx*strategiesPerAgent) + index_of_best_strategy; //converts to unique 1d array index, will repeat

    //produce a unique number to seed the bit generator with dependence on both the above (index seed and strategy index)
    mt19937 indexDependentGenerator (indexSeed);
    mt19937 metaStrategyIndexDependentGenerator (metaStrategyIndex);
    uniform_int_distribution<int> bitdistribution(0,RNG_RESOLUTION);

    int randomizedSeedInput = bitdistribution(indexDependentGenerator);
    int strategySolutionSeed = bitdistribution(metaStrategyIndexDependentGenerator);
    //cout<<"strategy Solution Seed: "<<strategySolutionSeed<<"     randomizedSeedInput: "<<randomizedSeedInput<<endl;
    int bitToBe = randomizedSeedInput+strategySolutionSeed; //deterministically produces bit from strategy domain
    bitToBe = bitToBe < RNG_RESOLUTION ? 1 : -1;
    //Ideally takes less time than simply recalling from memory
    return (bitToBe);
}

void StrategyManipulation::binaryHistoryUpdate(vector<int>& binaryHistory, int marketCount){
    binaryHistory.push_back((marketCount > 0) ? 1 : -1);
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
        attendance.push_back(agentPop+obv[i]);
    }
    return attendance;
}//yeilds abs. attendance vector
