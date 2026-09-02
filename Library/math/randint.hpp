#ifndef RANDINT__HPP_
#define RANDINT__HPP_

#include <random>
#include <cassert>
using ll = long long;
using ull = unsigned long long;
using namespace std;

/// @brief 符号付き64bit整数の範囲内の乱数を生成する。
struct randint{
    mt19937_64 mt;
    uniform_int_distribution<ull> ui;
    randint(ll seed = 9223372036854775807ll){
        if (seed == 9223372036854775807ll){
            random_device rd;
            mt = mt19937_64(rd());
        }
        else{
            mt = mt19937_64(seed);
        }
        ui = uniform_int_distribution<ull>(0, 18446744073709551615ull);
    };
    /// @brief `L`以上`R`未満の乱数を生成する。
    /// @param L
    /// @param R
    ll gen(ll L = -9223372036854775807ll, ll R = 9223372036854775807ll){
        assert(L<R);
        ull M = R-L;
        return ui(mt)%M+L;
    }
};

#endif /* RANDINT__HPP_ */