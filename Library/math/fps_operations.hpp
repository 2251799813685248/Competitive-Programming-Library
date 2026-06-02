#ifndef FPS_OPERATION_HPP_
#define FPS_OPERATION_HPP_

#include <vector>
#include <algorithm>
#include <fps.hpp>
#include <math_functions.hpp>
#include <mod_table.hpp>
using namespace std;
using ll = long long;
using uint = unsigned;
using ull = unsigned long long;

template<typename T> inline bool btest_for_fps(T K, int i){return K&(1ull<<i);}


/// @brief mod M上での形式的冪級数の計算を行う構造体
template<ull M>
struct fps_operator{
    ull sum_e[30];
    ull sum_ie[30];
    uint log_max_length;
    uint last_powroot;
    constexpr fps_operator(){
        vector<ll> powroot{1};
        vector<ll> powrootinv;
        while (powroot.back() >= 0){
            powroot.push_back(cipolla<M>(powroot.back()));
            if (powroot.back() == powroot[powroot.size()-2]){
                powroot.back() = M-powroot.back();
            }
        }
        powroot.pop_back();
        log_max_length = powroot.size()-1;
        last_powroot = powroot.back();
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
    void inplaceDFT_T(FormalPowerSeries& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        for (int ph = h; ph >= 1; ph--) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            ull now = 1;
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; i++) {
                    uint l = F[i + offset];
                    uint r = F[i + offset + p];
                    F[i + offset] = l+r<M ? l+r : l+r-M;
                    F[i + offset + p] = (l<r ? l+M-r : l-r)*now%M;
                }
                now = now*sum_e[__builtin_ctz(~s)]%M;
            }
        }
    }
    void inplaceIDFT_T(FormalPowerSeries& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        for (int ph = 1; ph <= h; ph++) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            ull inow = 1;
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; i++) {
                    uint l = F[i + offset];
                    uint r = F[i + offset + p]*inow%M;
                    F[i + offset] = l+r<M ? l+r : l+r-M;
                    F[i + offset + p] = l<r ? l+M-r : l-r;
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
    FormalPowerSeries exp(const FormalPowerSeries& F, const int n, const mod_table<M>& mtable) const {
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
                epsilon[i] = epsilon[i-1]*(ull)mtable.invmod[i]%M+M-(i < sz_f ? F[i] : 0);
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
    FormalPowerSeries log(const FormalPowerSeries& F, const int n, const mod_table<M>& mtable) const {
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
            res[i] = res[i-1]*(ull)mtable.invmod[i]%M;
        }
        res[0] = 0;
        return res;
    }
    /// @brief F^k mod x^n を求める。
    FormalPowerSeries pow(const FormalPowerSeries& F, const ull k, const int n, const mod_table<M>& mtable) const {
        if (k == 0){
            FormalPowerSeries ret(n,0);
            ret[0] = 1;
            return ret;
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
        auto logG = log(G, n, mtable);
        ull kmodm=k%M;
        for (int i = 0; i < n; i++){
            logG[i] = logG[i]*kmodm%M;
        }
        auto res = exp(logG, n, mtable);
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
    // \sum_{j} wt[j]*[x^j]f^i を i=0,1,...,m
    FormalPowerSeries power_projection(FormalPowerSeries f, FormalPowerSeries wt, uint m, const mod_table<M>& mtable) const {
        assert(f.size() == wt.size());
        if (f.size() == 0){return vector<uint>(m+1, 0);}
        if (f[0] != 0){
            uint c = f[0];
            f[0] = 0;
            auto A = power_projection(f, wt, m);
            for (uint p = 0; p < m+1; p++) A[p] = A[p]*(ull)mtable.factorialmodinv[p]%M;
            FormalPowerSeries B(m+1);
            ull pow = 1;
            for (uint q = 0; q < m+1; q++) B[q] = pow * (ull)mtable.factorialmodinv[q]%M, pow = pow*c%M;
            A = convolution(A, B);
            A.resize(m + 1);
            for (uint i = 0; i < m+1; i++) A[i] = A[i]*(ull)mtable.factorialmod[i]%M;
            return A;
        }
  
        int n = 1;
        while (n < (int)f.size()) n *= 2;
        f.resize(n), wt.resize(n);
        reverse(wt.f.begin(), wt.f.end());
  
        FormalPowerSeries W(2*n);
        {
            // bit reverse order
            vector<int> btr(2*n);
            int log = 31-__builtin_clz(2*n);
            for (int i = 0; i < 2*n; i++){btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1));}
            int t = log_max_length;
            uint r = last_powroot;
            ull dw = modpow(inverse_mod<ll,ll>(r, M), (1<<t)/(4*n), M);
            uint w = 1;
            for (auto i: btr) { W[i] = w, w = w*dw%M; }
        }
  
        int k = 1;
        FormalPowerSeries P(2*n), Q(2*n);
        for (int i = 0; i < n; i++) P[i] = wt[i], Q[i] = f[i] == 0 ? 0 : M-f[i];
  
        while (n > 1) {
            P.resize(4*n*k), Q.resize(4*n*k);
            Q[2*n*k] = 1;
            FormalPowerSeries R(4*n*k);
            for(int i = 0; i < 4*n*k; i++) R[i] = (i % 2 == 0 ? Q[i] : Q[i] == 0 ? 0 : M-Q[i]);
            inplaceDFT(P), inplaceDFT(Q);
            for(int i = 0; i < 2*n*k; i++){
                P[i] = (W[i]&1 ? (W[i]+M)/2 : W[i]/2)*(P[2*i]*(ull)Q[2*i+1]%M+M-P[2*i+1]*(ull)Q[2*i]%M)%M;
                Q[i] = Q[2*i]*(ull)Q[2*i+1]%M;
            }
            P.resize(2*n*k), Q.resize(2*n*k);
            inplaceIDFT(P), inplaceIDFT(Q);
            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) P[n*j+i] = 0, Q[n*j+i] = 0;
            Q[0] = 0;
            n /= 2;
            k *= 2;
        }
        FormalPowerSeries p(k);
        for (int i = 0; i < k; i++) p[i] = P[2 * i];
        reverse(p.f.begin(), p.f.end());
        p.resize(m+1);
        return p;
    }
    // f(g(x)) mod x^len(f) を求める。
    FormalPowerSeries composition(FormalPowerSeries f, FormalPowerSeries g, const mod_table<M>& mtable) const {
        f.resize(max(f.size(),g.size()));
        int N = f.size();
        if (N == 0) return FormalPowerSeries(0);

        int n = 1;
        while (n < (int)f.size()) n *= 2;
        f.resize(n), g.resize(n);

        FormalPowerSeries W(2*n);
        {
            // bit reverse order
            vector<int> btr(2*n);
            int log = 31-__builtin_clz(2*n);
            for (int i = 0; i < 2*n; i++){btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1));}
            int t = log_max_length;
            uint r = last_powroot;
            ull dw = modpow(inverse_mod<ll,ll>(r, M), (1<<t)/(4*n), M);
            uint w = 1;
            for (auto i: btr) { W[i] = w, w = w*dw%M; }
        }

        auto rec = [&](auto &rec, int n, int k, FormalPowerSeries &Q) -> FormalPowerSeries {
            if (n == 1) {
                reverse(f.f.begin(), f.f.end());
                FormalPowerSeries p(2*k);
                for (int i = 0; i < k; i++) p[2 * i] = f[i];
                return p;
            }
            Q.resize(4*n*k);
            Q[2*n*k] = 1;
            inplaceDFT(Q);
            FormalPowerSeries nxt_Q(2*n*k);
            for (int i = 0; i < 2*n*k; i++) nxt_Q[i] = Q[2*i]*(ull)Q[2*i+1]%M;
            
            inplaceIDFT(nxt_Q);
            ull iz = inverse_mod((ll)nxt_Q.size(), M);
            for (uint i = 0; i < nxt_Q.size(); i++) nxt_Q[i] = nxt_Q[i]*iz%M;

            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) nxt_Q[n*j+i] = 0;
            nxt_Q[0] = 0;
            FormalPowerSeries p = rec(rec, n/2, 2*k, nxt_Q);
            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) p[n * j + i] = 0;
            
            iz = mtable.invmod[p.size()];
            for (uint i = 0; i < p.size(); i++) p[i] = p[i]*iz%M;
            inplaceIDFT_T(p);
            
            p.resize(4*n*k);
            for (int i = 2*n*k-1; i >= 0; i--){
                p[2*i+1] = (ull)(mtable.invmod[2]-1+M)%M*W[i]%M*Q[2*i]%M*p[i]%M;
                p[2*i] = (ull)mtable.invmod[2]*W[i]%M*Q[2*i+1]%M*p[i]%M;
            }
            
            inplaceDFT_T(p);
            
            p.resize(2*n*k);
            return p;
        };

        FormalPowerSeries Q(2*n);
        for(int i = 0; i < n; i++) Q[i] = g[i] == 0 ? 0 : M-g[i];
        FormalPowerSeries p = rec(rec, n, 1, Q);
        p.resize(n);
        reverse(p.f.begin(), p.f.end());
        p.resize(N);
        return p;
    }

    FormalPowerSeries polynominal_taylor_shift(const FormalPowerSeries& F, ll a, const mod_table<M>& mtable){
        ull b = M+a%M;
        if (b >= M){b -= M;}
        if (b == 0){return F;}
        int N = F.size();
        FormalPowerSeries G(N), H(N);
        for (int i = 0; i < N; i++){
            G[i] = F[N-1-i]*(ull)mtable.factorialmod[N-1-i]%M;
            H[i] = mtable.factorialmodinv[i];
        }
        ull t = b;
        for (int i = N-2; i >= 0; i--){
            G[i] = G[i]*t%M;
            t = t*b%M;
        }
        G = convolution(G,H);
        t = 1;
        b = inverse_mod(b,M);
        for (int j = 0; j < N; j++){
            H[j] = mtable.factorialmodinv[j]*t%M*G[N-1-j]%M;
            t = t*b%M;
        }
        return H;
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