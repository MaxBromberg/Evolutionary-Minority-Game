#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include "Minority_Game_Utilities.h"

typedef int signum;

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
    std::uniform_int_distribution<int> distribution {0, agentPopulation};

    std::vector<int> result (source.size());
    std::transform (source.begin(), source.end(), result.begin(), [&] (int i) {
        int rand = distribution (generator);
        return i * rand;
    });
    return result;
}//Initializes market history to rand val (-Agentpop, Agent Pop). Keeping it for conformity

int random_generate(double weight, int range, int seed){
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> memory_dist (1, range);
    auto memory = (int) (floor(memory_dist(generator) + weight*memory_dist(generator)));
    if( memory < 1){
        return random_generate(weight, range, seed + 1);
    }else if (memory > range){
        return random_generate(weight, range, seed + 1);
    }
    return memory;
}