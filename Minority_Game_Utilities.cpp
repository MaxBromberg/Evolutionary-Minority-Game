#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include "Minority_Game_Utilities.h"

// ***************************************************************
//  Utilities Implementation
// ***************************************************************

template <typename Iter>
unsigned int sign_array_to_bits (Iter begin, Iter end) {
    int output = 0;
    for (auto iter = begin; iter != end; ++iter) {
        if (*iter == 1) output += 1 << (iter - begin);
    }
    return output;
}

std::vector<int> random_bool_vector(int size, int seed, int true_value, int false_value) { // generates a vector of random bools, as defined in true/false val
    std::mt19937 generator (seed);
    std::uniform_int_distribution<int> bitDistribution (0, 1);

    std::vector<int> v (size);
    std::generate (v.begin(), v.end(), [&bitDistribution, &generator, true_value, false_value] () {return bitDistribution (generator) ? true_value : false_value;});
    return v;
}

uint64_t signum_vector_to_bits(const std::vector<signum> &v, int n) {
    return sign_array_to_bits (v.end() - n, v.end());
}

std::vector<signum> random_signum_vector(int NumIndicesInStrategy, int seed){ //fills a vector with +/-1
    return random_bool_vector(NumIndicesInStrategy, seed, 1, -1);
}

std::vector<int> market_history_generator(const std::vector<signum> &source, int agentPopulation, int seed){
    std::mt19937 generator {static_cast<unsigned int>(seed)};
    //std::uniform_int_distribution<int> distribution {0, (int)floor((agentPopulation*0.2)+0.5)};
    std::uniform_int_distribution<int> distribution {0, agentPopulation};

    std::vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop). Keeping it for conformity

int random_generate(double weight, int min, int max, int seed){
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> random_gen(min, max);
    auto random_number = (int) (floor(random_gen(generator) + weight*random_gen(generator)));
    //this does make two different numbers for weight and origional
    if( random_number < 1){
        return random_generate(weight, min, max, seed + 1);
    }else if (random_number > max-min){
        return random_generate(weight, min, max, seed + 1);
    }
    return random_number;
}

