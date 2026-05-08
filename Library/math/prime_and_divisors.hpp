#ifndef PRIME_AND_DIVISORS_HPP_
#define PRIME_AND_DIVISORS_HPP_

#include <array>
#include <vector>
#include <math_functions.hpp>
using namespace std;
using ll = long long;
using pll = array<ll,2>;
using pii = array<int,2>;



/// @brief 正の整数Nを素因数分解する
/// @return vector<array<ll,2>>{{素因数1,個数}, {素因数2,個数}, {素因数3,個数}...}
vector<pll> p_fact(ll N){
    if (N == 1){
        return vector<pll> {{1,0}};
    }
    vector<pll> R;//戻り値用リスト

    const int M = isqrt(N);
    for (int i = 2; i <= M; i++){
        if (N % i == 0){
            ll divide_count = 0;
            while (N % i == 0){
                divide_count++;
                N /= i;
            }
            R.push_back({i,divide_count});
        }
    }
    if (N != 1){
        R.push_back({N,1});
    }
    return R;
}

/// @brief 素因数分解リストを受け取って約数関数の値を求める
/// @return 約数のK乗和
template<typename T>
ll divisor_function(const vector<array<T,2>>& vv, ll K, const ll MOD = -1){
    if (vv[0][0] == 1){
        return 1;
    }
    ll R = 1;
    if (K == 0){
        for (auto x : vv){
            R *= x[1]+1;
            if (MOD > 0){
                R %= MOD;
            }
        }
    }
    else{
        for (auto x : vv){
            ll r = powll(x[0],K);
            R *= (powll(r,x[1]+1) - 1)/(r - 1);
            if (MOD > 0){
                R %= MOD;
            }
        }
    }
    return R;
}

/// @brief 素因数分解の結果pを受け取って、約数リストを生成する。
template<typename T>
vector<T> enumerate_divisor(const vector<array<T,2>>& p){
    vector<T> d{1};
    if (p[0][0] == 1){
        return d;
    }
    for (auto &v : p){
        int t = d.size();
        ll temp = 1;
        for (int w = 0; w < v[1]; w++){
            temp *= v[0];
            for (int i = 0; i < t; i++){
                d.push_back(d[i]*temp);
            }
        }
    }
    sort(vall(d));
    return d;
}

/// @brief 線形篩
/// @attention コンストラクタに整数Nを渡すことでN以下の整数を扱うことができる。
struct LinearSieve{
    vector<int> p_list;
    vector<int> lpf;
    //Nを渡すことで1以上N以下の整数を扱うことができる
    LinearSieve(int N): lpf(N+1,-1){
        lpf[1] = 1;
        int p_list_size = 0;
        for (int i = 2; i <= N; i++){
            if (lpf[i] < 0){
                p_list.push_back(i);
                p_list_size++;
                lpf[i] = i;
            }
            for (int j = 0; j < p_list_size && p_list[j] <= lpf[i] && p_list[j]*i <= N; j++){
                lpf[p_list[j]*i] = p_list[j];
            }
        }
    }
    vector<pii> p_fact(int x){
        if (x == 1){return {{1,0}};}
        vector<pii> r;
        do{
            if (r.empty() || lpf[x] != r.back()[0]){
                r.push_back({lpf[x], 1});
            }
            else{
                r.back()[1]++;
            }
            x /= lpf[x];
        }while(x > 1);
        return r;
    }
    vector<vector<pii>> p_fact_all(int N){
        vector<vector<pii>> r(N+1);
        r[1].push_back({1,0});
        for (int i = 2; i <= N; i++){
            r[i] = p_fact(i);
        }
        return r;
    }
};





#endif /* PRIME_AND_DIVISORS_HPP_ */