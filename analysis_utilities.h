#pragma once

#include <iostream>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

using namespace std;

namespace Analysis{

    template <typename T>
    int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    template <typename T>
    static inline double mean(const T &v) {
        if (v.empty()) return 0.0;
        return static_cast<double> (std::accumulate (v.cbegin(), v.cend(), 0.0)) / static_cast<double> (v.size());
    }

    template <typename T>
    static inline double variance (const T& v) {
        if (v.empty()) return 0.0;
        auto m = mean(v);
        auto variance = 0.0;
        for(const auto& e : v){variance += (e - m)*(e - m);};
        return variance/v.size();
    }

    template <typename T>
    static inline std::vector<std::vector<int>> frequency_histogram(const std::vector<T> &v){
        std::vector<std::vector<int>> frequencyHistogram;
        std::vector<int> temp;
        std::vector<T> sortedV = v;
        std::sort(sortedV.begin(), sortedV.end());
        int i = 0;
        for(; i < sortedV.size();) {
            temp.push_back(sortedV[i]);
            temp.push_back(std::count(sortedV.cbegin(), sortedV.cend(), sortedV[i]));
            frequencyHistogram.push_back(temp);
            i += std::count(sortedV.begin(), sortedV.end(), sortedV[i]); //=temp[1]
            assert(sortedV[i-1] != sortedV[i]);
            temp.clear();
        }
        return frequencyHistogram;
    }

    template <typename T>
    static inline int number_of_unique_elements(const std::vector<T> &v){
        std::vector<T> tempV = v;
        std::sort(tempV.begin(),tempV.end());
        int uniqueCount = std::unique(tempV.begin(), tempV.end()) - tempV.begin();
        return uniqueCount;
    }


    //mg specific functions
    template <typename T>
    static inline double success_rate(const vector<T> &obv, int agentPop){
        double successRate = 0;
        for(auto e : obv){
            successRate += (agentPop - abs(e))/(double)(2*agentPop);
        }
        return successRate/obv.size();
    }

    static inline vector<int> attendance(const vector<int>& obv, int agentPop) {
        vector<int> attendance;
        for (int i = 0; i < obv.size(); i++) {
            attendance.push_back((agentPop / 2) - obv[i]);
        }
        return attendance;
    }

}