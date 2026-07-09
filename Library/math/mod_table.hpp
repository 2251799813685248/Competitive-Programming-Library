#ifndef INV_TABLE_HPP_
#define INV_TABLE_HPP_

#include <vector>
using uint = unsigned;
using ull = unsigned long long;
using namespace std;

/// @brief mod M上での階乗,逆元テーブルを保持する構造体
/// @tparam M 
template<ull M>
struct mod_table{
    vector<uint> invmod;
    vector<uint> factorialmod;
    vector<uint> factorialmodinv;
    uint N_MAX;
    constexpr mod_table(const uint N_MAX__){
        N_MAX = max(1u, N_MAX__);
        invmod = vector<uint>(N_MAX+1);
        factorialmod = vector<uint>(N_MAX+1);
        factorialmodinv = vector<uint>(N_MAX+1);
        invmod[1] = 1;
        for (uint i = 2; i <= N_MAX; i++){
            invmod[i] = (M-M/i)*invmod[M%i]%M;
        }
        factorialmodinv[0] = 1;
        factorialmod[0] = 1;
        for (ull i = 1; i <= N_MAX; i++){
            factorialmodinv[i] = (invmod[i]*(ull)factorialmodinv[i-1])%M;
            factorialmod[i] = (factorialmod[i-1]*i)%M;
        }
    }
};

#endif /* INV_TABLE_HPP_ */