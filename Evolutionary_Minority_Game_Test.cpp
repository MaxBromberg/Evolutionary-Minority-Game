#include "catch/catch.hpp"
#include "Evolutionary_Minority_Game.h"
#include "debug_utilities.h"

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

    for (int i = 0; i < 100; ++i) {
        v2.push_back (i % 2 ? 2 : 0);
    };
    SECTION("Frequency Histogram Function") {
        debug_print(Analysis::frequencyHistogram(v));
        debug_print(Analysis::frequencyHistogram(v2));
    }
}