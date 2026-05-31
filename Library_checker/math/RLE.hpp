#ifndef RLE_HPP_
#define RLE_HPP_

#include <vector>
#include <string>
using namespace std;



/// @brief AをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
template<typename T>
vector<pair<T,int>> RLEvec(const vector<T> &A){
    vector<pair<T,int>> R;
    for (int l = 0, r = 0, sz = A.size(); l < sz;){
        while (r < sz && A[l] == A[r]){
            r++;
        }
        R.push_back(make_pair(A[l], r-l));
        l = r;
    }
    return R;
}

/// @brief SをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
vector<pair<char,int>> RLEstr(const string &S){
    vector<pair<char,int>> R;
    for (int l = 0, r = 0, sz = S.size(); l < sz;){
        while (r < sz && S[l] == S[r]){
            r++;
        }
        R.push_back(make_pair(S[l], r-l));
        l = r;
    }
    return R;
}



#endif /* RLE_HPP_ */