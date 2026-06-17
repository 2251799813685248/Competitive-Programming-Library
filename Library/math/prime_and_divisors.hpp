#ifndef PRIME_AND_DIVISORS_HPP_
#define PRIME_AND_DIVISORS_HPP_

#include <array>
#include <vector>
#include <math_functions.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
using namespace std;
using ll = long long;
using ulll = __uint128_t;
using pll = array<ll,2>;
using pii = array<int,2>;



/// @brief 試し割り法で正の整数Nを素因数分解する
/// @return vector<array<ll,2>>{{素因数1,個数}, {素因数2,個数}, {素因数3,個数}...}
constexpr vector<pll> trial_division(ll N){
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
    vector<pii> factorize(int x){
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
    vector<vector<pii>> factorize_all(int N){
        vector<vector<pii>> r(N+1);
        r[1].push_back({1,0});
        for (int i = 2; i <= N; i++){
            r[i] = factorize(i);
        }
        return r;
    }
    /// @brief N以下の整数に対するメビウス関数の値を列挙する。
    vector<int> enumerate_mobius(int N){
        vector<int> ret(N+1);
        ret[1] = 1;
        for (int i = 2; i <= N; i++){
            if (lpf[i] == i){ret[i] = -1; continue;}
            int temp = i/lpf[i];
            if (lpf[i] == lpf[temp]){
                ret[i] = 0;
            }
            else{
                ret[i] = ret[lpf[i]]*ret[temp];
            }
        }
        return ret;
    }
};

constexpr ull __MillerRabin_small[3] = {2,7,61};
constexpr ull __MillerRabin_large[7] = {2,325,9375,28178,450775,9780504,1795265022};

struct Montgomery64 {
    ull n, ni, r2;
    constexpr Montgomery64(ull n) : n(n), ni(n), r2(-ulll(n) % n){
        for (int i = 0; i < 5; ++i) ni *= 2ull - n * ni;
        ni *= -1;
    }
    constexpr ull reduce(ulll x) const {
        ull m = (ull)x * ni;
        ull res = (x + ulll(m) * n) >> 64;
        return res >= n ? res - n : res;
    }
    constexpr ull mul(ull x, ull y) const {
        return reduce(ulll(x) * y);
    }
    constexpr ull pow_not_reduced(ull a, ull b)const{
        ull res = reduce(r2);
        a = reduce(ulll(a) * r2);
        while (b) {
            if (b & 1) res = mul(res, a);
            a = mul(a, a);
            b >>= 1;
        }
        return res;
    }
};

/// @brief ミラーラビン素数判定法 
constexpr bool MillerRabin(ull N){
    if (N <= 1) return false;
    if (N == 2 || N == 3) return true;
    if (N % 2 == 0) return false;

    ull d = N - 1;
    int s = 0;
    while (d % 2 == 0){
        d >>= 1;
        s++;
    }
    Montgomery64 mg(N);
    auto check = [&](ull a){
        if (a >= N) a %= N;
        if (a == 0) return true;
        ull x = mg.pow_not_reduced(a, d);
        ull y = mg.reduce(x);
        if (y == 1 || y == N-1) return true;
        for (int r = 1; r < s; r++){
            x = mg.mul(x, x);
            if (mg.reduce(x) == N - 1) return true;
        }
        return false;
    };

    if (N < 4759123141ull){
        for (auto a : __MillerRabin_small){
            if (!check(a)) return false;
        }
    }
    else{
        for (auto a : __MillerRabin_large){
            if (!check(a)) return false;
        }
    }
    return true;
}

/// @brief ポラード・ロー法による素因数分解
constexpr vector<pll> Pollard_rho(ull N){
    assert(N>0);
    if (N == 1){return {{1,0}};}
    vector<ll> res;
    vector<ull> dq;
    dq.push_back(N);
    while (!dq.empty()){
        ull n = dq.back();
        dq.pop_back();
        bool next_loop = false;
        while (!MillerRabin(n)){
            if ((n&1) == 0){
                while ((n&1) == 0){res.push_back(2); n>>=1;}
                if (n == 1){next_loop = true; break;}
                continue;
            }
            Montgomery64 mg(n);
            int blocksize = max<int>(1, pow(n, 0.125));
            for (ull c = 1; c <= n; c++){
                ull reduced_c = mg.reduce(c*(ulll)mg.r2);
                ull y = (0b1010110101101011110011ull^c^n)%n; y = mg.reduce(y*(ulll)mg.r2);
                ull r = 1;
                ull k = 0;
                int k_mod_blocksize = 0;
                bool next_loop_inner = false;
                for (int _ = 0; _ < 63; _++){
                    ull q = mg.reduce(mg.r2);
                    ull q_old = q;
                    ull y_old = y;
                    ull x = y;
                    while (k < r){
                        k++;
                        k_mod_blocksize++;
                        y = mg.reduce(y*(ulll)y)+reduced_c; if (y >= n){ y -= n;}
                        q = mg.reduce(q*(y >= x ? (ulll)(y-x) : (ulll)(x-y)));
                        if (k_mod_blocksize == blocksize){
                            k_mod_blocksize -= blocksize;
                            ull g = gcd(mg.reduce(q),n);
                            if (g > 1){
                                if (g != n){
                                    dq.push_back(g);
                                    dq.push_back(n/g);
                                    next_loop = true; break;
                                }
                                else{
                                    y = y_old;
                                    q = q_old;
                                    for (int i = 0; i < blocksize; i++){
                                        y = mg.reduce(y*(ulll)y)+reduced_c; if (y >= n){ y -= n;}
                                        q = mg.reduce(q*(y >= x ? (ulll)(y-x) : (ulll)(x-y)));
                                        g = gcd(mg.reduce(q),n);
                                        if (g > 1){
                                            if (g != n){
                                                dq.push_back(g);
                                                dq.push_back(n/g);
                                                next_loop = true; break;
                                            }
                                            next_loop_inner = true; break;
                                        }
                                    }
                                    if (next_loop || next_loop_inner) break;
                                }
                            }
                            q_old = q;
                            y_old = y;
                        }
                    }
                    if (next_loop || next_loop_inner) break;
                    q_old = gcd(mg.reduce(q),n);
                    if (q_old > 1){
                        if (q_old != n){
                            dq.push_back(q_old);
                            dq.push_back(n/q_old);
                            next_loop = true; break;
                        }
                        next_loop_inner = true; break;
                    }
                    r <<= 1;
                }
                if (next_loop) break;
            }
            if (next_loop) break;
        }
        if (!next_loop) {
            res.push_back(n);
        }
    }
    sort(res.begin(), res.end());
    vector<pll> res2;
    for (int l = 0, r = 0, sz = res.size(); l < sz;){
        while (r < sz && res[l] == res[r]){
            r++;
        }
        res2.push_back({res[l], r-l});
        l = r;
    }
    return res2;
}

/// @brief 素因数分解を行う。`N`の大きさによって、試し割り法とポラード・ロー法を使い分けてくれる。
constexpr vector<pll> factorize(ull N){
    if (N < 640000ull){
        return trial_division(N);
    }
    return Pollard_rho(N);
}

struct Xorshift64{
    ull state;
    constexpr Xorshift64(ull seed = 881726454633252252ull) : state(seed) {}
    constexpr ull next() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
};

/// @brief mod Pにおける原始根を1つ探す
constexpr ull primitive_root(ull P){
    assert(MillerRabin(P));
    if (P == 2){
        return 1;
    }
    Montgomery64 mg(P);
    Xorshift64 rng(0x1234567E89ABCDEF);
    auto factorized = Pollard_rho(P-1);
    while (true) {
        ull r = rng.next()%P;
        if (r == 0){continue;}
        bool ok = true;
        for (auto& q : factorized){
            if (mg.reduce(mg.pow_not_reduced(r, (P-1)/q[0])) == 1){
                ok = false;
                break;
            }
        }
        if (ok) return r;
    }
}

#endif /* PRIME_AND_DIVISORS_HPP_ */