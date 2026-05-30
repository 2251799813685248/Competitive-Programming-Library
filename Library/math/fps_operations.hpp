#ifndef FPS_OPERATION_HPP_
#define FPS_OPERATION_HPP_

#include <vector>
#include <algorithm>
#include <math_functions.hpp>
#include <fps.hpp>
#include <inv_table.hpp>
using namespace std;
using ll = long long;
using uint = unsigned;

template<typename T> inline bool btest_for_fps(T K, int i){return K&(1ull<<i);}


/// @brief mod M上での形式的冪級数の計算を行う構造体
/// @attention expやpowなどの関数を使用する際は、inv_table_initalizedがtrueである必要がある。初期化されていない場合は、inv_tableを使用する関数を呼び出す前にinit_inv_table()を呼び出すこと。
template<ull M>
struct fps_operator{
    ull sum_e[30];
    ull sum_ie[30];
    ull inv_table[1048576];
    bool inv_table_initalized = false;
    constexpr fps_operator(bool use_inv_table = false){
        vector<ll> powroot{1};
        vector<ll> powrootinv;
        while (powroot.back() >= 0){
            powroot.push_back(cipolla<M>(powroot.back()));
            if (powroot.back() == powroot[powroot.size()-2]){
                powroot.back() = M-powroot.back();
            }
        }
        powroot.pop_back();
        for (auto v : powroot){
            powrootinv.push_back(inverse_mod(v,M));
        }
        int cnt2 = powroot.size()-1;
        ull now = 1;
        for (int i = 0; i <= cnt2-2; i++){
            sum_e[i] = (powroot[i+2]*now)%M;
            now = (now*powrootinv[i+2])%M;
        }
        ull inow = 1;
        for (int i = 0; i <= cnt2-2; i++){
            sum_ie[i] = (powrootinv[i+2]*inow)%M;
            inow = (inow*powroot[i+2])%M;
        }
        for (int i = cnt2-1; i < 30; i++){
            sum_e[i] = 0;
            sum_ie[i] = 0;
        }
        if (use_inv_table) init_inv_table();
    }
    constexpr void init_inv_table(){
        inv_table_initalized = true;
        inv_table[0] = 0;
        inv_table[1] = 1;
        for(int i = 2; i < 1048576; i++) inv_table[i] = inv_table[M%i]*(M-M/i)%M;
    }
    FormalPowerSeries add(const FormalPowerSeries& F1, const FormalPowerSeries& F2) const {
        FormalPowerSeries ret(max(F1.sz, F2.sz));
        if (F1.sz < F2.sz){
            for (uint i = 0; i < F1.sz; i++){
                ret[i] = (F1[i]+F2[i])%M;
            }
            for (uint i = F1.sz; i < F2.sz; i++){
                ret[i] = F2[i];
            }
        }
        else{
            for (uint i = 0; i < F2.sz; i++){
                ret[i] = (F1[i]+F2[i])%M;
            }
            for (uint i = F2.sz; i < F1.sz; i++){
                ret[i] = F1[i];
            }
        }
        return ret;
    }
    FormalPowerSeries subtract(const FormalPowerSeries& F1, const FormalPowerSeries& F2) const {
        FormalPowerSeries ret(max(F1.sz, F2.sz));
        if (F1.sz < F2.sz){
            for (uint i = 0; i < F1.sz; i++){
                ret[i] = (M+F1[i]-F2[i])%M;
            }
            for (uint i = F1.sz; i < F2.sz; i++){
                ret[i] = (M-F2[i])%M;
            }
        }
        else{
            for (uint i = 0; i < F2.sz; i++){
                ret[i] = (M+F1[i]-F2[i])%M;
            }
            for (uint i = F2.sz; i < F1.sz; i++){
                ret[i] = F1[i];
            }
        }
        return ret;
    }
    void inplaceDFT(FormalPowerSeries& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        for (int ph = 1; ph <= h; ph++) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            ull now = 1;
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; i++) {
                    uint l = F[i + offset];
                    uint r = F[i + offset + p]*now%M;
                    F[i + offset] = l+r<M ? l+r : l+r-M;
                    F[i + offset + p] = l<r ? l+M-r : l-r;
                }
                now = now*sum_e[__builtin_ctz(~s)]%M;
            }
        }
    }
    void inplaceIDFT(FormalPowerSeries& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        for (int ph = h; ph >= 1; ph--) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            ull inow = 1;
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; i++) {
                    uint l = F[i + offset];
                    uint r = F[i + offset + p];
                    F[i + offset] = l+r<M ? l+r : l+r-M;
                    F[i + offset + p] = (l<r ? l+M-r : l-r)*inow%M;
                }
                inow = inow*sum_ie[__builtin_ctz(~s)]%M;
            }
        }
    }
    /// @brief 多項式の積を求める。 
    FormalPowerSeries convolution(FormalPowerSeries F1, FormalPowerSeries F2) const {
        int n = F1.size();
        int m = F2.size();
        if (n == 0 || m == 0) return FormalPowerSeries(0);
        if (min(n, m) <= 60){
            if (n < m) {
                swap(n, m);
                swap(F1.sz, F2.sz);
                swap(F1.f, F2.f);
            }
            FormalPowerSeries ans(n+m-1);
            for (int i = 0; i < n; i++){
                for (int j = 0; j < m; j++){
                    ans[i+j] = (ans[i+j] + F1[i]*(ull)F2[j])%M;
                }
            }
            return ans;
        }
        int reference_size = upperpow2(n+m-1);
        F1.resize(reference_size);
        F2.resize(reference_size);
        inplaceDFT(F1);
        inplaceDFT(F2);
        for (int i = 0; i < reference_size; i++){
            F1[i] = F1[i]*(ull)F2[i]%M;
        }
        inplaceIDFT(F1);
        ull iz = inverse_mod((ll)reference_size, M);
        for (int i = 0; i < reference_size; i++){
            F1[i] = F1[i]*iz%M;
        }
        F1.resize(n+m-1);
        return F1;
    }
    /// @brief F*G == 1 mod x^n となるGを求める。F[x^0] != 0 が必要 
    FormalPowerSeries inv(const FormalPowerSeries& F, const int n) const {
        int sz_f = F.sz;
        FormalPowerSeries res{inverse_mod((ll)F[0], M)};
        for(int d = 1; d < n; d<<=1){
            FormalPowerSeries f(2*d), g(2*d);
            for(int j = 0; j < 2*d; j++) f[j] = (j < sz_f ? F[j] : 0);
            for(int j = 0; j < d; j++) g[j] = res[j];
            inplaceDFT(f);
            inplaceDFT(g);
            for(int j = 0; j < 2*d; j++) f[j] = f[j]*(ull)g[j]%M;
            inplaceIDFT(f);
            ull iz = inverse_mod((ll)2*d, M);
            for (int i = 0; i < 2*d; i++){
                f[i] = f[i]*iz%M;
            }
            for(int j = 0; j < d; j++){
                f[j] = 0;
                if (f[j+d] > 0) f[j+d] = M-f[j+d];
            }
            inplaceDFT(f);
            for(int j = 0; j < 2*d; j++) f[j] = f[j]*(ull)g[j]%M;
            inplaceIDFT(f);
            for (int i = 0; i < 2*d; i++){
                f[i] = f[i]*iz%M;
            }
            for(int j = 0; j < d; j++) f[j] = res[j];
            res = f;
        }
        res.resize(n);
        return res;
    }
    /// @brief exp(F) mod x^n を求める。F[x^0] == 0 が必要
    FormalPowerSeries exp(const FormalPowerSeries& F, const int n){
        assert(inv_table_initalized);
        assert(F[0] == 0);
        ull sz_f = F.sz;
        FormalPowerSeries F0{1},f0{1},g0{1};
        for(int d = 1; d < n; d<<=1){
            auto G0 = g0;
            inplaceDFT(G0);
            FormalPowerSeries Delta(d);
            for(int j = 0; j < d; j++) Delta[j] = F0[j]*(ull)G0[j]%M;
            inplaceIDFT(Delta);
            ull iz = inverse_mod((ll)d, M);
            for (int i = 0; i < d; i++) Delta[i] = Delta[i]*iz%M;
            if (Delta[0] == 0) Delta[0] = M;
            Delta[0] -= 1;
            FormalPowerSeries delta(2*d);
            for(int j = 0; j < d; j++) delta[d+j] = Delta[j];
            FormalPowerSeries epsilon(2*d);

            FormalPowerSeries DF0(d-1);
            for (ull j = 0, ulld = d; j < ulld-1; j++) DF0[j] = f0[j+1]*(j+1)%M;
            DF0.f.push_back(0);
            DF0.sz++;
            inplaceDFT(DF0);
            for(int j = 0; j < d; j++) DF0[j] = DF0[j]*(ull)G0[j]%M;
            inplaceIDFT(DF0);
            for (int i = 0; i < d; i++) DF0[i] = DF0[i]*iz%M;
            for(ull j = 0, ulld = d; j < ulld-1; j++){
                epsilon[j] += (j+1 < sz_f ? F[j+1] : 0)*(j+1)%M;
                if (epsilon[j] >= M) epsilon[j] -= M;
                epsilon[j+d] += DF0[j];
                if (epsilon[j+d] >= M) epsilon[j+d] -= M;
                epsilon[j+d] += M-(j+1 < sz_f ? F[j+1] : 0)*(j+1)%M;
                if (epsilon[j+d] >= M) epsilon[j+d] -= M;
            }
            epsilon[d-1] += DF0[d-1];
            if (epsilon[d-1] >= M) epsilon[d-1] -= M;
            Delta = delta;
            inplaceDFT(Delta);
            FormalPowerSeries DH0(d-1);
            for (ull j = 0, limit = d-1; j < limit; j++) DH0[j] = (j+1 < sz_f ? F[j+1] : 0)*(j+1)%M;
            DH0.resize(2*d);
            inplaceDFT(DH0);
            for(int j = 0; j < 2*d; j++) Delta[j] = Delta[j]*(ull)DH0[j]%M;
            inplaceIDFT(Delta);
            iz = inverse_mod((ll)2*d, M);
            for (int i = 0; i < 2*d; i++) Delta[i] = Delta[i]*iz%M;
            for(int j = 0; j < d; j++){
                epsilon[j+d] += M-Delta[j+d];
                if (epsilon[j+d] >= M){
                    epsilon[j+d] -= M;
                }
            }
        
            for (ull i = 2*d-1; i >= 1; i--){
                epsilon[i] = epsilon[i-1]*inv_table[i]%M+M-(i < sz_f ? F[i] : 0);
                if (epsilon[i] >= M) epsilon[i] -= M;
            }
            epsilon[0] = (F[0] != 0 ? M-F[0] : 0);
            
            auto Epsilon = epsilon;
            inplaceDFT(Epsilon);
            for (int j = 0; j < d; j++){DH0[j] = f0[j]; DH0[j+d] = 0;}
            inplaceDFT(DH0);
            for (int j = 0; j < 2*d; j++) Epsilon[j] = Epsilon[j]*(ull)DH0[j]%M;
            inplaceIDFT(Epsilon);
            for (int i = 0; i < 2*d; i++) Epsilon[i] = Epsilon[i]*iz%M;
            f0.resize(2*d);
            for (int j = 0; j < d; j++){
                f0[j+d] += M-Epsilon[j+d];
                if (f0[j+d] >= M) f0[j+d] -= M;
            }
            if(2*d >= n) break;
            
            G0.resize(2*d);
            for (int j = 0; j < d; j++) G0[j] = g0[j];
            inplaceDFT(G0);
            F0 = f0;
            inplaceDFT(F0);
            FormalPowerSeries T(2*d);
            for (int j = 0; j < 2*d; j++) T[j] = F0[j]*(ull)G0[j]%M;
            inplaceIDFT(T);
            for (int i = 0; i < 2*d; i++) T[i] = T[i]*iz%M;
            for (int j = 0; j < d; j++){
                T[j] = 0;
                if (T[j+d] != 0) T[j+d] = M-T[j+d];
            }
            inplaceDFT(T);
            for (int j = 0; j < 2*d; j++) T[j] = T[j]*(ull)G0[j]%M;
            inplaceIDFT(T);
            for (int i = 0; i < 2*d; i++) T[i] = T[i]*iz%M;
            for (int j = 0; j < d; j++) T[j] = g0[j];
            g0 = T;
        }
        f0.resize(n);
        return f0;
    }
    /// @brief log(F) mod x^n を求める。F[x^0] == 1 が必要 
    FormalPowerSeries log(const FormalPowerSeries& F, const int n){
        assert(F[0] == 1);
        auto DF = F;
        for (ull i = 0, limit = DF.sz-1; i < limit; i++){
            DF[i] = DF[i+1]*(i+1)%M;
        }
        DF.f.pop_back();
        DF.sz--;
        auto Finv = inv(F, n);
        auto res = convolution(DF, Finv);
        res.resize(n);
        for (int i = n-1; i >= 1; i--){
            res[i] = res[i-1]*inv_table[i]%M;
        }
        res[0] = 0;
        return res;
    }
    /// @brief F^k mod x^n を求める。
    FormalPowerSeries pow(const FormalPowerSeries& F, ull k, const int n){
        if (k == 0){
            return FormalPowerSeries{1};
        }
        uint lowest_deg = 0;
        ull lowest_coef = 0;
        while (lowest_deg < F.sz && F[lowest_deg] == 0){
            lowest_deg++;
        }
        if (lowest_deg == F.sz) return FormalPowerSeries(n);
        lowest_coef = F[lowest_deg];
        ull iz = inverse_mod((ll)lowest_coef, M);
        FormalPowerSeries G(n);
        for (uint i = lowest_deg, limit = min(F.sz, lowest_deg + n); i < limit; i++){
            G[i-lowest_deg] = F[i]*iz%M;
        }
        auto logG = log(G, n);
        ull kmodm=k%M;
        for (int i = 0; i < n; i++){
            logG[i] = logG[i]*kmodm%M;
        }
        auto res = exp(logG, n);
        FormalPowerSeries ret(n);
        ull t = modpow(lowest_coef, k, M);
        int offset = lowest_deg == 0 ? 0 : k > (ull)n ? n : min(lowest_deg*k, (ull)n);
        for (int i = 0;; i++){
            if (i+offset < n){
                ret[i+offset] = res[i]*t%M;
            }
            else{
                break;
            }
        }
        return ret;
    }

    // \sum_j[x^j]f^i を i=0,1,...,m
    template <typename mint>
    vc<mint> power_projection(vc<mint> f, vc<mint> wt, int m, const inv_table& it){
      assert(len(f) == len(wt));
      if (f.empty()) { return vc<mint>(m + 1, mint(0)); }
      if (f[0] != mint(0)) {
        mint c = f[0];
        f[0] = 0;
        vc<mint> A = power_projection(f, wt, m);
        FOR(p, m + 1) A[p] *= fact_inv<mint>(p);
        vc<mint> B(m + 1);
        mint pow = 1;
        FOR(q, m + 1) B[q] = pow * fact_inv<mint>(q), pow *= c;
        A = convolution<mint>(A, B);
        A.resize(m + 1);
        FOR(i, m + 1) A[i] *= fact<mint>(i);
        return A;
      }

      int n = 1;
      while (n < len(f)) n *= 2;
      f.resize(n), wt.resize(n);
      reverse(all(wt));

      vc<mint> W(2 * n);
      {
        // bit reverse order
        vc<int> btr(2 * n);
        int log = topbit(2 * n);
        FOR(i, 2 * n) { btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1)); }
        int t = mint::ntt_info().fi;
        mint r = mint::ntt_info().se;
        mint dw = r.inverse().pow((1 << t) / (4 * n));
        mint w = 1;
        for (auto& i: btr) { W[i] = w, w *= dw; }
      }

      int k = 1;
      vc<mint> P(2 * n), Q(2 * n);
      FOR(i, n) P[i] = wt[i], Q[i] = -f[i];

      while (n > 1) {
        P.resize(4 * n * k), Q.resize(4 * n * k);
        Q[2 * n * k] = 1;
        vc<mint> R(4 * n * k);
        FOR(i, 4 * n * k) R[i] = (i % 2 == 0 ? Q[i] : -Q[i]);
        ntt(P, 0), ntt(Q, 0);
        FOR(i, 2 * n * k) {
          P[i] = inv<mint>(2) * W[i]
                 * (P[2 * i] * Q[2 * i + 1] - P[2 * i + 1] * Q[2 * i]);
          Q[i] = Q[2 * i] * Q[2 * i + 1];
        }
        P.resize(2 * n * k), Q.resize(2 * n * k);
        ntt(P, 1), ntt(Q, 1);
        FOR(j, 2 * k) FOR(i, n / 2, n) P[n * j + i] = 0, Q[n * j + i] = 0;
        Q[0] = 0;
        n /= 2, k *= 2;
      }
      vc<mint> p(k);
      FOR(i, k) p[i] = P[2 * i];
      reverse(all(p));
      p.resize(m + 1);
      return p;
    }

};

/// @brief [x^N](P(x)/Q(x))をmod Mで求める。
template<ull M>
ll Bostan_Mori(const ll N, FormalPowerSeries P, FormalPowerSeries Q, const fps_operator<M>& op){
    assert(N >= 0);
    if (N == 0){
        return P[0]*inverse_mod(Q[0], M);
    }
    vector<ll> Q_minus;
    const int maxloop = (N == 1 ? 1 : 65-__builtin_clzll(N-1));
    for (int _i_ = 0; _i_ < maxloop; _i_++){
        Q_minus.resize(Q.size());
        for (size_t i = 0; i < Q_minus.size(); i += 2){
            Q_minus[i] = Q[i];
        }
        for (size_t i = 1; i < Q_minus.size(); i += 2){
            Q_minus[i] = (M-1)*Q[i]%M;
        }
        auto A = op.convolution(P,Q_minus);
        auto B = op.convolution(Q,Q_minus);
        Q.resize((B.size()+1)/2);
        for (size_t i = 0; i < B.size(); i += 2){
            Q[i/2] = B[i];
        }

        P.resize((A.size()+!btest_for_fps(N,_i_))/2);
        for (size_t i = btest_for_fps(N,_i_); i < A.size(); i += 2){
            P[i/2] = A[i];
        }
    }
    return P[0]*inverse_mod(Q[0], M);
}


#endif /* FPS_OPERATION_HPP_ */