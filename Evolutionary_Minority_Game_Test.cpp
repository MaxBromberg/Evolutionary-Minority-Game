#include "catch/catch.hpp"
#include <iostream>
#include <cmath>
#include <random>
#include <fstream>
#include <algorithm>
#include "analysis_utilities.h"
#include "Evolutionary_Minority_Game.h"
#include "debug_utilities.h"

using namespace std;

enum{
    TEST_POPULATION = 301,
    TEST_MEMORY_LENGTH = 3,
    TEST_RUN_TIME = 500,
    TEST_NUM_STRATEGIES_AGENT = 22,
    TEST_RNG_SEED = 42,
    TEST_AGENTS_IDENTIFIER = 0,
};

static const double euler_e = 2.71828182846;

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
    vector<int> empty_vector = {-1, -1, -1, -1, -1, -1};
    REQUIRE(BinaryArrayToStrategyIndex(binary_vector.begin(), binary_vector.end()) == 53);
    REQUIRE(BinaryArrayToStrategyIndex(binary_vector.begin(), binary_vector.end()) == BinaryArrayToStrategyIndex(nonbinary_vector_with_ones.begin(), nonbinary_vector_with_ones.end()));
    REQUIRE(BinaryArrayToStrategyIndex(empty_vector.begin(), empty_vector.end()) == 0);
}

TEST_CASE("BinaryVectorLastNToStrategyIndex"){
    vector<signum> binary_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED);
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
    std::vector<int> v3 = RandomBoolVector(10000, 0, 1, -1);
    std::vector<int> v4;

    for (int i = 0; i < 100; ++i) {
        v2.push_back (i % 2 ? 2 : 0);
        std:: vector<int> Random_Bool_Vector = RandomBoolVector(1000, 0, 1, -1);
        v4.push_back(std::accumulate(Random_Bool_Vector.begin(), Random_Bool_Vector.end(), 0));
    };

    REQUIRE (v.size() == 100);
    REQUIRE (v4.size() == 100);
    for(int i = 0; i < v4.size(); i++){
       REQUIRE(v4[i] !=  v[i+1]); //just to see if there is indeed variation
    }
    SECTION ("mean") {
        REQUIRE (Analysis::mean(v) == 5.0);
        REQUIRE (Analysis::mean(v2) == 1.0);
        REQUIRE (Analysis::mean(v3) <= 0.05);
        REQUIRE (Analysis::mean(v3) >= -0.05);
    }

    SECTION ("squared_mean") {
        REQUIRE (Analysis::squared_mean(v) == 25.0);
        REQUIRE (Analysis::squared_mean(v2) == 2.0);
        REQUIRE (Analysis::squared_mean(v3) == 1.0);
    }

    SECTION ("variance") {
        REQUIRE (Analysis::variance(v) == 0.0);
        REQUIRE (Analysis::variance(v2) == 1.0);
        REQUIRE (Analysis::variance(v3) < 1.05);
        REQUIRE (Analysis::variance(v3) > 0.95);
        REQUIRE (Analysis::variance(v4)/v4.size() < 1.05);
        REQUIRE (Analysis::variance(v4)/v4.size() > 0.95);
    }

    SECTION ("unexpanded_variance") {
        REQUIRE (Analysis::unexpanded_variance(v) == 0.0);
        REQUIRE (Analysis::unexpanded_variance(v2) == 1.0);
        REQUIRE (Analysis::unexpanded_variance(v3) < 0.55);
        REQUIRE (Analysis::unexpanded_variance(v3) > 0.45);
        REQUIRE (Analysis::unexpanded_variance(v4)/v4.size() < 1.05);
        REQUIRE (Analysis::unexpanded_variance(v4)/v4.size() > 0.95);
    }

    SECTION ("literature variance") {
        REQUIRE (Analysis::literatureVariance(v) == 0.0);
        REQUIRE (Analysis::literatureVariance(v2) == 1.0);
        REQUIRE (Analysis::literatureVariance(v3) < 0.55);
        REQUIRE (Analysis::literatureVariance(v3) > 0.45);
        REQUIRE (Analysis::literatureVariance(v4)/v4.size() < 1.05);
        REQUIRE (Analysis::literatureVariance(v4)/v4.size() > 0.95);

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
    Experiment environment1{TEST_POPULATION, TEST_NUM_STRATEGIES_AGENT, TEST_MEMORY_LENGTH, TEST_AGENTS_IDENTIFIER, TEST_RNG_SEED};

    SECTION("Experiment Constructor") {
        REQUIRE(environment1.agent_count == TEST_POPULATION);
        REQUIRE(environment1.strategies_per_agent == TEST_NUM_STRATEGIES_AGENT);
        REQUIRE(environment1.num_indicies_in_strategy == TEST_MEMORY_LENGTH);
        REQUIRE(environment1.history == binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH, TEST_RNG_SEED));
        REQUIRE(environment1.nonbinary_history ==
                marketHistoryGenerator(environment1.history, TEST_POPULATION, TEST_RNG_SEED));
    }
    SECTION("Agent Initialization") {
        /*
        for(int i = 0; i < environment1.linear_dist_agents.size(); i++){
            cout<<i<<"th Agent:"<<endl;
            for(int j = 0; j < environment1.linear_dist_agents[i].strategies.size(); j++){
                cout<<"For "<<j<<"th strategy:"<<endl;
                cout<<"num_indicies_in_strategy = "<<(environment1.linear_dist_agents[i].strategies[j].num_indicies_in_strategy)<<endl;
                cout<<"strategy score = "<<(environment1.linear_dist_agents[i].strategies[j].strategy_score)<<endl;
            }
        }
        */ //Print Out
        REQUIRE (environment1.agents.size() == environment1.agent_count);
        for (int i = 0; i < environment1.agents.size(); i++) {
            REQUIRE (environment1.agents[i].strategies.size() == environment1.strategies_per_agent);
            for (int j = 0; j < environment1.agents[i].strategies.size(); j++) {
                REQUIRE (environment1.agents[i].strategies[j].num_indicies_in_strategy ==
                         environment1.num_indicies_in_strategy);
                REQUIRE (environment1.agents[i].strategies[j].strategy_score == 0);
            }
        }//The for-loop should be unnecessary, given there is no initial variation

/*
        //Should perhaps initialize environment1 with the linear dist fct, but these values are the same
        vector<Agent> linear_dist_agents = environment1.linear_memory_dist_agent_init(TEST_AGENTS_IDENTIFIER);
        REQUIRE (linear_dist_agents.size() == environment1.agent_count);
        for (int i = 0; i < linear_dist_agents.size(); i++) {
            REQUIRE (linear_dist_agents[i].strategies.size() == environment1.strategies_per_agent);
            for (int j = 0; j < linear_dist_agents[i].strategies.size(); j++) {
                REQUIRE (linear_dist_agents[i].strategies[j].num_indicies_in_strategy == (i % environment1.num_indicies_in_strategy) + 1);
                REQUIRE (linear_dist_agents[i].strategies[j].strategy_score == 0);
            }
        }//The for-loop should be unnecessary, given there is no initial variation

        //Should perhaps initialize environment1 with the exp dist fct, but these values are the same
        vector<Agent> exponential_dist_agents = environment1.exponential_memory_dist_agent_init(euler_e, 1.2, TEST_AGENTS_IDENTIFIER);
        double max_exponential = log(environment1.num_indicies_in_strategy*1.2); //as ln(e) = 1, no division necessary
        REQUIRE (exponential_dist_agents.size() == environment1.agent_count);
        for (int i = 0; i < exponential_dist_agents.size(); i++) {
            REQUIRE (exponential_dist_agents[i].strategies.size() == environment1.strategies_per_agent);
            for (int j = 0; j < exponential_dist_agents[i].strategies.size(); j++) {
                REQUIRE (exponential_dist_agents[i].strategies[j].num_indicies_in_strategy == (int)floor((i/max_exponential)+0.5));
                REQUIRE (exponential_dist_agents[i].strategies[j].strategy_score == 0);
            }
        }//Does not contain actual information regarding the eventual distribution, though.
*/
        vector<Agent> weighted_random_dist_agents = environment1.weighted_rnd_memory_dist_agent_init(0, TEST_RNG_SEED, TEST_AGENTS_IDENTIFIER);
        REQUIRE (weighted_random_dist_agents.size() == environment1.agent_count);
        for (int i = 0; i < weighted_random_dist_agents.size(); i++) {
            REQUIRE (weighted_random_dist_agents[i].strategies.size() == environment1.strategies_per_agent);
            for (int j = 0; j < weighted_random_dist_agents[i].strategies.size(); j++) {
                REQUIRE (weighted_random_dist_agents[i].strategies[j].num_indicies_in_strategy <= environment1.num_indicies_in_strategy);
                REQUIRE (weighted_random_dist_agents[i].strategies[j].num_indicies_in_strategy >= 1);
                REQUIRE (weighted_random_dist_agents[i].strategies[j].strategy_score == 0);
            }
        }//Does not contain actual information regarding the eventual distribution, though.

        vector<Agent> bell_curve_dist_agents = environment1.bell_curve_memory_dist_agent_init(TEST_RNG_SEED, TEST_AGENTS_IDENTIFIER);
        REQUIRE (bell_curve_dist_agents.size() == environment1.agent_count);
        for (int i = 0; i < bell_curve_dist_agents.size(); i++) {
            REQUIRE (bell_curve_dist_agents[i].strategies.size() == environment1.strategies_per_agent);
            for (int j = 0; j < bell_curve_dist_agents[i].strategies.size(); j++) {
                REQUIRE (bell_curve_dist_agents[i].strategies[j].num_indicies_in_strategy <= environment1.num_indicies_in_strategy);
                REQUIRE (bell_curve_dist_agents[i].strategies[j].num_indicies_in_strategy >= 1);
                REQUIRE (bell_curve_dist_agents[i].strategies[j].strategy_score == 0);
            }
        }//Does not contain actual information regarding the eventual distribution, though.

    }

    SECTION("Minority Game Run"){
        //Experiment environment1{TEST_POPULATION, TEST_NUM_STRATEGIES_AGENT, TEST_MEMORY_LENGTH, TEST_RNG_SEED};
        int strategy_score = environment1.agents[0].strategies[0].strategy_score;
        int agent_pop = environment1.agents.size();
        environment1.run_minority_game(1); //the strategy score will definitely have a non-zero update
        REQUIRE(strategy_score != environment1.agents[0].strategies[0].strategy_score);
        REQUIRE(environment1.nonbinary_history.back() <= abs(environment1.agent_count));
        REQUIRE(environment1.agents.size() == agent_pop);
        REQUIRE(abs(environment1.history.back()) == 1);
    }

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
    vector<signum> alt_predictions;
    vector<signum> high_resolution_predictions;
    for(int i = 0; i < TEST_NUM_STRATEGIES_AGENT; i++){
        predictions.push_back(Agent1.predict(i, TEST_MEMORY_LENGTH, binary_history));
        predictions_2.push_back(Agent1.predict(i, TEST_MEMORY_LENGTH, binary_history_2));
        alt_predictions.push_back(Agent1.sin_predict(i, TEST_MEMORY_LENGTH, binary_history));
        high_resolution_predictions.push_back(Agent1.high_resolution_predict(i, TEST_MEMORY_LENGTH, binary_history));
    }
    int market_evaluation = accumulate(predictions.begin(), predictions.end(), 0);
    int market_evaluation_2 = accumulate(predictions_2.begin(), predictions_2.end(), 0);

    SECTION("Predict Function"){
        REQUIRE(abs(Agent1.predict(0,TEST_MEMORY_LENGTH, binary_history)) == 1);
        REQUIRE((Agent1.predict(0,TEST_MEMORY_LENGTH, binary_history)) == Agent1.predict(0,TEST_MEMORY_LENGTH,binary_history));
        cout<<"Predictions of all strategies of sample agent: "<<endl;
        debug_print(predictions); //easier to just see randomness than require it.
        REQUIRE(Analysis::numberOfUniqueElements(predictions) == 2);
        //REQUIRE(market_evaluation != market_evaluation_2); //as this is probable for high Strategies/agent
    }

    SECTION("High Resolution Predict Function"){
        REQUIRE(abs(Agent1.high_resolution_predict(0,TEST_MEMORY_LENGTH, binary_history)) == 1);
        REQUIRE((Agent1.high_resolution_predict(0,TEST_MEMORY_LENGTH, binary_history)) == Agent1.high_resolution_predict(0,TEST_MEMORY_LENGTH,binary_history));
        cout<<"High Resolution Predictions of sample agent: "<<endl;
        debug_print(high_resolution_predictions); //easier to just see randomness than require it.
        REQUIRE(Analysis::numberOfUniqueElements(high_resolution_predictions) == 2);

        vector<int> distribution;
        int distRange = 10000;
        for(int i = 0; i < 100000; i++){
            mt19937 generator (i);
            uniform_int_distribution<int> dist(-distRange, distRange);
            distribution.push_back(dist(generator));
        }
        REQUIRE(Analysis::mean(distribution) < 0.001*distRange);
        REQUIRE(Analysis::mean(distribution) > -0.001*distRange);
    }

    SECTION("Sin based Predict Function"){
        REQUIRE(abs(Agent1.sin_predict(0, TEST_MEMORY_LENGTH, binary_history)) == 1);
        REQUIRE((Agent1.sin_predict(0, TEST_MEMORY_LENGTH, binary_history)) ==
                        Agent1.sin_predict(0, TEST_MEMORY_LENGTH, binary_history));
        cout<<"Alt Predictions of sample agent: "<<endl;
        debug_print(alt_predictions); //easier to just see randomness than require it.
        REQUIRE(Analysis::numberOfUniqueElements(alt_predictions) == 2);
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
        //relies on market evaluation > 0
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
        //--------------reinitialization to compare--------------------------
        vector<Strategy> reinitialized_Strategies;
        for(int j = 0; j < TEST_NUM_STRATEGIES_AGENT; j++){
            reinitialized_Strategies.push_back(Strategy {0, TEST_MEMORY_LENGTH});
        }
        Agent1 = Agent{reinitialized_Strategies, 0};
        Agent2 = Agent{reinitialized_Strategies, 0};
        binary_history = binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH,TEST_RNG_SEED);
        predictions.clear();
        for(int i = 0; i < TEST_NUM_STRATEGIES_AGENT; i++){
            predictions.push_back(Agent1.predict(i, TEST_MEMORY_LENGTH, binary_history));
        }
        market_evaluation = accumulate(predictions.begin(), predictions.end(), 0);
        //--------------Reinitialization complete--------------------------

        Agent1.update(binary_history, sgn(market_evaluation));
        Agent2.weighted_update(binary_history, market_evaluation);
        if(sgn(Agent2.strategies[0].strategy_score) != 0) {
            REQUIRE(Agent1.strategies[0].strategy_score == sgn(Agent2.strategies[0].strategy_score));
        }
    }
}

TEST_CASE("Strategy Struct"){
    SECTION("Constructor"){
        Strategy strategy1 = Strategy{0, TEST_MEMORY_LENGTH};
        REQUIRE(strategy1.strategy_score == 0);
        REQUIRE(strategy1.num_indicies_in_strategy == TEST_MEMORY_LENGTH);
    }//not really much to test...
}

/*
TEST_CASE("Proving Randomness of Predict Functions"){
    const int SAMPLE_SIZE = 1000000;
    vector<signum> uniform_int_generation_values;
    vector<signum> sin_bit_sampled_values;
    for(int i = 1; i < SAMPLE_SIZE; i++) {
        mt19937 generator(i);
        uniform_int_distribution<int> bit_distribution(0, 1);
        uniform_int_generation_values.push_back(bit_distribution(generator) ? 1 : -1);
        double sin_seed = sin((double) i);
        sin_bit_sampled_values.push_back(((int)(sin_seed*10000) & 1) ? 1 : -1);
    }
    int market_evaluation = accumulate(uniform_int_generation_values.begin(), uniform_int_generation_values.end(), 0);
    int alt_market_evaluation = accumulate(sin_bit_sampled_values.begin(), sin_bit_sampled_values.end(), 0);
    REQUIRE(market_evaluation < SAMPLE_SIZE/100);
    REQUIRE(alt_market_evaluation < SAMPLE_SIZE/100);

    const int number_of_ten_bit_sequences = SAMPLE_SIZE/10;
    vector<int> convertered_test_sequence;
    vector<int> sin_convertered_test_sequence;
    for(int i = 0; i < number_of_ten_bit_sequences; i+=10){
        REQUIRE(BinaryArrayToStrategyIndex(uniform_int_generation_values.begin()+i, uniform_int_generation_values.begin()+(i+10)) <= 1024);
        REQUIRE(BinaryArrayToStrategyIndex(uniform_int_generation_values.begin()+i, uniform_int_generation_values.begin()+(i+10)) >= -1024);
        REQUIRE(BinaryArrayToStrategyIndex(sin_bit_sampled_values.begin()+i, sin_bit_sampled_values.begin()+(i+10)) <= 1024);
        REQUIRE(BinaryArrayToStrategyIndex(sin_bit_sampled_values.begin()+i, sin_bit_sampled_values.begin()+(i+10)) >= -1024);
        convertered_test_sequence.push_back(BinaryArrayToStrategyIndex(uniform_int_generation_values.begin()+i, uniform_int_generation_values.begin()+(i+10)));
        sin_convertered_test_sequence.push_back(BinaryArrayToStrategyIndex(sin_bit_sampled_values.begin()+i, sin_bit_sampled_values.begin()+(i+10)));
    }
    REQUIRE(convertered_test_sequence.size() == SAMPLE_SIZE/100);
    REQUIRE(convertered_test_sequence.size() % 100 == 0);

    //Overlapping sums test:
    vector<double> sums;
    vector<double> sin_sums;
    for(int i = 0; i < convertered_test_sequence.size(); i+=100){
        REQUIRE(accumulate(convertered_test_sequence.begin()+i, convertered_test_sequence.begin()+(i+100), 0) <= 102400);
        REQUIRE(accumulate(convertered_test_sequence.begin()+i, convertered_test_sequence.begin()+(i+100), 0) >= -102400);
        REQUIRE(accumulate(sin_convertered_test_sequence.begin()+i, sin_convertered_test_sequence.begin()+(i+100), 0) <= 102400);
        REQUIRE(accumulate(sin_convertered_test_sequence.begin()+i, sin_convertered_test_sequence.begin()+(i+100), 0) >= -102400);
        sums.push_back(abs((double) accumulate(convertered_test_sequence.begin()+i, convertered_test_sequence.begin()+(i+100), 0)/ (double) 102400));
        sin_sums.push_back(abs((double) accumulate(sin_convertered_test_sequence.begin()+i, sin_convertered_test_sequence.begin()+(i+100), 0)/ (double) 102400));
    }//These sums ought to be normally distributed, so var(sums) = 0.5, mean = 0
    //REQUIRE(sums[3] == 0); //Checks if sums were 0, just forces a print out to console of actual value
    REQUIRE(Analysis::mean(sums) < 0.06);
    REQUIRE(Analysis::mean(sums) > -0.06);
    REQUIRE(Analysis::mean(sin_sums) < 0.6);
    REQUIRE(Analysis::mean(sin_sums) > -0.6);
    REQUIRE(Analysis::variance(sums) < 1.1);
    REQUIRE(Analysis::variance(sums) > 0.9);
    REQUIRE(Analysis::variance(sin_sums) < 1.1);
    REQUIRE(Analysis::variance(sin_sums) > 0.9);

}
*/

