#ifndef CRT_HPP_
#define CRT_HPP_


#include <array>
#include <cmath>
#include <cassert>
#include <vector>
#include <math_functions.hpp>

using namespace std;
using ll = long long;
using ld = long double;
using ull = unsigned long long;
#ifdef BOOST_VERSION
using bll = boost::multiprecision::cpp_int;
#endif
#ifndef LEN_DEFINED
#define LEN_DEFINED
template<typename T> inline ll len(T A){return ssize(A);}
#endif

// Garnerのアルゴリズムで元の数を復元する。すべての法同士が互いに素である必要がある。解のうち最小の非負整数が返る。
template<typename T> constexpr T crt_relatively_prime(const vector<T>& r, const vector<T>& m){
    T ans = 0;
    int N = len(r);
    assert(N == len(m));
    vector<T> A(N);
    for (int i = 0; i < N; i++){
        ll b = r[i]%m[i];
        ll cumulative_prod = 1;
        for (int k = 0; k < i; k++){
            b += m[i]-cumulative_prod%m[i]*A[k]%m[i];
            if (b >= m[i]){b -= m[i];}
            cumulative_prod *= m[k];
        }
        A[i] = b*inverse_mod<T>(cumulative_prod, m[i])%m[i];
        ans += A[i]*cumulative_prod;
    }
    return ans;
}

// Garnerのアルゴリズムで元の数を復元する。すべての法同士が互いに素である必要がある。解のうち最小の非負整数%MODが返る。
template<typename T> constexpr T crt_relatively_prime(const vector<T>& r, const vector<T>& m, T MOD){
    T ans = 0;
    int N = len(r);
    assert(N == len(m));
    vector<T> A(N);
    for (int i = 0; i < N; i++){
        ll b = r[i]%m[i];
        ll cumulative_prod1 = 1;
        ll cumulative_prod2 = 1;
        for (int k = 0; k < i; k++){
            b += m[i]-cumulative_prod1*A[k]%m[i];
            if (b >= m[i]){b -= m[i];}
            cumulative_prod1 = cumulative_prod1*m[k]%m[i];
            cumulative_prod2 = cumulative_prod2*m[k]%MOD;
        }
        A[i] = b*inverse_mod<T>(cumulative_prod1, m[i])%m[i];
        ans += A[i]*cumulative_prod2%MOD;
        if (ans >= MOD){
            ans -= MOD;
        }
    }
    return ans;
}

#ifdef BOOST_VERSION
// Garnerのアルゴリズムで元の数を復元する。すべての法同士が互いに素である必要がある。解のうち最小の非負整数が返る。多倍長整数版
constexpr bll crt_relatively_prime_large(const vector<bll>& r, const vector<bll>& m){
    bll ans = 0;
    int N = len(r);
    assert(N == len(m));
    vector<bll> A(N);
    for (int i = 0; i < N; i++){
        bll b = r[i]%m[i];
        bll cumulative_prod = 1;
        for (int k = 0; k < i; k++){
            b += m[i]-cumulative_prod%m[i]*A[k]%m[i];
            if (b >= m[i]){b -= m[i];}
            cumulative_prod *= m[k];
        }
        A[i] = b*inverse_mod<bll>(cumulative_prod, m[i])%m[i];
        ans += A[i]*cumulative_prod;
    }
    return ans;
}
#endif

template<typename T> inline constexpr T internal_arrange_mod(T m1, T m2, T g){
    g = gcd(g, m2/g);
    T temp = m1;
    do{
        temp /= g;
        g = gcd(g, temp);
    }while(g > 1);
    return temp;
}

// Garnerのアルゴリズムで元の数を復元する。存在しなければ-1が返り、存在するならばありうるもののうち、最小の非負整数が返る。
//modの最小公倍数も求まる。
template<typename T> constexpr array<T, 2> crt(vector<T> r, vector<T> m){
    int N = len(r);
    assert(N == len(m));
    for (int i = 0; i < N-1; i++){
        for (int j = i+1; j < N; j++){
            T g = gcd(m[i], m[j]);
            if ((r[i]-r[j])%g){
                return {-1, -1};
            }
            m[i] = internal_arrange_mod(m[i], m[j], g);
            m[j] /= gcd(m[i], g);
        }
    }
    T prod = 1;
    for (int i = 0; i < N; i++){
        r[i] %= m[i];
        prod *= m[i];
    }
    return {crt_relatively_prime<T>(r, m), prod};
}
// Garnerのアルゴリズムで元の数を復元する。存在しなければ-1が返り、存在するならばありうるもののうち、最小の非負整数%MODが返る。
//modの最小公倍数も求まる。
template<typename T> constexpr array<T, 2> crt(vector<T> r, vector<T> m, T MOD){
    int N = len(r);
    assert(N == len(m));
    for (int i = 0; i < N-1; i++){
        for (int j = i+1; j < N; j++){
            T g = gcd(m[i], m[j]);
            if ((r[i]-r[j])%g){
                return {-1, -1};
            }
            m[i] = internal_arrange_mod(m[i], m[j], g);
            m[j] /= gcd(m[i], g);
        }
    }
    T prod = 1;
    for (int i = 0; i < N; i++){
        r[i] %= m[i];
        prod = prod*m[i]%MOD;
    }
    return {crt_relatively_prime<T>(r, m, MOD), prod};
}


#endif /* CRT_HPP_ */