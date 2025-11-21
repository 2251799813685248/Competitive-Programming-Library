#include <bits/stdc++.h>
using namespace std;
using ll = long long;
ll MOD = 998244353;


//64bit整数行列
struct matrixll{
    vector<vector<ll>> M;
    ll H,W;
    /// @brief N次単位行列を生成
    /// @param N 
    matrixll(ll N){
        H = N;W = N;
        M = vector<vector<ll>>(N,vector<ll>(N,0));
        for (ll i = 0; i < N; i++){
            M[i][i] = 1;
        }
    }
    /// @brief h×w型の、全要素がvの行列を生成
    /// @param h 
    /// @param w 
    /// @param v 
    matrixll(ll h, ll w, ll v){
        H = h;
        W = w;
        M = vector<vector<ll>>(H,vector<ll>(W,v));
    }
    /// @brief 2次元配列を用いて行列を生成
    /// @param A 
    matrixll(vector<vector<ll>> &A){
        M = A;
        H = A.size();
        W = A[0].size();
    }
    matrixll operator+(const matrixll &T){
        if (H != T.H  || W != T.W){
            cerr << "size error\n";
            assert(false);
        }
        matrixll ans(M);
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                ans.M[i][j] += T.M[i][j];
                ans.M[i][j] %= MOD;
            }
        }
        return ans;
    }
    void operator+=(const matrixll &T){
        if (H != T.H  || W != T.W){
            cerr << "size error\n";
            assert(false);
        }
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                M[i][j] += T.M[i][j];
                M[i][j] %= MOD;
            }
        }
    }
    matrixll operator-(const matrixll &T){
        if (H != T.H  || W != T.W){
            cerr << "size error\n";
            assert(false);
        }
        matrixll ans(M);
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                ans.M[i][j] -= T.M[i][j];
                ans.M[i][j] += MOD;
                ans.M[i][j] %= MOD;
            }
        }
        return ans;
    }
    void operator-=(const matrixll &T){
        if (H != T.H  || W != T.W){
            cerr << "size error\n";
            assert(false);
        }
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                M[i][j] -= T.M[i][j];
                M[i][j] += MOD;
                M[i][j] %= MOD;
            }
        }
    }
    matrixll operator*(const matrixll &T){
        if (W != T.H){
            cerr << "size error\n";
            assert(false);
        }
        matrixll ans(H,T.W,0);
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < T.W; j++){
                for (ll k = 0; k < W; k++){
                    ans.M[i][j] += M[i][k]*T.M[k][j];
                    ans.M[i][j] %= MOD;
                }
            }
        }
        return ans;
    }
    void operator*=(const matrixll &T){
        if (W != T.H){
            cerr << "size error\n";
            assert(false);
        }
        matrixll ans(H,T.W,0);
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < T.W; j++){
                for (ll k = 0; k < W; k++){
                    ans.M[i][j] += M[i][k]*T.M[k][j];
                    ans.M[i][j] %= MOD;
                }
            }
        }
        W = T.W;
        M = ans.M;
    }
    matrixll operator*(const ll &c){
        matrixll ans(H,W,0);
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                ans.M[i][j] = M[i][j] * c;
                ans.M[i][j] %= MOD;
            }
        }
        return ans;
    }
    void operator*=(const ll &c){
        for (ll i = 0; i < H; i++){
            for (ll j = 0; j < W; j++){
                M[i][j] *= c;
                M[i][j] %= MOD;
            }
        }
    }
};


/// @brief A^Nを返す。
/// @param A 
/// @param N 
/// @return A^N
matrixll matrixmodpow(matrixll A, ll N, const ll &M){
    matrixll R(A.H);
    for (ll i = 0; i < A.H; i++){
        for (ll j = 0; j < A.H; j++){
            A.M[i][j] %= M;
        }
    }
    if (N){
        while (N){
            if (N%2){
                R *= A;
            }
            A *= A;
            for (ll i = 0; i < A.H; i++){
                for (ll j = 0; j < A.H; j++){
                    A.M[i][j] %= M;
                }
            }
            N /= 2;
        }
        return R;
    }
    else{
        return R;
    }
}