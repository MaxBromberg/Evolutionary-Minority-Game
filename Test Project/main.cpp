#include <iostream>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

enum{
    NUM_ITERATIONS_PER_RN = 12,
    RANDOM_SEED = 12, //must be non-zero
    NUM_INDICIES_PER_STRATEGY = 4,
    NUM_AGENTS = 100,
    NUM_STRATEGIES_PER_AGENT = 2,
};

int RNG_retreiver(int seed);

int main(){
    for(int i = 0; i < 10; i++) {
        cout << "Seeded with the same value: " <<  RNG_retreiver(RANDOM_SEED) << endl;
    }
    for(int i = 0; i < 10; i++) {
        cout << "Seeded with a different value: " <<  RNG_retreiver(i) << endl;
    }
    for(int i = 0; i < 10; i++) {
        cout << "Seeded with a different value: " <<  RNG_retreiver(i) << endl;
    }
    return 0;
}

int RNG_retreiver(int seed){
    mt19937 generator (seed);
    uniform_int_distribution<int> dist(0, 10000);
    return(dist(generator));
}

/*
int LinearCongruentialGenerator(int num_iterations_per_run, int index, int seedX, int a, int c); //doesn't let me pass by reference?

int main() {
    cout << "Printing Pseudo Random Numbers!" << endl;
    //vector<int> LCG_Distribution;
    for(int i = 0; i < NUM_AGENTS*NUM_STRATEGIES_PER_AGENT; i++){
        cout<<i<<"th random number is "<<
            LinearCongruentialGenerator(NUM_ITERATIONS_PER_RN, i, RANDOM_SEED, NUM_INDICIES_PER_STRATEGY, NUM_STRATEGIES_PER_AGENT)<<endl;
        //LCG_Distribution.push_back(linearCongruentialGenerator(i, NUM_INDICIES_PER_STRATEGY, NUM_STRATEGIES_PER_AGENT));
    }
    return 0;
}


int LinearCongruentialGenerator(int num_iterations_per_run, int index, int seedX, int a, int c){ //compiler knows to bitshift
    int LCG = (a * seedX + index); //% 2;
    //for(int i = 0; i < num_iterations_per_run; i++){
    //    LCG = (a * LCG + c); //% 2;
    //}
    return LCG;
}
*/
