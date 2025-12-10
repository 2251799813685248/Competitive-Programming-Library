#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <set>
#define vall(a) a.begin(), a.end()
#define ll long long
using namespace std;



struct rolling_hash{

    __int128_t safe_modpow(__int128_t x, __int128_t n, __int128_t m){
        __int128_t r = 1;
        x %= m;
        if (n){
            while (n){
                if (n%2){
                    r *= x;
                    r %= m;
                }
                x *= x;
                x %= m;
                n >>= 1;
            }
            return r;
        }
        return 1;
    }

    pair<__int128_t, __int128_t> lllaxby1(__int128_t a, __int128_t b){
        if (a == 1 or b == 1){
            if (a == 1){
                return {1-b,1};
            }
            else{
                return {1,1-a};
            }
        }
        else if (a>b){
            auto p = lllaxby1(a%b, b);
            return {p.first, p.second - p.first*(a/b)};
        }
        else{
            swap(a,b);
            auto p = lllaxby1(a%b, b);
            return {p.second - p.first*(a/b), p.first};
        }
    }

    random_device rd__;
    uniform_int_distribution<int> ui1__;
    uniform_int_distribution<int> ui2__;
    __int128_t B;
    __int128_t Binv;
    __int128_t R = 1;
    vector<__int128_t> alphabet_to_ll;

    /// @brief ローリングハッシュライブラリを初期化
    rolling_hash(){
        mt19937 gen__(rd__());
        ui1__ = uniform_int_distribution(134217728, 2147483647);
        ui2__ = uniform_int_distribution(1, 2147483647);
        //Rが2冪にならないように乱数で初期化
        while ((1ll<<(63-__builtin_clzll((ll)R))) == (ll)R){
            R = (((ll)ui1__(gen__))<<30) + ui2__(gen__);
        }
        B = R;
        //BをRと互いに素になるように乱数で初期化
        while (__gcd(B,R) != 1){
            B = (((ll)ui1__(gen__))<<30) + ui2__(gen__);
        }

        B %= R;
        Binv = (R+(lllaxby1(B,R).first)%R)%R;

        set<ll> temp;
        while (temp.size() < 58){
            temp.insert(ui2__(gen__));
        }
        for (auto &v : temp){
            alphabet_to_ll.push_back(v);
        }
        shuffle(vall(alphabet_to_ll), gen__);
    }

    /// @brief 文字列Sにおいて、indexが0以上i以下の部分文字列のローリングハッシュを生成する。
    /// @param S 
    /// @return ローリングハッシュを記録した配列
    vector<__int128_t> gen_hash(const string &S){
        vector<__int128_t> r(S.size());
        r[0] = alphabet_to_ll[S[0]-97];
        __int128_t temp = B;
        for (ll i = 1; i < S.size(); i++){
            r[i] = r[i-1] + (alphabet_to_ll[S[i]-97]*temp)%R;
            r[i] %= R;
            temp *= B;
            temp %= R;
        }
        return r;
    }

    /// @brief l以上r以下の部分文字列のハッシュを生成する。
    /// @param l 
    /// @param r 
    /// @return [l,r]のハッシュ
    ll get_hash(const vector<__int128_t> &hs, const int &l, const int &r){
        __int128_t ret = hs[r] - (l == 0 ? 0 : hs[l-1]);
        ret += R;
        ret %= R;
        ret = (ret*safe_modpow(Binv, l, R))%R;
        return ret;
    }

    ll connect_hash(const __int128_t &a, const int &len_a, const __int128_t &b, const int &len_b){
        return (a+(b*safe_modpow(B,len_a,R))%R)%R;
    }
};