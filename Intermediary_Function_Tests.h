#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;

static std::vector<std::vector<int>> strategyScoreUpdate_test(int agentPopulation, int strategiesPerAgent, int NumIndicesInStrategy,
                                               const std::vector<int>& binaryMarketHistory, std::vector<std::vector<int>> strategy_scores);

static int market_count_test (const std::vector<std::vector<int>>& strategy_scores, const std::vector<int>& binary_history,
                                    int NumIndicesInStrategy, int strategiesPerAgent);

static int evaluateStrategy_test (int agent_idx, int index_of_best_strategy, int NumIndicesInStrategy,
                                        int strategiesPerAgent, const std::vector<int>& binaryMarketHistory);

static void binaryHistoryUpdate_test (std::vector<int>& binaryHistory, int marketCount);

void test_run(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
              int NUM_INDICES_IN_STRATEGY);

void test_Minority_Game_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY);