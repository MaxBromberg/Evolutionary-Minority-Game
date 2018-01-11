#include "catch/catch.hpp"
#include <iostream>
#include <cmath>
#include <random>
#include <fstream>
#include <numeric>
#include "analysis_utilities.h"
#include "Simulation_Functions.h"
#include "Evolutionary_Minority_Game.h"
#include "debug_utilities.h"

using namespace std;

enum{
    TEST_POPULATION = 11,
    TEST_MEMORY_LENGTH = 3,
    TEST_RUN_TIME = 1000,
    TEST_NUM_STRATEGIES_AGENT = 2,
    TEST_RNG_SEED = 42,
};

TEST_CASE ("Random Bool Vector") {
    auto v = RandomBoolVector(10, 0, 1, -1);

    REQUIRE (v.size() == 10);
    for (auto e : v) {
        if (e != 1 && e != -1)
            FAIL (e);
    }

    auto v2 = RandomBoolVector(10, 1, 1, -1);
    REQUIRE (v != v2);

    std::vector<std::vector<int>> v3;

    for (int i = 0; i < 10; ++i) {
        v3.emplace_back (RandomBoolVector (10, i, 1, -1));
    }

}

TEST_CASE("BinaryArrayToStrategyIndex"){
    vector<signum> binary_vector = {1, -1, 1, -1, 1, 1};
    vector<int> nonbinary_vector_with_ones = {1, 23, 1, -5, 1, 1};
    REQUIRE(BinaryArrayToStrategyIndex(binary_vector.begin(), binary_vector.end()) == 53);
    REQUIRE(BinaryArrayToStrategyIndex(binary_vector.begin(), binary_vector.end()) == BinaryArrayToStrategyIndex(nonbinary_vector_with_ones.begin(), nonbinary_vector_with_ones.end()));
}

TEST_CASE("BinaryVectorLastNToStrategyIndex"){
    vector<signum> binary_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED);
    int binary_conversion = BinaryVectorLastNToStrategyIndex(binary_history, TEST_MEMORY_LENGTH);
    REQUIRE(BinaryVectorLastNToStrategyIndex(binary_history, TEST_MEMORY_LENGTH) >= 0);
    REQUIRE(BinaryVectorLastNToStrategyIndex(binary_history, TEST_MEMORY_LENGTH) <= floor(pow(2, (double)TEST_MEMORY_LENGTH)+0.5));
}

TEST_CASE("BinaryMarketHistoryGenerator"){
    vector<signum> binary_market_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED);
    CHECK_FALSE(binary_market_history.empty());
    REQUIRE(BinaryArrayToStrategyIndex(binary_market_history.begin(), binary_market_history.end()) >= 0);
    if(TEST_MEMORY_LENGTH % 2 == 1){
        REQUIRE(std::accumulate(binary_market_history.begin(), binary_market_history.end(), 0) != 0);
    }
}

TEST_CASE("marketHistoryGenerator") {
    vector<signum> binary_market_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED);
    vector<int> market_history = marketHistoryGenerator(binary_market_history, TEST_POPULATION, TEST_RNG_SEED);
    CHECK_FALSE(market_history.empty());
    REQUIRE(BinaryArrayToStrategyIndex(market_history.begin(), market_history.end()) >= 0);
    if (TEST_MEMORY_LENGTH % 2 == 1) {
        REQUIRE(std::accumulate(market_history.begin(), market_history.end(), 0) != 0);
    }
}

TEST_CASE ("Numerics") {
    std::vector<int> v (100, 5);
    std::vector<int> v2;

    for (int i = 0; i < 100; ++i) {
        v2.push_back (i % 2 ? 2 : 0);
    };

    REQUIRE (v.size() == 100);

    SECTION ("mean") {
        REQUIRE (Analysis::mean(v) == 5.0);
        REQUIRE (Analysis::mean(v2) == 1.0);
    }

    SECTION ("squared_mean") {
        REQUIRE (Analysis::squared_mean(v) == 25.0);
        REQUIRE (Analysis::squared_mean(v2) == 2.0);
    }

    SECTION ("variance") {
        REQUIRE (Analysis::variance(v) == 0.0);
        REQUIRE (Analysis::variance(v2) == 1.0);
    }
}

TEST_CASE("Frequency Analysis") {
    std::vector<int> v (100,5);
    std::vector<int> v2;
    std::vector<int> v3 = RandomBoolVector(100, 0, 1, -1);

    for (int i = 0; i < 100; ++i) {
        v2.push_back (i % 2 ? 2 : 0);
    };

    /*
    SECTION("Frequency Histogram Function") {
        debug_print(Analysis::frequencyHistogram(v));
        debug_print(Analysis::frequencyHistogram(v2));
        debug_print(Analysis::frequencyHistogram(v3));
    }
    */

    SECTION("Unique Element Frequency Function"){
        REQUIRE(Analysis::numberOfUniqueElements(v) == 1);
        REQUIRE(Analysis::numberOfUniqueElements(v2) == 2);
        REQUIRE(Analysis::numberOfUniqueElements(v3) == 2);
    }
}

TEST_CASE ("Market Initialization") {
    SECTION ("Binary History Generator") {
        auto v = binaryMarketHistoryGenerator (10, TEST_RNG_SEED);
        for (auto e : v) {
            if (e != -1 && e != 1) {
                FAIL();
            }
        }
        REQUIRE (v.size() == 10);

        REQUIRE (binaryMarketHistoryGenerator (10, TEST_RNG_SEED) == v);
    }//Not exactly worth doing tests on the non-binary version, as it isn't really used....
}

TEST_CASE("Experiment Struct") {
    Experiment environment1{TEST_POPULATION, TEST_NUM_STRATEGIES_AGENT, TEST_MEMORY_LENGTH, TEST_RNG_SEED};
    SECTION("Agent Initialization"){
        /*
        for(int i = 0; i < environment1.agents.size(); i++){
            cout<<i<<"th Agent:"<<endl;
            for(int j = 0; j < environment1.agents[i].strategies.size(); j++){
                cout<<"For "<<j<<"th strategy:"<<endl;
                cout<<"num_indicies_in_strategy = "<<(environment1.agents[i].strategies[j].num_indicies_in_strategy)<<endl;
                cout<<"strategy score = "<<(environment1.agents[i].strategies[j].strategy_score)<<endl;
            }
        }
        */ //Print Out
        REQUIRE (environment1.agents.size() == environment1.agent_count);
        for(int i = 0; i < environment1.agents.size(); i++){
            REQUIRE (environment1.agents[i].strategies.size() == environment1.strategies_per_agent);
            REQUIRE (environment1.agents[i].strategies[0].num_indicies_in_strategy == environment1.num_indicies_in_strategy);
            REQUIRE (environment1.agents[i].strategies[0].strategy_score == 0);
        }//The for-loop should be unnecessary, given there is no initial variation
    }

    SECTION("Experiment Constructor"){
        REQUIRE(environment1.agent_count == TEST_POPULATION);
        REQUIRE(environment1.strategies_per_agent == TEST_NUM_STRATEGIES_AGENT);
        REQUIRE(environment1.num_indicies_in_strategy == TEST_MEMORY_LENGTH);
        REQUIRE(environment1.history == binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED));
        REQUIRE(environment1.nonbinary_history == marketHistoryGenerator(environment1.history, TEST_POPULATION, TEST_RNG_SEED));
    }
    /*
    SECTION("Minority Game Run"){
        int strategy_score = environment1.agents[0].strategies[0].strategy_score;
        int agent_pop = environment1.agents.size());
        environment1.run_minority_game(1); //the strategy score will definitely have a non-zero update
        REQUIRE(strategy_score != environment1.agents[0].strategies[0].strategy_score);
        REQUIRE(environment1.nonbinary_history.back() <= abs(environment1.agent_count));
        REQUIRE(environment1.agents.size() == agent_pop);
        REQUIRE(abs(environment1.history.back()) == 1);
    }
    */
}

TEST_CASE("Agent Struct"){
    //Initialization of one Agent (Agent1)
    vector<Strategy> Strategies;
    for(int j = 0; j < TEST_NUM_STRATEGIES_AGENT; j++){
        Strategies.push_back(Strategy {0, TEST_MEMORY_LENGTH});
    }
    Agent Agent1 = Agent{Strategies, 0};
    Agent Agent2 = Agent{Strategies, 0};
    vector<signum> binary_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH,TEST_RNG_SEED);
    vector<signum> binary_history_2 = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH,TEST_RNG_SEED + 1);

    SECTION("Constrcutor"){
        CHECK_FALSE(Agent1.strategies.empty());
        REQUIRE(Agent1.strategies[0].strategy_score == 0);
        REQUIRE(Agent1.strategies[0].num_indicies_in_strategy == TEST_MEMORY_LENGTH);
    }

    vector<signum> predictions;
    vector<signum> predictions_2;
    for(int i = 0; i < TEST_NUM_STRATEGIES_AGENT; i++){
        predictions.push_back(Agent1.predict(i, TEST_MEMORY_LENGTH, binary_history));
        predictions_2.push_back(Agent1.predict(i, TEST_MEMORY_LENGTH, binary_history_2));
    }
    int market_evaluation = accumulate(predictions.begin(), predictions.end(), 0);
    int market_evaluation_2 = accumulate(predictions_2.begin(), predictions_2.end(), 0);

    SECTION("Predict Function"){
        REQUIRE(abs(Agent1.predict(0,TEST_MEMORY_LENGTH, binary_history)) == 1);
        cout<<"Predictions of all strategies of sample agent: "<<endl;
        debug_print(predictions); //easier to just see randomness than require it.
        CHECK_FALSE(market_evaluation == 0); //
        REQUIRE(market_evaluation != market_evaluation_2); //as this is probable for high Strategies/agent
    }
    SECTION("Update Function"){
        int first_strategy_score_before = Agent1.strategies[0].strategy_score;
        int last_strategy_score_before = Agent1.strategies[TEST_NUM_STRATEGIES_AGENT-1].strategy_score;
        Agent1.update(binary_history, 1);
        CHECK_FALSE(first_strategy_score_before == Agent1.strategies[0].strategy_score);
        CHECK_FALSE(last_strategy_score_before == Agent1.strategies[TEST_NUM_STRATEGIES_AGENT-1].strategy_score);
        REQUIRE(abs(first_strategy_score_before - Agent1.strategies[0].strategy_score) == 1);
        REQUIRE(abs(last_strategy_score_before - Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score) == 1);

        //testing response to new binary history
        int first_strategy_score_before_2 = Agent1.strategies[0].strategy_score;
        int last_strategy_score_before_2 = Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score;
        Agent1.update(binary_history_2, -1);
        CHECK_FALSE(first_strategy_score_before_2 == Agent1.strategies[0].strategy_score);
        CHECK_FALSE(last_strategy_score_before_2 == Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score);
        REQUIRE(abs(first_strategy_score_before_2 - Agent1.strategies[0].strategy_score) == 1);
        REQUIRE(abs(last_strategy_score_before_2 - Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score) == 1);
    }
    SECTION("Weighted Update Function"){

        int first_strategy_score_before = Agent1.strategies[0].strategy_score;
        int last_strategy_score_before = Agent1.strategies[TEST_NUM_STRATEGIES_AGENT-1].strategy_score;
        Agent1.weighted_update(binary_history, market_evaluation);
        CHECK_FALSE(first_strategy_score_before == Agent1.strategies[0].strategy_score);
        CHECK_FALSE(last_strategy_score_before == Agent1.strategies[TEST_NUM_STRATEGIES_AGENT-1].strategy_score);
        REQUIRE(abs(first_strategy_score_before - Agent1.strategies[0].strategy_score) >= 1);
        REQUIRE(abs(last_strategy_score_before - Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score) >= 1);

        //testing response to new binary history
        int first_strategy_score_before_2 = Agent1.strategies[0].strategy_score;
        int last_strategy_score_before_2 = Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score;
        Agent1.weighted_update(binary_history_2, market_evaluation_2);
        CHECK_FALSE(first_strategy_score_before_2 == Agent1.strategies[0].strategy_score);
        CHECK_FALSE(last_strategy_score_before_2 == Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score);
        REQUIRE(abs(first_strategy_score_before_2 - Agent1.strategies[0].strategy_score) >= 1);
        REQUIRE(abs(last_strategy_score_before_2 - Agent1.strategies[TEST_NUM_STRATEGIES_AGENT - 1].strategy_score) >= 1);
}
    SECTION("Congruency of Update Functions"){
        Agent1.update(binary_history, sgn(market_evaluation));
        Agent2.weighted_update(binary_history, market_evaluation);
        if(sgn(Agent2.strategies[0].strategy_score) != 0) {
            REQUIRE(Agent1.strategies[0].strategy_score == sgn(Agent2.strategies[0].strategy_score));
        }
    }//Works as both agents are identical
}

TEST_CASE("Strategy Struct"){
    SECTION("Constructor"){
        Strategy strategy1 = Strategy{0, TEST_MEMORY_LENGTH};
        REQUIRE(strategy1.strategy_score == 0);
        REQUIRE(strategy1.num_indicies_in_strategy == TEST_MEMORY_LENGTH);
    }//not really much to test...
}

