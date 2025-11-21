#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <functional>

using namespace std;
#define ll long long



/// @brief sum[i=1 ~ i=N] i^a * (floor(M/i))^b を計算する
/// @param N 
/// @param M 
/// @param a 
/// @param b 
/// @return 
ll inv_floor_sum(ll N, ll M, ll a ,ll b){
    auto floorsqrt = [](ll N){
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
    };
    auto lpowl = [](ll x, ll N){
        ll r = 1;
        for (ll i = 1; i <= N; i++){
            r *= x;
        }
        return r;
    };
    
    vector<vector<ll>> nCrtable(a+2,vector<ll>(a+2,0));
    nCrtable[0][0] = 1;
    for (ll i = 1; i <= a+1; i++){
        for (ll j = 0; j <= i; j++){
            if (j == 0 || j == i){
                nCrtable[i][j] = 1;
            }
            else{
                nCrtable[i][j] = nCrtable[i-1][j-1] + nCrtable[i-1][j];
            }
        }
    }

    function<ll(ll,ll)> sum = [&](ll n, ll l){
        if (l == 0){
            return n;
        }
        if (l == 1){
            return n*(n+1)/2;
        }
        if (l == 2){
            return n*(n+1)*(2*n+1)/6;
        }
        ll T = 0;
        for (ll i = 0; i < l; i++){
            T += nCrtable[l+1][i]*sum(n,i);
        }
        return (lpowl(n+1,l+1)-1-T)/(l+1);
    };


    ll ans = 0;
    ll rootM = floorsqrt(M);
    
    if (N <= M / rootM){//Nが小さいときはそのまま計算して値を返す
        for (ll i = 1; i <= N; i++){
            ans += lpowl(i,a)*lpowl(M/i,b);
        }
        return ans;
    }

    //それ以外の場合は主客転倒を行う

    for (ll i = 1; i <= M/rootM; i++){//最初の方をそのまま足す。
        ans += lpowl(i,a)*lpowl(M/i,b);
    }
    for (ll i = rootM-1; i > M/N; i--){
        ans += lpowl(i,b)*(sum(M/i,a) - sum(M/(i+1),a));
    }
    ans += lpowl(M/N,b)*(sum(N,a) - sum(M/((M/N)+1), a));

    return ans;
}