#pragma once

#include <iostream>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

struct Analysis{

    template <typename T>
    static double mean(const T &v) {
        if (v.empty()) return 0.0;
        return static_cast<double> (std::accumulate (v.cbegin(), v.cend(), 0)) / static_cast<double> (v.size());
    }

    template <typename T>
    static double squared_mean (const T& v) {
        typedef typename T::value_type Number;
        if (v.empty()) return 0.0;
        return static_cast<double> (std::accumulate (v.cbegin(), v.cend(), 0, [] (const Number& accumulator, const Number& next) {return accumulator + next * next;})) / static_cast<double> (v.size());
    }

    template <typename T>
    static double variance (const T& v) {
        auto m = mean(v);
        return squared_mean (v) - (m * m);
    }

/*
    // Version 1, using find
    template <typename T>
    static std::vector<std::vector<T>> frequencyHistogram (const std::vector<T>& v){
        std::vector<std::vector<T>> frequencyHistogram;
        std::vector<T> temp;
        for(int i = 0; i < v.size(); i++) {
            temp.push_back(v[i]);
            temp.push_back(std::count(v.cbegin(), v.cend(), v[i]));
            frequencyHistogram.push_back(temp);
            for(; std::find(frequencyHistogram.begin(), frequencyHistogram.end(), v[i]) != v.cend(); ){
                i++;
            }
            temp.clear();
        }
    return frequencyHistogram;
    }
*/
    //Version 2, using sort. Possibly faster for larger vectors.
    template <typename T>
    static std::vector<std::vector<int>> frequencyHistogram (const std::vector<T>& v){
        std::vector<std::vector<int>> frequencyHistogram;
        std::vector<int> temp;
        std::vector<T> sortedV = v;
        std::sort(sortedV.begin(), sortedV.end());
        int i = 0;
        for(; i < sortedV.size();) {
            temp.push_back(sortedV[i]);
            temp.push_back(std::count(sortedV.cbegin(), sortedV.cend(), sortedV[i]));
            frequencyHistogram.push_back(temp);
            i+=std::count(sortedV.begin(), sortedV.end(), sortedV[i]); //=temp[1]
            assert(sortedV[i-1] != sortedV[i]);
            temp.clear();
        }
        return frequencyHistogram;
    }

    template <typename T>
    static int numberOfUniqueElements (const std::vector<T>& v){
        std::vector<T> tempV = v;
        std::sort(tempV.begin(),tempV.end());
        int uniqueCount = std::unique(tempV.begin(), tempV.end()) - tempV.begin();
        return uniqueCount;
    }

    static double literatureVariance(const vector<int>& obv){
        vector<double> sqObvMinusObvMean(obv.size()); //implement generate
        double variance;
        double obvMean = Analysis::mean(obv);
        for(int i = 0; i < sqObvMinusObvMean.size(); i++){
            sqObvMinusObvMean[i] = (obv[i]-obvMean)*(obv[i]-obvMean);
        }
        variance = Analysis::mean(sqObvMinusObvMean);
        return variance;
    }

    static double successRate(const vector<int>& obv, int agentPop){
        double successRate = 0;
        for(auto e : obv){
            successRate += (agentPop - abs(e))/(double)(2*agentPop);
        }
        return successRate/obv.size();
    }

    static vector<int> attendance(const vector<int>& obv, int agentPop) {
        vector<int> attendance;
        for (int i = 0; i < obv.size(); i++) {
            attendance.push_back((agentPop / 2) - obv[i]);
        }
        return attendance;
    }

};