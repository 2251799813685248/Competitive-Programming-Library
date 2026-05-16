#ifndef INV_TABLE_HPP_
#define INV_TABLE_HPP_

#include <vector>
using ull = unsigned long long;

/// @brief mod M上での逆元テーブルを保持する構造体
/// @tparam M 
template<ull M>
struct inv_table{
    vector<uint> invmod;
    vector<uint> factorialinvmod;
    uint N_MAX;
    inv_table(const uint N_MAX__){
        N_MAX = max(1u, N_MAX__);
        invmod = vector<uint>(N_MAX+1);
        factorialinvmod = vector<uint>(N_MAX+1);
        invmod[1] = 1;
        for (uint i = 2; i <= N_MAX; i++){
            invmod[i] = (M-M/i)*invmod[M%i]%M;
        }
        factorialinvmod[0] = 1;
        for (uint i = 1; i <= N_MAX; i++){
            factorialinvmod[i] = (invmod[i]*factorialinvmod[i-1])%M;
        }
    }
};

#endif /* INV_TABLE_HPP_ */