#ifndef PRIME_AND_DIVISORS_HPP_
#define PRIME_AND_DIVISORS_HPP_

#include <array>
#include <vector>
#include <math_functions.hpp>
#include <algorithm>
#include <queue>
using namespace std;
using ll = long long;
using ulll = __uint128_t;
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


ll prime_counting(ll N) {
  ll N2 = sqrt(N);
  ll NdN2 = N/N2;

  vector<ll> hl(NdN2);
  for (int i = 1; i < NdN2; i++) hl[i] = N/i - 1;

  vector<int> hs(N2 + 1);
  for (int i = 0; i <= N2; i++){
    hs[i] = i-1;
  }

  for (int x = 2, pi = 0; x <= N2; ++x) {
    if (hs[x] == hs[x - 1]) continue;
    ll x2 = ll(x) * x;
    ll imax = min(NdN2, N/x2 + 1);
    ll ix = x;
    for (ll i = 1; i < imax; i++) {
      hl[i] -= (ix < NdN2 ? hl[ix] : hs[N/ix]) - pi;
      ix += x;
    }
    for (int n = N2; n >= x2; n--) {
      hs[n] -= hs[n/x] - pi;
    }
    ++pi;
  }
  return hl[1];
}

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


constexpr vector<pll> Pollard_rho(ull N){
    assert(N>0);
    if (N == 1){return {{1,0}};}
    vector<pll> res;
    deque<ull> dq;
    dq.push_back(N);
    while (!dq.empty()){
        ull n = dq.front();
        dq.pop_front();
        while (!MillerRabin(n)){
            if ((n&1) == 0){
                res.emplace_back(2,0);
                while ((n&1) == 0){res.back()[1]++; n>>=1;}
                continue;
            }
            int blocksize = max<int>(1, pow(n, 0.125));
            for (ull c = 1; c <= n; c++){
                ull y = (0b1010110101101011110011ull^c^n)%n;
                ull r = 1;
                ull k = 0;
                ull k_mod_blocksize = 0;
                for (int _ = 0; _ < 63; _++){
                    ull q = 1, q_old = 1;
                    while (k < r){
                        ull x = y;
                        k++;
                        k_mod_blocksize++;
                        y = (y*(ulll)y+c)%n;
                        q = q*(y >= x ? (ulll)(y-x) : (ulll)(x-y))%n;
                        if (k_mod_blocksize == blocksize){
                            k_mod_blocksize -= blocksize;
                            ull g = gcd(q,n);
                            if (g > 1){
                                if (g != n){
                                    dq.push_back(g);
                                    dq.push_back(n/g);
                                    goto next_loop;
                                }
                                else{
                                    y = x;
                                    q = q_old;
                                    for (int i = 0; i < blocksize; i++){
                                        y = (y*(ulll)y+c)%n;
                                        q = q*(y >= x ? (ulll)(y-x) : (ulll)(x-y))%n;
                                        g = gcd(q,n);
                                        if (g > 1){
                                            if (g != n){
                                                dq.push_back(g);
                                                dq.push_back(n/g);
                                                goto next_loop;
                                            }
                                            else{
                                                for (int j = k-blocksize+i+1; j < r; j++){
                                                    y = (y*(ulll)y+c)%n;
                                                }
                                                goto next_loop;
                                            }
                                        }
                                    }
                                }
                            }
                            q_old = q;
                        }
                    }
                }
            }
        }
        res.emplace_back(n,1);
        next_loop:
    }
    return res;
}


#endif /* PRIME_AND_DIVISORS_HPP_ */