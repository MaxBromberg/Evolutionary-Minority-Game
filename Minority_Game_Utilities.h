#pragma once

#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "debug_utilities.h"
#include "analysis_utilities.h"

typedef int signum;

// ***************************************************************************
//                              Utilities
// ***************************************************************************

std::vector<int> random_bool_vector(int size, int seed, int true_value = 1, int false_value = 0);
uint64_t signum_vector_to_bits(const std::vector<signum> &v, int n);
std::vector<signum> random_signum_vector(int size, int seed);
std::vector<int> market_history_generator(const std::vector<signum> &source, int agentPopulation, int seed);
int random_generate(double weight, int min, int max, int seed);
