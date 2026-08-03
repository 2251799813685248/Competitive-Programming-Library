
#ifndef NCR_HPP_
#define NCR_HPP_


#include <utility>
#include <vector>
#include <cmath>
#include <cassert>
#include <math_functions.hpp>
#include <prime_and_divisors.hpp>
#include <CRT.hpp>
using namespace std;
using ll = long long;
using ull = unsigned long long;

#ifndef LEN_DEFINED
#define LEN_DEFINED
template<typename T> inline constexpr ll len(const T& A){return ssize(A);}
#endif


//素数mod限定の階乗前計算による二項係数mod M  Mが実行時定数である必要がある。
//前計算O(N) 取得O(1)
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

//素数mod限定の階乗前計算による二項係数mod M  Mが実行時に与えられても使える
//前計算O(N) 取得O(1)
struct factorialncr2{
    vector<uint> factorialmod;
    vector<uint> factorialmodinv;
    uint N_MAX;
    ull M;
    factorialncr2(const uint N_MAX__, const ull MOD){
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

//任意modの階乗前計算による二項係数mod M  Mが実行時に与えられても使える。
//前計算O(Nlog M) 取得O(log M)
struct factorialncr3{
    uint N_MAX;
    ull M;
    int k_max; //Mが持つ素因数の個数
    vector<vector<uint>> factorial_vp; //i!がp_1, p_2,...,p_kで何回割れるか
    vector<vector<uint>> pow_p; // p_1, p_2,...,p_kの累乗表
    vector<uint> factorialmod;
    vector<uint> factorialmodinv;

    factorialncr3(uint N_MAX__, ull MOD){
        N_MAX = max(N_MAX__, 1u); //0が入ってくるとバグるので調整
        M = MOD;
        vector<uint> M_fact;
        for (auto& p : factorize(M)){
            M_fact.push_back(p[0]);
        }
        k_max = M_fact.size();
        
        //配列の長さを決め、初期化
        factorial_vp = vector<vector<uint>>(N_MAX+1, vector<uint>(k_max, 0));
        pow_p = vector<vector<uint>>(k_max);
        vector<uint> except_M(N_MAX+1);
        
        //累乗数を前計算
        for (int k = 0; k < k_max; k++){
            pow_p[k] = vector<uint>((N_MAX+M_fact[k]-2)/(M_fact[k]-1), 1); //ルジャンドルの定理より、N_MAX!がpで割り切れる回数は最大N/(p-1)回
            ull temp = M_fact[k];
            for (int i = 1, sz = pow_p[k].size(); i < sz; i++){
                pow_p[k][i] = temp;
                temp = (temp*M_fact[k])%M;
            }
        }
        
        
        //i!がp_1, p_2,...,p_kのそれぞれで何回ずつ割れたか、と、i!をp_1, p_2,...,p_kで限界まで割った値をMで割った余りを前計算
        for (uint i = 1; i <= N_MAX; i++){
            uint i2 = i; //iを直接いじるとループが壊れるので別の変数を利用
            for (int k = 0; k < k_max; k++){
                factorial_vp[i][k] = factorial_vp[i-1][k]; //階乗なので、指数は累積和になる。一個前の値を今の値に足しておく
                while (i2%M_fact[k] == 0){
                    factorial_vp[i][k]++;
                    i2 /= M_fact[k];
                }
            }
            except_M[i] = i2%M; //限界まで割って、残った部分を記録
        }
        
        //配列を初期化
        factorialmod = vector<uint>(N_MAX+1);
        factorialmodinv = vector<uint>(N_MAX+1);
        factorialmod[0] = 1;
        factorialmodinv[0] = 1;
        
        ull temp = 1;
        for (uint i = 1; i <= N_MAX; i++){
            temp = (temp*except_M[i])%M;
            factorialmod[i] = temp;
        }
        temp = inverse_mod(factorialmod[N_MAX], M);
        for (uint i = N_MAX; i > 0; i--){
            factorialmodinv[i] = temp;
            temp = (temp*except_M[i])%M;
        }
    }
    //計算量はO(log M)
    inline ll nCr(ll n, ll r){
        assert(n <= N_MAX);
        if (r < 0 || n < r){
            return 0;
        }
        ull res = 1; //除算はullの方が速いので途中経過はullで計算する
        //Mと互いに素でない部分は指数の引き算をして約分し、答えにかけておく
        for (int k = 0; k < k_max; k++){
            res = res*pow_p[k][factorial_vp[n][k]-factorial_vp[r][k]-factorial_vp[n-r][k]]%M;
        }
        //Mと互いに素な部分は逆元計算
        return res*factorialmod[n]%M*factorialmodinv[r]%M*factorialmodinv[n-r]%M;
    }
};

//任意modの階乗前計算による二項係数mod M  Mが実行時に与えられても使える。
//前計算O(M) 取得O(log N log M)
struct factorialncr4{
    ull M;
    int k_max;//Mが持つ素因数の個数
    vector<vector<uint>> factorialmod; //n! mod p**q
    vector<vector<uint>> factorialmodinv; //n!^-1 mod p**q
    vector<vector<uint>> pow_pi; //pごとの{p^i}のリスト
    vector<ll> crt_mod; //pごとのp^qのリスト
    vector<ll> crt_rem;

    factorialncr4(ull MOD){
        M = MOD;
        for (auto& p : factorize(M)){
            ull pq = powll(p[0], p[1]);
            vector<uint> powlist(p[1]+1); //累乗リスト
            powlist[0] = 1;
            for (uint i = 1; i <= p[1]; i++){
                powlist[i] = powlist[i-1]*p[0];
            }
            crt_mod.push_back(powlist.back());
            pow_pi.push_back(std::move(powlist));
            vector<uint> temp(pq); //階乗逆元
            temp[0] = 1;
            for (ull i = 1; i < pq; i++){
                if (i%p[0] == 0){
                    temp[i] = 1;
                }
                else{
                    temp[i] = i;
                }
            }
            vector<uint> temp2(pq); //階乗
            temp2[0] = 1;
            for (ull i = 1; i < pq; i++){
                temp2[i] = temp2[i-1]*(ull)temp[i]%pq;
            }
            ull temp3 = inverse_mod<ll>(temp2[pq-1], pq);
            temp[0] = 1;
            for (ull i = pq-1; i > 0; i--){
                uint temp4 = temp3;
                temp3 = temp[i]*temp3%pq;
                temp[i] = temp4;
            }
            factorialmod.push_back(std::move(temp2));
            factorialmodinv.push_back(std::move(temp));
        }
        k_max = len(crt_mod);
        crt_rem.resize(k_max, 0ll);
    }
    private: inline ull nCrmodpq(ll n, ll r, int k){
        ll vp = 0;
        ll nn = n/pow_pi[k][1], rr = r/pow_pi[k][1], mm = (n-r)/pow_pi[k][1];
        while (nn){
            vp += nn-rr-mm;
            nn /= pow_pi[k][1];
            rr /= pow_pi[k][1];
            mm /= pow_pi[k][1];
        }
        vp = vp < len(pow_pi[k])-1 ? pow_pi[k][vp] : 0;
        if (vp == 0){return 0ull;}
        ll vq = 0;
        nn = n/pow_pi[k].back();
        rr = r/pow_pi[k].back();
        mm = (n-r)/pow_pi[k].back();
        while (nn){
            vq += nn-rr-mm;
            nn /= pow_pi[k][1];
            rr /= pow_pi[k][1];
            mm /= pow_pi[k][1];
        }
        nn = n;
        rr = r;
        mm = n-r;
        ull res = vp;
        ull temp_mod = pow_pi[k].back();
        while (nn){
            res = res*factorialmod[k][nn%temp_mod]%temp_mod*factorialmodinv[k][rr%temp_mod]%temp_mod*factorialmodinv[k][mm%temp_mod]%temp_mod;
            nn /= pow_pi[k][1];
            rr /= pow_pi[k][1];
            mm /= pow_pi[k][1];
        }
        if (!(pow_pi[k][1] == 2 && len(pow_pi[k]) > 3)){
            if (res != 0 && (vq&1)){
                res = temp_mod-res;
            }
        }
        return res;
    }
    public: ll nCr(ll n, ll r){
        if (r < 0 || n < r || M == 1){
            return 0;
        }
        for (int i = 0; i < k_max; i++){
            crt_rem[i] = nCrmodpq(n, r, i);
        }
        return crt_relatively_prime(crt_rem, crt_mod);
    }
};

//任意modの表の前計算による二項係数modM
struct tablencr{
    vector<vector<ll>> ncrmodlist;
    ll N_MAX;
    tablencr(const ll N_MAX__, const ll M){
        N_MAX = N_MAX__;
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
        assert(n <= N_MAX);
        if (r < 0 || n < r){
            return 0;
        }
        return ncrmodlist[n][r];
    }
};




#endif /* NCR_HPP_ */
