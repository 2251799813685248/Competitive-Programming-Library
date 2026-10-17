#ifndef INV_TABLE_HPP_
#define INV_TABLE_HPP_

#include <list.hpp>
#include <cassert>
#include <modint.hpp>
using uint = unsigned;
using ull = unsigned long long;
using ll = long long;
using namespace std;

/// @brief mod M上での階乗,逆元テーブルを保持する構造体
/// @tparam M 
template<uint M>
class mod_table{
    using mint = constant_modint<M>;
    list<mint> invmodlist;
    list<mint> factorialmodlist;
    list<mint> factorialmodinvlist;
    uint N_MAX;

    public:
    constexpr mod_table(const uint N_MAX__){
        N_MAX = max(1u, N_MAX__);
        invmodlist = list<mint>(N_MAX+1);
        factorialmodlist = list<mint>(N_MAX+1);
        factorialmodinvlist = list<mint>(N_MAX+1);
        invmodlist[1] = 1;
        for (uint i = 2; i <= N_MAX; i++){
            invmodlist[i] = (M-M/i)*invmodlist[M%i];
        }
        factorialmodinvlist[0] = 1;
        factorialmodlist[0] = 1;
        for (ull i = 1; i <= N_MAX; i++){
            factorialmodinvlist[i] = invmodlist[i]*factorialmodinvlist[i-1];
            factorialmodlist[i] = factorialmodlist[i-1]*i;
        }
    }
    inline constexpr mint inverse_mod(ll x) const {
        assert(0 <= x and x <= N_MAX);
        return invmodlist[x];
    }
    inline constexpr mint factorialmod(ll x) const {
        assert(0 <= x and x <= N_MAX);
        return factorialmodlist[x];
    }
    inline constexpr mint factorialmodinv(ll x) const {
        assert(0 <= x and x <= N_MAX);
        return factorialmodinvlist[x];
    }
    inline constexpr mint inverse_mod(mint x) const {return inverse_mod(x.val);}
    inline constexpr mint factorialmod(mint x) const {return factorialmod(x.val);}
    inline constexpr mint factorialmodinv(mint x) const {return factorialmodinv(x.val);}
};

#endif /* INV_TABLE_HPP_ */