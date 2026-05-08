#ifndef FPS_OPERATION_HPP_
#define FPS_OPERATION_HPP_

#include <vector>
#include <algorithm>
#include <math_functions.hpp>
#include <fps.hpp>
#include <string.h>
using namespace std;
using ll = long long;
using uint = unsigned;

template<typename T> inline bool btest_for_fps(T K, int i){return K&(1ull<<i);}


/// @brief mod M上での形式的冪級数の計算を行う構造体
template<ull M>
struct fps_operator{
    ull sum_e[30];
    ull sum_ie[30];
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
        for (auto v : powroot){
            powrootinv.push_back(inverse_mod(v,M));
        }
        int cnt2 = powroot.size()-1;
        ull now = 1;
        for (int i = 0; i <= cnt2-2; i++) {
            sum_e[i] = (powroot[i+2]*now)%M;
            now = (now*powrootinv[i+2])%M;
        }
        ull inow = 1;
        for (int i = 0; i <= cnt2-2; i++) {
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
        return F1;
    }
    FormalPowerSeries inv() const {
        return FormalPowerSeries{2};
    }
};

/// @brief [x^N](P(x)/Q(x))をmod Mで求める。
template<ull M>
ll Bostan_Mori(const ll N, FormalPowerSeries P, FormalPowerSeries Q, const fps_operator<M>& op){
    assert(N >= 0);
    if (N == 0){
        return P[0]*inverse_mod(Q[0], 998244353);
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