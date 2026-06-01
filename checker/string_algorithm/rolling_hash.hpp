#ifndef ROLLING_HASH_HPP_
#define ROLLING_HASH_HPP_

#include <vector>
#include <array>
#include <random>
#include <math_functions.hpp>
using namespace std;
using ll = long long;
using pll = array<ll, 2>;


/// @brief ローリングハッシュの作成、接続など
struct rolling_hash{
    inline ull multiply_mod(ull x, ull y){
        ull a = x>>31, b = x&2147483647ull, c = y>>31, d = y&2147483647ull;
        ull mid1 = a*d+b*c;
        ull mid2 = mid1&1073741823ull;
        mid1 >>= 30;
        ull ret = 2*a*c+mid1+(mid2<<31)+b*d;
        ret = (ret>>61)+(ret&2305843009213693951ull);
        return ret < 2305843009213693951ull ? ret : ret-2305843009213693951ull;
    }
    inline ull modpow261(ull x, int n){
        ull r = 1;
        if (n){
            while (n){
                if (n&1){
                    r = multiply_mod(r,x);
                }
                x = multiply_mod(x,x);
                n >>= 1;
            }
            return r;
        }
        return 1;
    }
    random_device rd;
    uniform_int_distribution<int> ui1;
    uniform_int_distribution<int> ui2;
    ull B = 0;
    ull Binv;
    /// @brief ローリングハッシュライブラリを初期化
    rolling_hash(){
        mt19937 gen(rd());
        ui1 = uniform_int_distribution<int>(134217728, 2147483647);
        ui2 = uniform_int_distribution<int>(1, 2147483647);
        //Bを設定
        while (B % 2305843009213693951ull == 0){
            B = (((ull)ui1(gen))<<31) + ui2(gen);
        }
        B %= 2305843009213693951ull;
        Binv = (2305843009213693951+(axby1<__int128_t>(B,2305843009213693951)[0])%2305843009213693951)%(__int128_t)2305843009213693951;
    }
    inline ull char_to_digit(char c){
        return (141*((3*c)^0b0101001101))^0b10010101110;
    }
    /// @brief 文字列Sにおいて、indexが0以上i以下の部分文字列のローリングハッシュを生成する。
    /// @param S 
    /// @return ローリングハッシュを記録した配列
    vector<ull> gen_hash(const string &S){
        vector<ull> r(S.size());
        r[0] = char_to_digit(S[0]);
        ull temp = B;
        for (size_t i = 1; i < S.size(); i++){
            r[i] = r[i-1] + multiply_mod(temp, char_to_digit(S[i]));
            if (r[i] >= 2305843009213693951ull){
                r[i] -= 2305843009213693951ull;
            }
            temp = multiply_mod(temp, B);
        }
        return r;
    }
    /// @brief indexが`l`以上`r`以下の部分文字列のハッシュを生成する。
    /// @param l 
    /// @param r 
    /// @return `[l,r]`のハッシュ
    ull get_hash(const vector<ull> &hs, int l, int r){
        ull ret = 2305843009213693951ull + hs[r] - (l == 0 ? 0ull : hs[l-1]);
        if (ret >= 2305843009213693951ull){
            ret -= 2305843009213693951ull;
        }
        ret = multiply_mod(ret, modpow261(Binv, l));
        return ret;
    }
    /// @brief aのハッシュ、aの長さ、bのハッシュを用いてa+bのハッシュを生成する。
    /// @param a 
    /// @param len_a 
    /// @param b 
    /// @return 
    ull connect_hash(const ull a, const int len_a, const ull b){
        ull ret = a+multiply_mod(b, modpow261(B,len_a));
        if (ret >= 2305843009213693951ull){
            ret -= 2305843009213693951ull;
        }
        return ret;
    }
};



#endif /* ROLLING_HASH_HPP_ */