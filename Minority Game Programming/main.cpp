#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

//Header Declarations
enum constants{
    memory = 4,
};

class StrategyManipulation{
    public:

    //Member Functions
    static vector<int> binaryConverter(int memory, unsigned int long long index);
    static int reverseBinaryConverter(int memory, vector<int> strategyDomain);

};

int main(){


    unsigned int x = pow(2, 32)-1;
    cout << "x = " << x/2 << endl;
    int y = 1;
    cout << "x + y = " << x << " + " << y << " = " << x + y << endl;

    int z = 16;
    cout<<"Before z = "<<z<<endl;
    z <<=3;
    cout<<"After <<3, z = " <<z<< endl;
    cout<<endl;

    //Test binary Converter
    int index = 8;
    vector<int> test = StrategyManipulation::binaryConverter(memory,index);
    //Print Strategy
    cout << "Read as binary, with, 0 -> -1::"<<endl;
    for(int i = 0; i < test.size(); i++){
    cout << "2^" << i << "+    " << test[i] << endl;
    }
    cout << "= " << index << endl;
    //Test reverse Binary Converter
    int shouldBeIndex = StrategyManipulation::reverseBinaryConverter(memory, test);
    cout << "This should return the same as above: " << shouldBeIndex << endl;

return 0;
}


//StrategyManipulation Function Declaration
vector<int> StrategyManipulation::binaryConverter(int memory, unsigned int long long index){ //converts number and memory to strategy domain sequence
    bitset<64> binary (index); //the 64 is the length converted to binary, the index the number. length must be > memory
    vector<int> strategyDomain;
    for(int i = 0; i < memory; i++) {
        if(binary[i] == 0){ //reads elements of the binary conversion, from largest power of 2 to smallest
            strategyDomain.push_back(-1); //cannot simply change 0 -> -1, as I'm using pushback (though I could use an array, and return an array pointer
        }else{
            strategyDomain.push_back(1);
        }
    }
    return strategyDomain;
}

int StrategyManipulation::reverseBinaryConverter(int memory, vector<int> strategyDomain) {
//the point is to create a strategy on the fly based only on the index corresponding to its binary  in that form.
    int strategyIndex = 0;
    for(int i = 0; i < memory; i++) {
        if(strategyDomain[i] == 1){
            strategyIndex+=pow(2, i);
        }
    }
    return strategyIndex;
}

