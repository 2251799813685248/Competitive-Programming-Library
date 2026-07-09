#ifndef QUOTIENTS_HPP_
#define QUOTIENTS_HPP_

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
using namespace std;
using ll = long long;

/// @brief 1<=x<=M の範囲におけるN/xの商を列挙する。 {値, 左端, 右端}の形で求まる。
constexpr vector<array<ll,3>> enumerate_quotient(ll N, ll M){
    vector<array<ll,3>> ret;
    if (N == 0){
        ret.push_back({0,1,M});
        return ret;
    }
    ll k0 = sqrtl(N)-100;
    k0 = max(0ll, k0);
    ll k0r = sqrtl(N)+100;
    while (k0r*(k0r+1) <= N){
        k0r++;
    }
    while (k0r-k0 > 1){
        ll mid = (k0+k0r)/2;
        if (mid*(mid+1) <= N){
            k0 = mid;
        }
        else{
            k0r = mid;
        }
    }
    for (ll k = k0; k >= 0; k--){
        ret.push_back({k, N/(k+1)+1, min(M, k > 0 ? N/k : M)});
        if (ret.back()[1] > ret.back()[2]){
            ret.pop_back();
            break;
        }
    }
    reverse(ret.begin(), ret.end());
    for (ll x = min(M, N/(k0+1)); x >= 1; x--){
        ret.push_back({N/x, x,x});
    }
    return ret;
}

#endif /* QUOTIENTS_HPP_ */