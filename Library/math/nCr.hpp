
#ifndef NCR_HPP_
#define NCR_HPP_

#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <math_functions.hpp>
using namespace std;
using ll = long long;
using ull = unsigned long long;


//階乗前計算による二項係数mod M  Mが複数欲しいときにも使える
template<ull M>
struct factorialncr{
    vector<uint> factorialmod;
    vector<uint> factorialmodinv;
    uint N_MAX;
    factorialncr(const uint N_MAX__){
        N_MAX = max(1u, N_MAX__);
        factorialmod = vector<uint>(N_MAX+1);
        factorialmodinv = vector<uint>(N_MAX+1);
        factorialmod[0] = 1;
        factorialmodinv[0] = 1;
        ull temp = 1;
        for (ull i = 1; i <= N_MAX; i++){
            temp = (i*temp)%M;
            factorialmod[i] = temp;
        }
        factorialmodinv[N_MAX] = inverse_mod((ll)factorialmod[N_MAX], M);
        temp = factorialmodinv[N_MAX];
        for (ull i = N_MAX-1; i > 0; i--){
            temp = ((i+1)*temp)%M;
            factorialmodinv[i] = temp;
        }
    }

    ll nCr(ll n, ll r){
        assert(n <= N_MAX);
        if (r < 0 || n < r){
            return 0;
        }
        return factorialmod[n]*(ull)factorialmodinv[r]%M*factorialmodinv[n-r]%M;
    }
};

//階乗前計算による二項係数mod M  Mが実行時に与えられても使える
struct dynamicfactorialncr{
    vector<uint> factorialmod;
    vector<uint> factorialmodinv;
    uint N_MAX;
    ull M;
    dynamicfactorialncr(const uint N_MAX__, const ull MOD){
        N_MAX = max(1u, N_MAX__);
        M = MOD;
        factorialmod = vector<uint>(N_MAX+1);
        factorialmodinv = vector<uint>(N_MAX+1);
        factorialmod[0] = 1;
        factorialmodinv[0] = 1;
        ull temp = 1;
        for (ull i = 1; i <= N_MAX; i++){
            temp = (i*temp)%M;
            factorialmod[i] = temp;
        }
        factorialmodinv[N_MAX] = inverse_mod((ll)factorialmod[N_MAX], M);
        temp = factorialmodinv[N_MAX];
        for (ull i = N_MAX-1; i > 0; i--){
            temp = ((i+1)*temp)%M;
            factorialmodinv[i] = temp;
        }
    }

    ll nCr(ll n, ll r){
        assert(n <= N_MAX);
        if (r < 0 || n < r){
            return 0;
        }
        return factorialmod[n]*(ull)factorialmodinv[r]%M*factorialmodinv[n-r]%M;
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
                    ncrmodlist[i][j] = ncrmodlist[i-1][j-1] + ncrmodlist[i-1][j];
                    if (ncrmodlist[i][j] >= M){
                        ncrmodlist[i][j] -= M;
                    }
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
