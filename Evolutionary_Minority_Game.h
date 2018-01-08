#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

std::vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);
std::vector<std::vector<int>> TwoDimensionalVector (unsigned int x_size, int y_size, int value);

typedef int signum;

struct StrategyManipulation {
    //there's no need to assign strategies initially, as they will be produced procedurally with the index serving as the RNG seed
    static std::vector<std::vector<int>> strategyScoreUpdate(int actual_value, int NumIndicesInStrategy,
                                                   const std::vector<int>& binaryMarketHistory, std::vector<std::vector<int>> strategy_scores);

    static std::vector<std::vector<int>> Alternate_strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                             const std::vector<int>& binaryMarketHistory, std::vector<std::vector<int>> strategy_scores, int market_Count);

    static int market_count (const std::vector<std::vector<int>>& strategy_scores, const std::vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent);

    struct Strategy {
        int agent_indx;
        int strategy_index;
        int strategiesPerAgent;

        signum predict (const std::vector<signum>& history);
    };

    struct Agent {
        std::vector<Strategy> strategies;
        std::vector<int> scores;

        signum vote();
        void update (const std::vector<signum>& history);
    };

    struct Experiment {
        int agent_count;
        int strategies_per_agent;
        std::vector<signum> history;
        std::vector<Agent> agents;
    };


    signum predict (const struct Strategy&, const std::vector<int>& history);

    static int evaluateStrategy (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                            int strategiesPerAgent, const std::vector<int>& binaryMarketHistory);
    static void binaryHistoryUpdate(std::vector<int>& binaryHistory, int marketCount);
};

struct MarketInitialization{
    static std::vector<int> binaryMarketHistoryGenerator(int NumIndicesInStrategy);
    static std::vector<int> marketHistoryGenerator(const std::vector<int>& source, int agentPopulation);
};

struct Analysis{

    template <typename T>
    static double mean(const T &v) {
        if (v.empty()) return 0.0;
        return static_cast<double> (std::accumulate (v.cbegin(), v.cend(), 0)) / static_cast<double> (v.size());
    }

    template <typename T>
    static double squared_mean (const T& v) {
        typedef typename T::value_type Number;
        if (v.empty()) return 0.0;
        return static_cast<double> (std::accumulate (v.cbegin(), v.cend(), 0, [] (const Number& accumulator, const Number& next) {return accumulator + next * next;})) / static_cast<double> (v.size());
    }

    template <typename T>
    static double variance (const T& v) {
        auto m = mean(v);
        return squared_mean (v) - (m * m);
    }

/*
    // Version 1, using find
    template <typename T>
    static std::vector<std::vector<T>> frequencyHistogram (const std::vector<T>& v){
        std::vector<std::vector<T>> frequencyHistogram;
        std::vector<T> temp;
        for(int i = 0; i < v.size(); i++) {
            temp.push_back(v[i]);
            temp.push_back(std::count(v.cbegin(), v.cend(), v[i]));
            frequencyHistogram.push_back(temp);
            for(; std::find(frequencyHistogram.begin(), frequencyHistogram.end(), v[i]) != v.cend(); ){
                i++;
            }
            temp.clear();
        }
    return frequencyHistogram;
    }
*/
    //Version 2, using sort. Possibly faster for larger vectors.
    template <typename T>
    static std::vector<std::vector<int>> frequencyHistogram (const std::vector<T>& v){
        std::vector<std::vector<int>> frequencyHistogram;
        std::vector<int> temp;
        std::vector<T> sortedV = v;
        std::sort(sortedV.begin(), sortedV.end());
        int i = 0;
        for(; i < sortedV.size();) {
            temp.push_back(sortedV[i]);
            temp.push_back(std::count(sortedV.cbegin(), sortedV.cend(), sortedV[i]));
            frequencyHistogram.push_back(temp);
            i+=std::count(sortedV.begin(), sortedV.end(), sortedV[i]); //=temp[1]
            assert(sortedV[i-1] != sortedV[i]);
            temp.clear();
        }
        return frequencyHistogram;
    }

    template <typename T>
    static int numberOfUniqueElements (const std::vector<T>& v){
        std::vector<T> tempV = v;
        std::sort(tempV.begin(),tempV.end());
        int uniqueCount = std::unique(tempV.begin(), tempV.end()) - tempV.begin();
    return uniqueCount;
    }

    static double literatureVariance(const std::vector<int>& obv);
    static double successRate(const std::vector<int>& obv, int agentPop);
    static std::vector<int> attendance(const std::vector<int>& obv, int agentPop);
};

// Simulation_Functions.h
//main output function, Num_Diff_Agent_Pop is defined in log10 Range, NUm_Diff_Memory_Lengths is log2 range
void outputMinorityGameObservables(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS, int  NUM_DIFF_MEMORY_LENGTHS);

void output_Minority_Game_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                        int NUM_INDICES_IN_STRATEGY);

void alt_Strategy_Score_Update_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY);
void strategy_test_run(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY);
