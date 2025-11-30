#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
#define MOD 998244353
#define INF 9223372036854775807
#define ll long long
#define ld long double
#define vall(A) A.begin(),A.end()
#define gridinput(H,W,vv) for (ll i = 0; i < H; i++){string T; cin >> T; for(ll j = 0; j < W; j++){vv[i][j] = {T[j]};}}
#define adjustedgridinput(H,W,vv) for (ll i = 1; i <= H; i++){string T; cin >> T; for(ll j = 1; j <= W; j++){vv[i][j] = {T[j-1]};}}
#define vin(A,N) for (ll i = 0; i < N; i++){cin >> A[i];}
#define vout(A,N) for (ll i = 0; i < N; i++){cout << A[i] << " \n"[i == N-1]}


/// @brief 正の整数Nを素因数分解する
/// @param N
/// @return vector<vector<ll> {{素因数1,個数}, {素因数2,個数}, {素因数3,個数}...}
vector<vector<ll>> p_fact(ll N){
    if (N == 1){
        return vector<vector<ll>> {{1,1}};
    }
    vector<vector<ll>> R;//戻り値用リスト

    const ll M = N;
    for (ll i = 2; i <= sqrtl(M); i++){
        if (N % i == 0){
            ll divide_count = 0;
            while (N % i == 0){
                divide_count++;
                N /= i;
            }
            R.push_back(vector<ll> {i,divide_count});
        }
    }
    if (N != 1){
        R.push_back(vector<ll> {N,1});
    }
    return R;
}

/// @brief 二次元ベクトル表記の素因数分解リストを受け取って約数の和を求める
/// @param vv 
/// @return 約数の和
ll sum_of_divisor(vector<vector<ll>> vv){
    if (vv[0][0] == 1){
        return 1;
    }
    ll R = 1;
    for (vector<ll> x : vv){
        R *= ((ll)powl(x[0],x[1]+1) - 1)/(x[0] - 1);
    }
    return R;
}

/// @brief 二次元ベクトル表記の素因数分解リストを受け取って約数の種類を求める
/// @param vv 
/// @return 約数の種類
ll kind_of_divisor(vector<vector<ll>> vv){
    ll R = 1;
    for (vector<ll> x : vv){
        R *= x[1]+1;
    }
    return R;
}

/// @brief 1のみを含むデフォルトリストdと、素因数分解の結果pを受け取って、dを約数リストに変換する。
/// @return ない
void search_divisor(vector<ll> &d, vector<vector<ll>> &p, ll depth = 0){
    if (depth == p.size()){
        sort(vall(d));
        return;
    }
    ll t = d.size();
    for (ll i = 1; i <= p[depth][1]; i++){
        for (ll j = 0; j < t; j++){
            d.push_back(d[j]*powll(p[depth][0], i));
        }
    }
    search_divisor(d, p, depth+1);
}

/// @brief 有理数のceilを求める
/// @param y 
/// @param x 
/// @return ceil(y/x)
ll cefrac(ll y, ll x);

/// @brief 有理数のfloorを求める
/// @param y
/// @param x 
/// @return floor(y/x)
ll flfrac(ll y, ll x){
    if (x*y > 0){
        x = abs(x);
        y = abs(y);
        return (y-(y%x))/x;
    }
    else if (x*y < 0){
        x = abs(x);
        y = abs(y);
        return -cefrac(y,x);
    }
    else{
        return y/x;
    }
}

ll cefrac(ll y, ll x){
    if (x*y > 0){
        x = abs(x);
        y = abs(y);
        if (y%x == 0){
            return y/x;
        }
        else return 1 + (y-(y%x))/x;
    }
    else if (x*y < 0){
        x = abs(x);
        y = abs(y);
        return -flfrac(y,x);
    }
    else{
        return y/x;
    }
}


ll flsqrt(ll N){
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

/// @brief エラトステネスの篩
struct Eratosthenes{
    vector<ll> P_List;
    vector<bool> is_prime;
    vector<ll> min_factor;
    Eratosthenes(ll N) : is_prime(N+1,1), min_factor(N+1,-1){
        is_prime[1] = 0;
        min_factor[1] = 1;
        for (ll i = 2; i <= N; i++){
            if (is_prime[i]){
                P_List.push_back(i);
                min_factor[i] = i;
                for (ll j = 2*i; j <= N; j += i){
                    is_prime[j] = 0;
                    if (min_factor[j] == -1){
                        min_factor[j] = i;
                    }
                }
            }
        }
    }

    void chase_prime(const ll &reference, ll x, vector<vector<vector<ll>>> &r){
        if (r[reference].empty() || min_factor[x] != r[reference].back()[0]){
            r[reference].push_back({min_factor[x], 1});
        }
        else{
            r[reference].back()[1]++;
        }

        if (x != min_factor[x]){
            chase_prime(reference, x/min_factor[x], r);
        }
    }

    vector<vector<vector<ll>>> p_fact(ll N){
        vector<vector<vector<ll>>> r(N+1);
        r[1].push_back({1,1});
        for (ll i = 2; i <= N; i++){
            chase_prime(i, i, r);
        }
        return r;
    }

};

/// @brief a^bをmで割った余りを返す。bに関して対数時間で計算できる。
/// @param a 
/// @param b 
/// @param m 
/// @return a^b%m
ll modpow(ll a, ll b, ll m){
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
/// @param a 
/// @param n
/// @param m 
/// @return a^n
ll powll(ll a, ll n){
    ll r = 1;
    for (ll i = 1; i <= n; i++){
        r *= a;
    }
    return r;
}

/// @brief modint
struct mll{
    ll N;
    ll M = 998244353;
    mll(ll x){
        N = x;
    }
    void print(){
        cout << N << endl;
    }
};

mll operator+(mll &a, mll &b){
    return mll((a.N + b.N)%a.M);
}
mll operator-(mll &a, mll &b){
    return mll((a.M + a.N - b.N)%a.M);
}
mll operator*(mll &a, mll &b){
    return mll((a.N * b.N) % a.M);
}

/// @brief 一次不定方程式ax+by=1の解を1つ見つける
/// @param a 
/// @param b 
/// @return {x,y}
pair<ll,ll> axby1(ll a, ll b){
    if (a == 1 or b == 1){
        if (a == 1){
            return {1-b,1};
        }
        else{
            return {1,1-a};
        }
    }
    else if (a>b){
        auto p = axby1(a%b, b);
        return {p.first, p.second - p.first*(a/b)};
    }
    else{
        swap(a,b);
        auto p = axby1(a%b, b);
        return {p.second - p.first*(a/b), p.first};
    }
}

/// @brief 1/a mod Mを求める
/// @param a 
/// @param M 
/// @return 1/a mod M
ll inverse_mod(ll a, ll M){
    if (__gcd(a,M) != 1){
        return -1;
    }
    return (M+(axby1(a,M).first)%M)%M;
}

//階乗前計算による二項係数mod998244353
vector<ll> factorialmod(10000001);
factorialmod[0] = 1;
for (ll i = 1; i <= 10000000; i++){
    factorialmod[i] = (i*factorialmod[i-1])%998244353;
}
auto nCr = [&](ll n, ll r){return (factorialmod[n]*inverse_mod((factorialmod[n-r]*factorialmod[r])%998244353,998244353))%998244353;};