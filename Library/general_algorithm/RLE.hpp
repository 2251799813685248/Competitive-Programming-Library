#ifndef RLE_HPP_
#define RLE_HPP_

#include <utility>
#include <vector>
using namespace std;


template<typename T, typename itr> constexpr inline vector<pair<T, int>> RLE(itr begin, itr end){
    vector<pair<T, int>> res;
    for (itr l = begin, r = begin; l != end;){
        while (r != end && *l == *r){
            ++r;
        }
        res.push_back(make_pair(static_cast<T>(*l), static_cast<int>(r-l)));
        l = r;
    }
    return res;
}

#endif /* RLE_HPP_ */