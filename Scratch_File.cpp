#include <iostream>
#include <vector>
#include "debug_utilities.h"
#include "analysis_utilities.h"
#include <random>

//Sin vs Uniform_int_dist as bit generators are tested below.
//using namespace std;
//enum{
//    rng_seed = 44,
//    trials_per_run = 1000,
//    run_length = 100000,
//};
//
//typedef int signum;
//
//signum sin_bit_generator(int seed) {
//    auto result = sin(seed);
//    return ((int)(result*100000000.0) & 1) ? 1 : -1;
//}
//
//void write_memory_distribution(vector<int>& agents, vector<int>& uniform_agents) {
//    vector<vector<int>> memory_frequencies = Analysis::frequency_histogram(agents);
//    vector<vector<int>> uniform_memory_frequencies = Analysis::frequency_histogram(uniform_agents);
//    ofstream memory_distribution("Test Randomness Histogram.txt");
//    for (int i = 0; i < memory_frequencies.size(); i++) {
//        memory_distribution << memory_frequencies[i][0] << ", "
//                            << memory_frequencies[i][1] << ", "
//                            << uniform_memory_frequencies[i][0] << ", "
//                            << uniform_memory_frequencies[i][1] << endl;
//    }
//}
//
//int main (){
//    vector<int> agents;
//    vector<int> uniform_agents;
//
//    mt19937 bit_gen(rng_seed);
//    uniform_int_distribution<int> bit_dist(0,1);
//    for(int i = 0; i < run_length; ++i){
//        auto bits_from_one_run = 0;
//        auto uniform_bits_from_one_run = 0;
//        for(int j = 0; j < trials_per_run; j++){
//            bits_from_one_run += (sin_bit_generator( i*trials_per_run + j));
//            uniform_bits_from_one_run += bit_dist(bit_gen) ? 1 : -1;
//        }
//        agents.push_back(bits_from_one_run);
//        uniform_agents.push_back(uniform_bits_from_one_run);
//    }
//
//    double normalized_mean = Analysis::mean(agents)/(double)run_length;
//    double normalized_variance = Analysis::variance(agents)/(double)run_length;
//    double uniform_normalized_mean = Analysis::mean(uniform_agents)/(double)run_length;
//    double uniform_normalized_variance = Analysis::variance(uniform_agents)/(double)run_length;
//
//    printf("House Keeping : agent size = %i \n", agents.size());
//    printf(" Normalized mean = %lf \n Normalized variance = %lf \n", normalized_mean, normalized_variance);
//    printf(" Uniform Normalized mean = %lf \n Uniform Normalized variance = %lf \n", uniform_normalized_mean, uniform_normalized_variance);
//    write_memory_distribution(agents, uniform_agents);
//
//    return 0;
//}

enum {
    M = 5,
};

struct MrStruct{
    int num;

    void numplusplus(){
        num++;
    }

};


int main() {
    std::cout << "Scratch File:" << std::endl;

    std::cout << "Original Struct Vector Values:" << std::endl;
    std::vector<MrStruct *> MrVector;
    std::vector<MrStruct> Structs;
    for (int i = 0; i < M; ++i) {
        Structs.emplace_back(MrStruct{1});
        MrVector.emplace_back(&Structs[i]);
        std::cout << MrVector[i]->num << std::endl;
    }

    std::cout << "After shift via struct fct call:" << std::endl;
    for (auto e : MrVector) {
        e->numplusplus();
        std::cout << e->num << std::endl;
    }
    std::cout << Structs.size() << Structs[Structs.size()].num << endl;
}
