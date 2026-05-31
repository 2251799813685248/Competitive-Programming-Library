#ifndef GENERAL__algorithm_sum_HPP_
#define GENERAL__algorithm_sum_HPP_

#include <vector>
using namespace std;
using ll = long long;



/// @brief 累積和を作成する
template<typename T>
vector<T> cumulative_sum(vector<T> A){
    for (size_t i = 1, sz = A.size(); i < sz; i++){
        A[i] += A[i-1];
    }
    return A;
}

/// @brief vectorの和を求める
/// @tparam T 
/// @param A 
/// @return 
template<typename T>
T vsum(const vector<T> &A){
    T init_val = 0;
    for (auto &v : A){
        init_val += v;
    }
    return init_val;
}

/// @brief 前方向イテレーターで半壊区間を指定して、その間の要素の和を求める。
/// @tparam T 
/// @param A 
/// @return
template<typename Iterator>
ll vsum(Iterator begin, Iterator end){
    ll init_val = 0;
    while (begin != end){
        init_val += *begin;
        ++begin;
    }
    return init_val;
}




#endif /* GENERAL__algorithm_sum_HPP_ */