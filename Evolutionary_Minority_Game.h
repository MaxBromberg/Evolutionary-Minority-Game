#pragma once

#include <vector>
#include <numeric>

std::vector<int> RandomBoolVector (int size, int seed, int true_value = 1, int false_value = 0);
int BinaryVectorLastNToStrategyIndex (const std::vector<int>& v, int n);
std::vector<std::vector<int>> TwoDimensionalVector (unsigned int x_size, int y_size, int value);


struct StrategyManipulation {
    //there's no need to assign strategies initially, as they will be produced procedurally with the index serving as the RNG seed
    static std::vector<std::vector<int>> strategyScoreUpdate(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                                   const std::vector<int>& binaryMarketHistory, std::vector<std::vector<int>> strategy_scores);

    static int market_count (const std::vector<std::vector<int>>& strategy_scores, const std::vector<int>& binary_history,
                                        int NumIndicesInStrategy, int strategiesPerAgent);

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


    static double literatureVariance(const std::vector<int>& obv);
    static double successRate(const std::vector<int>& obv, int agentPop);
    static std::vector<int> attendance(const std::vector<int>& obv, int agentPop);
};
