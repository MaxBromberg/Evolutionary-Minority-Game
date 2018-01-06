#include "catch/catch.hpp"
#include "Evolutionary_Minority_Game.h"
#include "debug_utilities.h"
#include "Intermediary_Function_Tests.h"
#include <iostream>
#include <cmath>
#include <random>
#include <fstream>

using namespace std;

enum{
    TEST_POPULATION = 301,
    TEST_MEMORY_LENGTH = 3,
    TEST_RUN_TIME = 1000,
    TEST_NUM_STRATEGIES_AGENT = 2,
    TEST_RNG_RESOLUTION = 10000,

    STATIC_RNG_TEST_SIZE = 40,
};

/*
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

TEST_CASE("Frequency Analysis"){
    std::vector<int> v (100,5);
    std::vector<int> v2;
    std::vector<int> v3 = RandomBoolVector(100, 0, 1, -1);

    for (int i = 0; i < 100; ++i) {
        v2.push_back (i % 2 ? 2 : 0);
    };

    SECTION("Frequency Histogram Function") {
        debug_print(Analysis::frequencyHistogram(v));
        debug_print(Analysis::frequencyHistogram(v2));
        debug_print(Analysis::frequencyHistogram(v3));
    }

    SECTION("Unique Element Frequency Function"){
        REQUIRE(Analysis::numberOfUniqueElements(v) == 1);
        REQUIRE(Analysis::numberOfUniqueElements(v2) == 2);
        REQUIRE(Analysis::numberOfUniqueElements(v3) == 2);
    }
}

//Compiled with Intermediary_Function_tests.h .cpp
TEST_CASE("Intermediate Tests") {
    /*
    SECTION("Full Test Run") {
        cout << "Alpha = " << (floor(pow(2, (double) TEST_MEMORY_LENGTH) + 0.5) / TEST_POPULATION) << endl;
        //test_run(TEST_NUM_STRATEGIES_AGENT, TEST_RUN_TIME, TEST_POPULATION, TEST_MEMORY_LENGTH);
        //test_Minority_Game_Attendance_History(TEST_NUM_STRATEGIES_AGENT, TEST_RUN_TIME, TEST_POPULATION, TEST_MEMORY_LENGTH);
        //output_Minority_Game_Attendance_History(TEST_NUM_STRATEGIES_AGENT, TEST_RUN_TIME, TEST_POPULATION, TEST_MEMORY_LENGTH);
    }

    SECTION("Uniqueness of Permuted Generator Seed") {
        /*
        REQUIRE(random_Generator_Test(0, 100, STATIC_RNG_TEST_SIZE) ==
               random_Generator_Test(0, 100, STATIC_RNG_TEST_SIZE));

        vector<int> RNGVector = random_Generator_Test(0, 100, STATIC_RNG_TEST_SIZE);
        vector<int> RNGVector2 = random_Generator_Test(0, 100, STATIC_RNG_TEST_SIZE);
        auto print = [](const int& n) { std::cout << " " << n; };

        cout << "First Vector = " << endl;
        std::for_each(RNGVector.begin(), RNGVector.end(), print);
        cout << "Second Vector = " << endl;
        std::for_each(RNGVector2.begin(), RNGVector2.end(), print);

        SECTION("Strategy Evaluation Check"){
            auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH);
            for(int i = 0; i < TEST_RUN_TIME; i++) {
            REQUIRE(evaluateStrategy_test(2, 2, TEST_MEMORY_LENGTH, TEST_NUM_STRATEGIES_AGENT, binary_history) ==
                    evaluateStrategy_test(2, 2, TEST_MEMORY_LENGTH, TEST_NUM_STRATEGIES_AGENT, binary_history));
            //the above lines are not axiomatically true, as it tests whether or not a RNG generator seeded with the same value
            //twice gives the same value, i.e. its internal state is not advanced for that seed. (leading to a diff. # generated)
            }
        }

        SECTION("Randomness of Strategy Evaluations"){
            auto binary_history = MarketInitialization::binaryMarketHistoryGenerator(TEST_MEMORY_LENGTH);
            vector<int> BitVector;
            vector<int> RandomVector;
            for(int i = 0; i < TEST_RUN_TIME; i++) {
                for(int j = 0; j < TEST_MEMORY_LENGTH; j++) {
                    mt19937 indexDependentGenerator (i);
                    mt19937 metaStrategyIndexDependentGenerator (j);

                    uniform_int_distribution<int> bitDistribution(0, TEST_RNG_RESOLUTION);
                    int bitToBe = bitDistribution(indexDependentGenerator)+bitDistribution(metaStrategyIndexDependentGenerator);
                    RandomVector.push_back(bitToBe);
                    bitToBe = bitToBe < TEST_RNG_RESOLUTION ? 1 : -1;
                    BitVector.push_back(bitToBe);
                }
            }
            auto randomVectorVariance = Analysis::variance(RandomVector);
            auto bitVectorVariance = Analysis::variance(BitVector);
            cout << "Random Vector Variance = " << randomVectorVariance << endl;
            cout << "Bit Vector Variance = " << bitVectorVariance << endl;

            ofstream randomfile ("Randomness Test Full Distribution.txt");
            ofstream randomBitsfile ("Randomness Test Bit Distribution.txt");
            for(int i = 0; i < RandomVector.size(); i++){
                randomfile << i << ", "
                           << RandomVector[i] << endl;
                randomBitsfile << i << ", "
                               << BitVector[i] << endl;
            }
        }

    }
*/

TEST_CASE("Strategy Scores Updates"){
    SECTION("Strategy Score Printout") {
        cout << "Alpha = " << (floor(pow(2, (double) TEST_MEMORY_LENGTH) + 0.5) / TEST_POPULATION) << endl;
        test_run(TEST_NUM_STRATEGIES_AGENT, TEST_RUN_TIME, TEST_POPULATION,
                                              TEST_MEMORY_LENGTH);
    }
}