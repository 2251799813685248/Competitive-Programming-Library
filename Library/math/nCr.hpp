#ifndef NCR_HPP_
#define NCR_HPP_

#include <vector>
#include <string>
#include <cmath>
using namespace std;
using ll = long long;



//階乗前計算による二項係数mod998244353
struct factorialncr{
    vector<ll> factorialmod;
    vector<ll> factorialmodinv;
    ll N_MAX_N_MAX;
    ll MOD;
    factorialncr(const ll N_MAX, const ll M){
        N_MAX_N_MAX = max(1ll, N_MAX);
        MOD = M;
        factorialmod = vector<ll>(N_MAX+1);
        factorialmodinv = vector<ll>(N_MAX+1);
        factorialmod[0] = 1;
        factorialmod[1] = 1;
        factorialmodinv[0] = 1;
        factorialmodinv[1] = 1;
        for (int i = 2; i <= N_MAX; i++){
            factorialmod[i] = (i*factorialmod[i-1])%M;
            factorialmodinv[i] = (M-factorialmodinv[M%i]*(M/i)%M)%M;
        }
        for (int i = 1; i <= N_MAX; i++){
            factorialmodinv[i] = (factorialmodinv[i]*factorialmodinv[i-1])%M;
        }
    }

    ll nCr(ll n, ll r){
        if (r < 0 || n < r || n > N_MAX_N_MAX){
            return 0;
        }
        return factorialmod[n]*factorialmodinv[r]%MOD*factorialmodinv[n-r]%MOD;
    }
};

//表の前計算による二項係数modM
struct tablencr{
    vector<vector<ll>> ncrmodlist;
    ll N_MAX_N_MAX;
    public:
    tablencr(const ll N_MAX, const ll M){
        N_MAX_N_MAX = N_MAX;
        ncrmodlist = vector<vector<ll>> (N_MAX+1, vector<ll>(N_MAX+1,0));
        ncrmodlist[0][0] = 1;
        for (int i = 1; i <= N_MAX; i++){
            for (int j = 0; j <= i; j++){
                if (j == 0 || j == i){
                    ncrmodlist[i][j] = 1;
                }
                else{
                    ncrmodlist[i][j] = (ncrmodlist[i-1][j-1] + ncrmodlist[i-1][j])%M;
                }
            }
        }
    }
    ll nCr(ll n, ll r){
        if (r < 0 || n < r || n > N_MAX_N_MAX){
            return 0;
        }
        return ncrmodlist[n][r];
    }
};




#endif /* NCR_HPP_ */