#include <iostream>
#include <vector>
#include "debug_utilities.h"

enum {
    M = 12,
};

int main() {
    std::cout<<"Scratch File:"<<std::endl;

    //This is the formula to map from one st of whole numbers to another, with each integer in the first
    //series corresponding neatly to M elements of the second.
    std::vector<int> result;
    for (int n = 1; n <= 140; ++n) {
        for (int i = (M-1); i >= 0 ; --i) {
            result.emplace_back(n + (((M-1)*n) - i ) );
        }
    }
    debug_print(result);
    std::cout<<result.size()<<std::endl;

    return 0;
}