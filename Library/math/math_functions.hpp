#ifndef MATH_FUNCTION_HPP_
#define MATH_FUNCTION_HPP_

#include <array>
#include <cmath>
using namespace std;
using ll = long long;
using ull = unsigned long long;



/// @brief a^bをmで割った余りを返す。bに関して対数時間で計算できる。
constexpr ll modpow(ll a, ull b, const ll m){
    ll t = a%m;
    ll ans = 1;
    while (b > 0){
        if (b%2){
            ans = (ans*t)%m;
        }
        b /= 2;
        t = (t*t)%m;
    }
    return ans;
}

/// @brief a^nを返す。bに関して線形時間で計算できる。
constexpr ll powll(ll a, ull n){
    ll r = 1;
    for (ull i = 1; i <= n; i++){
        r *= a;
    }
    return r;
}

/// @brief floor(sqrt(N))を返す
constexpr ll isqrt(ll N){
    if (N){
        ll ok = 1;
        ll ng = min(N,2000000000LL);
        while (ng - ok >= 2){
            ll mid = (ok+ng)/2;
            if (mid*mid <= N){
                ok = mid;
            } 
            else{
                ng = mid;
            }
        }
        return ok;
    }
    else{return 0;}
}

/// @brief floor(log_a(L))を返す
constexpr ll ilog(ll a, ll L){
    __int128_t t = 1;
    ll ans = 0;
    while (t <= L){
        ans++;
        t *= a;
    }
    return ans-1;
}

/// @brief 有理数のfloorを求める
constexpr inline ll floor2(ll y, ll x){
    if ((x^y) > 0){
        x = abs(x);
        y = abs(y);
        return y/x;
    }
    else if ((x^y) < 0){
        x = abs(x);
        y = abs(y);
        return -((y+x-1)/x);
    }
    else{
        return y/x;
    }
}
/// @brief 有理数のceilを求める
constexpr inline ll ceil2(ll y, ll x){
    if ((x^y) > 0){
        x = abs(x);
        y = abs(y);
        return (y+x-1)/x;
    }
    else if ((x^y) < 0){
        x = abs(x);
        y = abs(y);
        return -(y/x);
    }
    else{
        return y/x;
    }
}

/// @brief 一次不定方程式ax+by=1の解を1つ見つける
/// @param a `a>=0`である必要がある
/// @param b `b>=0`である必要がある
/// @return {x,y,gcd(x,y)}
template<typename T>
constexpr array<T,3> axby1(T a, T b){
    T x = 1, y = 0;
    T z = 0, w = 1;
    T tmp = 0;
    while (b){
        T p = a/b, q = a%b;
        tmp = x - y * p; x = y; y = tmp;
        tmp = z - w * p; z = w; w = tmp;
        a = b; b = q;
    }
    return {x, z, a};
}

/// @brief 1/a mod Mを求める
template<typename T, typename U>
constexpr T inverse_mod(T a, U M){
    auto temp = axby1(a,(T)M);
    assert(temp[2] == 1);
    return (M+temp[0])%M;
}

/// @brief sqrt(a) mod Mを求める。ないなら-1が返される。
template<ll M>
constexpr ll cipolla(ll a){
    a %= M;
    if (M == 2) return a;
	if (a == 0) return 0;
    ll z = (M-1)/2;
    if (modpow(a, z, M) != 1){return -1;}
    int b = 0;
    while (modpow((b*b+M-a)%M, z, M) == 1){
        b++;
    }
    array<ll,2> x{1,0};
    array<ll,2> y{b, 1};
    ll w = (b*b+M-a)%M;
    z++;
    while (z){
        if (z&1){
            ll temp = x[0];
            x[0] = x[0]*y[0]%M+x[1]*y[1]%M*w%M;
            if (x[0] >= M){x[0] -= M;}
            x[1] = temp*y[1]%M+x[1]*y[0]%M;
            if (x[1] >= M){x[1] -= M;}
        }
        ll temp = y[0];
        y[0] = y[0]*y[0]%M+y[1]*y[1]%M*w%M;
        if (y[0] >= M){y[0] -= M;}
        y[1] = 2*temp*y[1]%M;
        z >>= 1;
    }
    return x[0];
}
ll cipolla(ll a, const ll M){
    a %= M;
    if (M == 2) return a;
	if (a == 0) return 0;
    ll z = (M-1)/2;
    if (modpow(a, z, M) != 1){return -1;}
    int b = 0;
    while (modpow((b*b+M-a)%M, z, M) == 1){
        b++;
    }
    array<ll,2> x{1,0};
    array<ll,2> y{b, 1};
    ll w = (b*b+M-a)%M;
    z++;
    while (z){
        if (z&1){
            ll temp = x[0];
            x[0] = x[0]*y[0]%M+x[1]*y[1]%M*w%M;
            if (x[0] >= M){x[0] -= M;}
            x[1] = temp*y[1]%M+x[1]*y[0]%M;
            if (x[1] >= M){x[1] -= M;}
        }
        ll temp = y[0];
        y[0] = y[0]*y[0]%M+y[1]*y[1]%M*w%M;
        if (y[0] >= M){y[0] -= M;}
        y[1] = 2*temp*y[1]%M;
        z >>= 1;
    }
    return x[0];
}

/// @brief x以下の最大の2冪を返す。0は0が返る。 
constexpr int lowerpow2(ull x){
    if (x == 0){return 0;}
    return 1ull<<(63-__builtin_clzll(x));
}
/// @brief x以上の最小の2冪を返す。0は0が返る。 
constexpr int upperpow2(ull x){
    if (x == 0){return 0;}
    if (x == 1){return 1;}
    return 1ull<<(64-__builtin_clzll(x-1));
}



#endif /* MATH_FUNCTION_HPP_ */