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

    inline __int128_t modpow1111111111111111111(__int128_t x, int n){
        __int128_t r = 1;
        if (n){
            while (n){
                if (n%2){
                    r *= x;
                    r %= 1111111111111111111;
                }
                x *= x;
                x %= 1111111111111111111;
                n >>= 1;
            }
            return r;
        }
        return 1;
    }
    inline __int128_t modpow1234567894987654321(__int128_t x, int n){
        __int128_t r = 1;
        if (n){
            while (n){
                if (n%2){
                    r *= x;
                    r %= 1234567894987654321;
                }
                x *= x;
                x %= 1234567894987654321;
                n >>= 1;
            }
            return r;
        }
        return 1;
    }


    random_device rd;
    uniform_int_distribution<int> ui1;
    uniform_int_distribution<int> ui2;
    __int128_t B = 0, C = 0;
    __int128_t Binv,Cinv;

    /// @brief ローリングハッシュライブラリを初期化
    rolling_hash(){
        mt19937 gen(rd());
        ui1 = uniform_int_distribution<int>(134217728, 2147483647);
        ui2 = uniform_int_distribution<int>(1, 2147483647);
        //Bを設定
        while (B % 1111111111111111111 == 0){
            B = (((__int128_t)ui1(gen))<<31) + ui2(gen);
        }
        //Cを設定
        while (C % 1234567894987654321 == 0){
            C = (((__int128_t)ui1(gen))<<31) + ui2(gen);
        }
        B %= 1111111111111111111;
        C %= 1234567894987654321;
        Binv = (1111111111111111111+(axby1<__int128_t>(B,1111111111111111111)[0])%1111111111111111111)%1111111111111111111;
        Cinv = (1234567894987654321+(axby1<__int128_t>(C,1234567894987654321)[0])%1234567894987654321)%1234567894987654321;
    }

    inline __int128_t char_to_digit(char c){
        return (141*((3*c)^0b0101001101))^0b10010101110;
    }

    /// @brief 文字列Sにおいて、indexが0以上i以下の部分文字列のローリングハッシュを生成する。
    /// @param S 
    /// @return ローリングハッシュを記録した配列
    vector<pll> gen_hash(const string &S){
        vector<pll> r(S.size());
        r[0] = {(ll)char_to_digit(S[0]), (ll)char_to_digit(S[0])};
        array<__int128_t, 2> temp{B, C};
        for (size_t i = 1; i < S.size(); i++){
            r[i][0] = (r[i-1][0] + char_to_digit(S[i])*temp[0])%1111111111111111111;
            r[i][1] = (r[i-1][1] + char_to_digit(S[i])*temp[1])%1234567894987654321;
            temp[0] *= B;
            temp[0] %= 1111111111111111111;
            temp[1] *= C;
            temp[1] %= 1234567894987654321;
        }
        return r;
    }

    /// @brief indexが`l`以上`r`以下の部分文字列のハッシュを生成する。
    /// @param l 
    /// @param r 
    /// @return `[l,r]`のハッシュ
    pll get_hash(const vector<pll> &hs, int l, int r){
        array<__int128_t, 2> ret = {1111111111111111111 + hs[r][0] - (l == 0 ? 0 : hs[l-1][0]), 1234567894987654321 + hs[r][1] - (l == 0 ? 0 : hs[l-1][1])};
        ret[0] %= 1111111111111111111;
        ret[1] %= 1234567894987654321;
        ret[0] = (ret[0]*modpow1111111111111111111(Binv, l))%1111111111111111111;
        ret[1] = (ret[1]*modpow1234567894987654321(Cinv, l))%1234567894987654321;
        return {(ll)ret[0], (ll)ret[1]};
    }

    /// @brief aのハッシュ、aの長さ、bのハッシュを用いてa+bのハッシュを生成する。
    /// @param a 
    /// @param len_a 
    /// @param b 
    /// @return 
    pll connect_hash(const pll &a, int len_a, const pll &b){
        return {(ll)((a[0]+(b[0]*modpow1111111111111111111(B,len_a))%1111111111111111111)%1111111111111111111), (ll)((a[1]+(b[1]*modpow1234567894987654321(B,len_a))%1234567894987654321)%1234567894987654321)};
    }
};




#endif /* ROLLING_HASH_HPP_ */