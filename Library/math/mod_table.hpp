#ifndef INV_TABLE_HPP_
#define INV_TABLE_HPP_

#include <vector>
#include <cassert>
using uint = unsigned;
using ull = unsigned long long;
using ll = long long;
using namespace std;

/// @brief mod M上での階乗,逆元テーブルを保持する構造体
/// @tparam M 
template<uint M>
struct mod_table{
    vector<uint> invmodlist;
    vector<uint> factorialmodlist;
    vector<uint> factorialmodinvlist;
    uint N_MAX;
    constexpr mod_table(const uint N_MAX__){
        N_MAX = max(1u, N_MAX__);
        invmodlist = vector<uint>(N_MAX+1);
        factorialmodlist = vector<uint>(N_MAX+1);
        factorialmodinvlist = vector<uint>(N_MAX+1);
        invmodlist[1] = 1;
        for (uint i = 2; i <= N_MAX; i++){
            invmodlist[i] = (M-M/i)*(ull)invmodlist[M%i]%M;
        }
        factorialmodinvlist[0] = 1;
        factorialmodlist[0] = 1;
        for (ull i = 1; i <= N_MAX; i++){
            factorialmodinvlist[i] = (invmodlist[i]*(ull)factorialmodinvlist[i-1])%M;
            factorialmodlist[i] = (factorialmodlist[i-1]*i)%M;
        }
    }
    ll inverse_mod(ll x){
        assert(0 <= x and x < N_MAX);
        return invmodlist[x];
    }
    ll factorialmod(ll x){
        assert(0 <= x and x < N_MAX);
        return factorialmodlist[x];
    }
    ll factorialmodinv(ll x){
        assert(0 <= x and x < N_MAX);
        return factorialmodinvlist[x];
    }
};

#endif /* INV_TABLE_HPP_ */