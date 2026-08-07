#ifndef FPS_OPERATION_HPP_
#define FPS_OPERATION_HPP_

#include <vector>
#include <algorithm>
#include <immintrin.h>
#include <fps.hpp>
#include <math_functions.hpp>
#include <mod_table.hpp>
#include <prime_and_divisors.hpp>
using namespace std;
using ll = long long;
using uint = unsigned;
using ull = unsigned long long;

template<typename T> inline bool btest_for_fps(T K, int i){return K&(1ull<<i);}


/// @brief mod M上での形式的冪級数の計算を行う構造体
template<uint M>
struct fps_operator{
    uint sum_e[30];
    uint sum_ie[30];
    uint log_max_length;
    uint last_powroot;

    // --- モンゴメリ乗算用の定数と関数 ---
    static constexpr uint get_r() {
        uint res = M;
        for (int i = 0; i < 4; ++i) res *= 2 - M * res;
        return res;
    }
    static constexpr uint R = get_r();
    static constexpr uint R2 = -ull(M) % M;
    static constexpr uint NEG_INV = 0 - R;

    static inline constexpr uint reduce(ull x) {
        uint res = (x + ull(uint(x) * NEG_INV) * M) >> 32;
        return res >= M ? res - M : res;
    }
    static inline constexpr uint to_montgomery(uint x) {
        return reduce(ull(x) * R2);
    }
    static inline constexpr uint from_montgomery(uint x) {
        return reduce(x);
    }
    static inline constexpr uint montgomery_mul(uint x, uint y) {
        return reduce(ull(x) * y);
    }
    static inline constexpr uint mod_add(uint x, uint y) {
        return x + y >= M ? x + y - M : x + y;
    }
    static inline constexpr uint mod_sub(uint x, uint y) {
        return x < y ? x + M - y : x - y;
    }

    // --- AVX-512 / AVX2 SIMD Intrinsics ---
#if defined(__AVX512F__) && defined(__AVX512DQ__)
    static inline __m512i montgomery_mul_simd(__m512i a, __m512i b, __m512i mod, __m512i neg_inv) {
        __m512i mul0 = _mm512_mul_epu32(a, b);
        __m512i mul1 = _mm512_mul_epu32(_mm512_srli_epi64(a, 32), _mm512_srli_epi64(b, 32));
        __m512i q0 = _mm512_mul_epu32(_mm512_mullo_epi32(mul0, neg_inv), mod);
        __m512i q1 = _mm512_mul_epu32(_mm512_mullo_epi32(mul1, neg_inv), mod);
        __m512i res0 = _mm512_srli_epi64(_mm512_add_epi64(mul0, q0), 32);
        __m512i res1 = _mm512_srli_epi64(_mm512_add_epi64(mul1, q1), 32);
        __m512i res = _mm512_or_si512(res0, _mm512_slli_epi64(res1, 32));
        __mmask16 cmp = _mm512_cmpge_epu32_mask(res, mod);
        return _mm512_mask_sub_epi32(res, cmp, res, mod);
    }
    static inline __m512i mod_add_simd(__m512i a, __m512i b, __m512i mod) {
        __m512i res = _mm512_add_epi32(a, b);
        __mmask16 cmp = _mm512_cmpge_epu32_mask(res, mod);
        return _mm512_mask_sub_epi32(res, cmp, res, mod);
    }
    static inline __m512i mod_sub_simd(__m512i a, __m512i b, __m512i mod) {
        __mmask16 cmp = _mm512_cmplt_epu32_mask(a, b);
        __m512i res = _mm512_sub_epi32(a, b);
        return _mm512_mask_add_epi32(res, cmp, res, mod);
    }
#elif defined(__AVX2__)
    // AVX2用 モンゴメリ乗算・剰余加減算
    static inline __m256i montgomery_mul_simd(__m256i a, __m256i b, __m256i mod, __m256i neg_inv) {
        __m256i mul0 = _mm256_mul_epu32(a, b);
        __m256i mul1 = _mm256_mul_epu32(_mm256_srli_epi64(a, 32), _mm256_srli_epi64(b, 32));
        __m256i q0 = _mm256_mul_epu32(_mm256_mullo_epi32(mul0, neg_inv), mod);
        __m256i q1 = _mm256_mul_epu32(_mm256_mullo_epi32(mul1, neg_inv), mod);
        __m256i res0 = _mm256_srli_epi64(_mm256_add_epi64(mul0, q0), 32);
        __m256i res1 = _mm256_srli_epi64(_mm256_add_epi64(mul1, q1), 32);
        
        __m256i res = _mm256_blend_epi32(res0, _mm256_slli_epi64(res1, 32), 0xAA);
        __m256i diff = _mm256_sub_epi32(res, mod);
        // diff < 0 なら -1 (全ビット1)、そうでないなら 0 となるマスク
        __m256i mask = _mm256_srai_epi32(diff, 31);
        return _mm256_add_epi32(diff, _mm256_and_si256(mask, mod));
    }
    static inline __m256i mod_add_simd(__m256i a, __m256i b, __m256i mod) {
        __m256i diff = _mm256_sub_epi32(_mm256_add_epi32(a, b), mod);
        __m256i mask = _mm256_srai_epi32(diff, 31);
        return _mm256_add_epi32(diff, _mm256_and_si256(mask, mod));
    }
    static inline __m256i mod_sub_simd(__m256i a, __m256i b, __m256i mod) {
        __m256i diff = _mm256_sub_epi32(a, b);
        __m256i mask = _mm256_srai_epi32(diff, 31);
        return _mm256_add_epi32(diff, _mm256_and_si256(mask, mod));
    }
#endif


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
            powrootinv.push_back(inverse_mod<ll,ll>(v,M));
        }
        int cnt2 = powroot.size()-1;
        uint now = reduce(R2);
        for (int i = 0; i <= cnt2-2; i++){
            sum_e[i] = montgomery_mul(to_montgomery(powroot[i+2]), now);
            now = montgomery_mul(now, to_montgomery(powrootinv[i+2]));
        }
        uint inow = reduce(R2);
        for (int i = 0; i <= cnt2-2; i++){
            sum_ie[i] = montgomery_mul(to_montgomery(powrootinv[i+2]), inow);
            inow = montgomery_mul(inow, to_montgomery(powroot[i+2]));
        }
        for (int i = cnt2-1; i < 30; i++){
            sum_e[i] = 0;
            sum_ie[i] = 0;
        }
    }

    void inplaceDFT(FormalPowerSeries<M>& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        
        for (int ph = 1; ph <= h; ph++) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            uint now = to_montgomery(1);
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
#if defined(__AVX512F__) && defined(__AVX512DQ__)
                if (p >= 16) {
                    __m512i vmod = _mm512_set1_epi32(M);
                    __m512i vneg_inv = _mm512_set1_epi32(NEG_INV);
                    __m512i vnow = _mm512_set1_epi32(now);
                    for (int i = 0; i < p; i += 16) {
                        __m512i l = _mm512_loadu_si512((__m512i*)&F[i + offset].val);
                        __m512i r = _mm512_loadu_si512((__m512i*)&F[i + offset + p].val);
                        __m512i r_now = montgomery_mul_simd(r, vnow, vmod, vneg_inv);
                        _mm512_storeu_si512((__m512i*)&F[i + offset].val, mod_add_simd(l, r_now, vmod));
                        _mm512_storeu_si512((__m512i*)&F[i + offset + p].val, mod_sub_simd(l, r_now, vmod));
                    }
                } else
#elif defined(__AVX2__)
                if (p >= 8) {
                    __m256i vmod = _mm256_set1_epi32(M);
                    __m256i vneg_inv = _mm256_set1_epi32(NEG_INV);
                    __m256i vnow = _mm256_set1_epi32(now);
                    for (int i = 0; i < p; i += 8) {
                        __m256i l = _mm256_loadu_si256((__m256i*)&F[i + offset].val);
                        __m256i r = _mm256_loadu_si256((__m256i*)&F[i + offset + p].val);
                        __m256i r_now = montgomery_mul_simd(r, vnow, vmod, vneg_inv);
                        _mm256_storeu_si256((__m256i*)&F[i + offset].val, mod_add_simd(l, r_now, vmod));
                        _mm256_storeu_si256((__m256i*)&F[i + offset + p].val, mod_sub_simd(l, r_now, vmod));
                    }
                } else
#endif
                {
                    for (int i = 0; i < p; i++) {
                        uint l = F[i + offset].val;
                        uint r = montgomery_mul(F[i + offset + p].val, now);
                        F[i + offset].val = mod_add(l, r);
                        F[i + offset + p].val = mod_sub(l, r);
                    }
                }
                now = montgomery_mul(now, sum_e[__builtin_ctz(~s)]);
            }
        }
    }
    void inplaceIDFT(FormalPowerSeries<M>& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        
        for (int ph = h; ph >= 1; ph--) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            uint inow = to_montgomery(1);
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
#if defined(__AVX512F__) && defined(__AVX512DQ__)
                if (p >= 16) {
                    __m512i vmod = _mm512_set1_epi32(M);
                    __m512i vneg_inv = _mm512_set1_epi32(NEG_INV);
                    __m512i vinow = _mm512_set1_epi32(inow);
                    for (int i = 0; i < p; i += 16) {
                        __m512i l = _mm512_loadu_si512((__m512i*)&F[i + offset].val);
                        __m512i r = _mm512_loadu_si512((__m512i*)&F[i + offset + p].val);
                        _mm512_storeu_si512((__m512i*)&F[i + offset].val, mod_add_simd(l, r, vmod));
                        _mm512_storeu_si512((__m512i*)&F[i + offset + p].val, montgomery_mul_simd(mod_sub_simd(l, r, vmod), vinow, vmod, vneg_inv));
                    }
                } else
#elif defined(__AVX2__)
                if (p >= 8) {
                    __m256i vmod = _mm256_set1_epi32(M);
                    __m256i vneg_inv = _mm256_set1_epi32(NEG_INV);
                    __m256i vinow = _mm256_set1_epi32(inow);
                    for (int i = 0; i < p; i += 8) {
                        __m256i l = _mm256_loadu_si256((__m256i*)&F[i + offset].val);
                        __m256i r = _mm256_loadu_si256((__m256i*)&F[i + offset + p].val);
                        _mm256_storeu_si256((__m256i*)&F[i + offset].val, mod_add_simd(l, r, vmod));
                        _mm256_storeu_si256((__m256i*)&F[i + offset + p].val, montgomery_mul_simd(mod_sub_simd(l, r, vmod), vinow, vmod, vneg_inv));
                    }
                } else
#endif
                {
                    for (int i = 0; i < p; i++) {
                        uint l = F[i + offset].val;
                        uint r = F[i + offset + p].val;
                        F[i + offset].val = mod_add(l, r);
                        F[i + offset + p].val = montgomery_mul(mod_sub(l, r), inow);
                    }
                }
                inow = montgomery_mul(inow, sum_ie[__builtin_ctz(~s)]);
            }
        }
    }
    void inplaceDFT_T(FormalPowerSeries<M>& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        
        for (int ph = h; ph >= 1; ph--) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            uint32_t now = to_montgomery(1);
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
#if defined(__AVX512F__) && defined(__AVX512DQ__)
                if (p >= 16) {
                    __m512i vmod = _mm512_set1_epi32(M);
                    __m512i vneg_inv = _mm512_set1_epi32(NEG_INV);
                    __m512i vnow = _mm512_set1_epi32(now);
                    for (int i = 0; i < p; i += 16) {
                        __m512i l = _mm512_loadu_si512((__m512i*)&F[i + offset].val);
                        __m512i r = _mm512_loadu_si512((__m512i*)&F[i + offset + p].val);
                        _mm512_storeu_si512((__m512i*)&F[i + offset].val, mod_add_simd(l, r, vmod));
                        _mm512_storeu_si512((__m512i*)&F[i + offset + p].val, montgomery_mul_simd(mod_sub_simd(l, r, vmod), vnow, vmod, vneg_inv));
                    }
                } else
#elif defined(__AVX2__)
                if (p >= 8) {
                    __m256i vmod = _mm256_set1_epi32(M);
                    __m256i vneg_inv = _mm256_set1_epi32(NEG_INV);
                    __m256i vnow = _mm256_set1_epi32(now);
                    for (int i = 0; i < p; i += 8) {
                        __m256i l = _mm256_loadu_si256((__m256i*)&F[i + offset].val);
                        __m256i r = _mm256_loadu_si256((__m256i*)&F[i + offset + p].val);
                        _mm256_storeu_si256((__m256i*)&F[i + offset].val, mod_add_simd(l, r, vmod));
                        _mm256_storeu_si256((__m256i*)&F[i + offset + p].val, montgomery_mul_simd(mod_sub_simd(l, r, vmod), vnow, vmod, vneg_inv));
                    }
                } else
#endif
                {
                    for (int i = 0; i < p; i++) {
                        uint32_t l = F[i + offset].val;
                        uint32_t r = F[i + offset + p].val;
                        F[i + offset].val = mod_add(l, r);
                        F[i + offset + p].val = montgomery_mul(mod_sub(l, r), now);
                    }
                }
                now = montgomery_mul(now, sum_e[__builtin_ctz(~s)]);
            }
        }
    }
    void inplaceIDFT_T(FormalPowerSeries<M>& F) const {
        F.resize(upperpow2(F.sz));
        int n = F.sz;
        if (n == 0) return;
        int h = __builtin_ctz(n);
        
        for (int ph = 1; ph <= h; ph++) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            uint32_t inow = to_montgomery(1);
            for (int s = 0; s < w; s++) {
                int offset = s << (h - ph + 1);
#if defined(__AVX512F__) && defined(__AVX512DQ__)
                if (p >= 16) {
                    __m512i vmod = _mm512_set1_epi32(M);
                    __m512i vneg_inv = _mm512_set1_epi32(NEG_INV);
                    __m512i vinow = _mm512_set1_epi32(inow);
                    for (int i = 0; i < p; i += 16) {
                        __m512i l = _mm512_loadu_si512((__m512i*)&F[i + offset].val);
                        __m512i r = _mm512_loadu_si512((__m512i*)&F[i + offset + p].val);
                        __m512i r_inow = montgomery_mul_simd(r, vinow, vmod, vneg_inv);
                        _mm512_storeu_si512((__m512i*)&F[i + offset].val, mod_add_simd(l, r_inow, vmod));
                        _mm512_storeu_si512((__m512i*)&F[i + offset + p].val, mod_sub_simd(l, r_inow, vmod));
                    }
                } else
#elif defined(__AVX2__)
                if (p >= 8) {
                    __m256i vmod = _mm256_set1_epi32(M);
                    __m256i vneg_inv = _mm256_set1_epi32(NEG_INV);
                    __m256i vinow = _mm256_set1_epi32(inow);
                    for (int i = 0; i < p; i += 8) {
                        __m256i l = _mm256_loadu_si256((__m256i*)&F[i + offset].val);
                        __m256i r = _mm256_loadu_si256((__m256i*)&F[i + offset + p].val);
                        __m256i r_inow = montgomery_mul_simd(r, vinow, vmod, vneg_inv);
                        _mm256_storeu_si256((__m256i*)&F[i + offset].val, mod_add_simd(l, r_inow, vmod));
                        _mm256_storeu_si256((__m256i*)&F[i + offset + p].val, mod_sub_simd(l, r_inow, vmod));
                    }
                } else
#endif
                {
                    for (int i = 0; i < p; i++) {
                        uint32_t l = F[i + offset].val;
                        uint32_t r = montgomery_mul(F[i + offset + p].val, inow);
                        F[i + offset].val = mod_add(l, r);
                        F[i + offset + p].val = mod_sub(l, r);
                    }
                }
                inow = montgomery_mul(inow, sum_ie[__builtin_ctz(~s)]);
            }
        }
    }
    FormalPowerSeries<M> convolution(FormalPowerSeries<M> F1, FormalPowerSeries<M> F2) const {
        int n = F1.size();
        int m = F2.size();
        if (n == 0 || m == 0) return FormalPowerSeries<M>(0);
        
        if (std::min(n, m) <= 60) {
            if (n < m) {
                std::swap(n, m);
                std::swap(F1.sz, F2.sz);
                std::swap(F1.f, F2.f);
            }
            FormalPowerSeries<M> ans(n + m - 1);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    ans[i + j].val = (ans[i + j].val + ull(F1[i].val) * F2[j].val) % M;
                }
            }
            return ans;
        }

        int reference_size = upperpow2(n + m - 1);
        F1.resize(reference_size);
        F2.resize(reference_size);

        for (int i = 0; i < reference_size; i++) {
            F1[i].val = to_montgomery(F1[i].val);
            F2[i].val = to_montgomery(F2[i].val);
        }

        inplaceDFT(F1);
        inplaceDFT(F2);

        int i = 0;
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        __m512i vmod512 = _mm512_set1_epi32(M);
        __m512i vneg_inv512 = _mm512_set1_epi32(NEG_INV);
        for (; i + 15 < reference_size; i += 16) {
            __m512i a = _mm512_loadu_si512((__m512i*)&F1[i].val);
            __m512i b = _mm512_loadu_si512((__m512i*)&F2[i].val);
            _mm512_storeu_si512((__m512i*)&F1[i].val, montgomery_mul_simd(a, b, vmod512, vneg_inv512));
        }
#elif defined(__AVX2__)
        __m256i vmod256 = _mm256_set1_epi32(M);
        __m256i vneg_inv256 = _mm256_set1_epi32(NEG_INV);
        for (; i + 7 < reference_size; i += 8) {
            __m256i a = _mm256_loadu_si256((__m256i*)&F1[i].val);
            __m256i b = _mm256_loadu_si256((__m256i*)&F2[i].val);
            _mm256_storeu_si256((__m256i*)&F1[i].val, montgomery_mul_simd(a, b, vmod256, vneg_inv256));
        }
#endif
        for (; i < reference_size; i++) {
            F1[i].val = montgomery_mul(F1[i].val, F2[i].val);
        }

        inplaceIDFT(F1);

        uint iz_normal = modpow<M>(reference_size, M-2);
        i = 0;
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        __m512i viz_normal512 = _mm512_set1_epi32(iz_normal);
        for (; i + 15 < reference_size; i += 16) {
            __m512i a = _mm512_loadu_si512((__m512i*)&F1[i].val);
            _mm512_storeu_si512((__m512i*)&F1[i].val, montgomery_mul_simd(a, viz_normal512, vmod512, vneg_inv512));
        }
#elif defined(__AVX2__)
        __m256i viz_normal256 = _mm256_set1_epi32(iz_normal);
        for (; i + 7 < reference_size; i += 8) {
            __m256i a = _mm256_loadu_si256((__m256i*)&F1[i].val);
            _mm256_storeu_si256((__m256i*)&F1[i].val, montgomery_mul_simd(a, viz_normal256, vmod256, vneg_inv256));
        }
#endif
        for (; i < reference_size; i++) {
            F1[i].val = reduce(ull(F1[i].val) * iz_normal);
        }

        F1.resize(n + m - 1);
        return F1;
    }
    FormalPowerSeries<M> add(const FormalPowerSeries<M>& F1, const FormalPowerSeries<M>& F2) const {
        FormalPowerSeries<M> ret(max(F1.sz, F2.sz));
        if (F1.sz < F2.sz){
            for (uint i = 0; i < F1.sz; i++){
                ret[i].val = F1[i].val+F2[i].val;
                if (ret[i].val >= M){
                    ret[i].val -= M;
                }
            }
            for (uint i = F1.sz; i < F2.sz; i++){
                ret[i].val = F2[i].val;
            }
        }
        else{
            for (uint i = 0; i < F2.sz; i++){
                ret[i].val = F1[i].val+F2[i].val;
                if (ret[i].val >= M){
                    ret[i].val -= M;
                }
            }
            for (uint i = F2.sz; i < F1.sz; i++){
                ret[i].val = F1[i].val;
            }
        }
        return ret;
    }
    FormalPowerSeries<M> subtract(const FormalPowerSeries<M>& F1, const FormalPowerSeries<M>& F2) const {
        FormalPowerSeries ret(max(F1.sz, F2.sz));
        if (F1.sz < F2.sz){
            for (uint i = 0; i < F1.sz; i++){
                ret[i].val = F1[i].val+M-F2[i].val;
                if (ret[i].val >= M){
                    ret[i].val -= M;
                }
            }
            for (uint i = F1.sz; i < F2.sz; i++){
                ret[i].val = F2[i].val == 0 ? 0 : M-F2[i].val;
            }
        }
        else{
            for (uint i = 0; i < F2.sz; i++){
                ret[i].val = F1[i].val+M-F2[i].val;
                if (ret[i].val >= M){
                    ret[i].val -= M;
                }
            }
            for (uint i = F2.sz; i < F1.sz; i++){
                ret[i].val = F1[i].val;
            }
        }
        return ret;
    }
    /// @brief F*G == 1 mod x^n となるGを求める F[x^0] != 0 が必要 
    FormalPowerSeries<M> inv(const FormalPowerSeries<M>& F, const int n) const {
        int sz_f = F.sz;
        FormalPowerSeries<M> res{inverse_mod<ll,ll>(F[0].val, M)};
        for(int d = 1; d < n; d<<=1){
            FormalPowerSeries<M> f(2*d), g(2*d);
            for(int j = 0; j < 2*d; j++) f[j].val = (j < sz_f ? F[j].val : 0);
            for(int j = 0; j < d; j++) g[j].val = res[j].val;
            inplaceDFT(f);
            inplaceDFT(g);
            for(int j = 0; j < 2*d; j++) f[j].val = f[j].val*(ull)g[j].val%M;
            inplaceIDFT(f);
            ull iz = inverse_mod<ll,ll>(2*d, M);
            for (int i = 0; i < 2*d; i++){
                f[i].val = f[i].val*iz%M;
            }
            for(int j = 0; j < d; j++){
                f[j].val = 0;
                if (f[j+d].val > 0) f[j+d].val = M-f[j+d].val;
            }
            inplaceDFT(f);
            for(int j = 0; j < 2*d; j++) f[j].val = f[j].val*(ull)g[j].val%M;
            inplaceIDFT(f);
            for (int i = 0; i < 2*d; i++){
                f[i].val = f[i].val*iz%M;
            }
            for(int j = 0; j < d; j++) f[j].val = res[j].val;
            res = f;
        }
        res.resize(n);
        return res;
    }
    /// @brief exp(F) mod x^n を求める F[x^0] == 0 が必要
    FormalPowerSeries<M> exp(const FormalPowerSeries<M>& F, const int n, const mod_table<M>& mtable) const {
        assert(F[0].val == 0);
        ull sz_f = F.sz;
        FormalPowerSeries<M> F0{1},f0{1},g0{1};
        for(int d = 1; d < n; d<<=1){
            auto G0 = g0;
            inplaceDFT(G0);
            FormalPowerSeries<M> Delta(d);
            for(int j = 0; j < d; j++) Delta[j].val = F0[j].val*(ull)G0[j].val%M;
            inplaceIDFT(Delta);
            ull iz = inverse_mod<ll,ll>(d, M);
            for (int i = 0; i < d; i++) Delta[i].val = Delta[i].val*iz%M;
            if (Delta[0].val == 0) Delta[0].val = M;
            Delta[0].val -= 1;
            FormalPowerSeries<M> delta(2*d);
            for(int j = 0; j < d; j++) delta[d+j].val = Delta[j].val;
            FormalPowerSeries<M> epsilon(2*d);

            FormalPowerSeries<M> DF0(d-1);
            for (ull j = 0, ulld = d; j < ulld-1; j++) DF0[j].val = f0[j+1].val*(j+1)%M;
            DF0.f.push_back(0);
            DF0.sz++;
            inplaceDFT(DF0);
            for(int j = 0; j < d; j++) DF0[j].val = DF0[j].val*(ull)G0[j].val%M;
            inplaceIDFT(DF0);
            for (int i = 0; i < d; i++) DF0[i].val = DF0[i].val*iz%M;
            for(ull j = 0, ulld = d; j < ulld-1; j++){
                epsilon[j].val += (j+1 < sz_f ? F[j+1].val : 0)*(j+1)%M;
                if (epsilon[j].val >= M) epsilon[j].val -= M;
                epsilon[j+d].val += DF0[j].val;
                if (epsilon[j+d].val >= M) epsilon[j+d].val -= M;
                epsilon[j+d].val += M-(j+1 < sz_f ? F[j+1].val : 0)*(j+1)%M;
                if (epsilon[j+d].val >= M) epsilon[j+d].val -= M;
            }
            epsilon[d-1].val += DF0[d-1].val;
            if (epsilon[d-1].val >= M) epsilon[d-1].val -= M;
            Delta = delta;
            inplaceDFT(Delta);
            FormalPowerSeries<M> DH0(d-1);
            for (ull j = 0, limit = d-1; j < limit; j++) DH0[j].val = (j+1 < sz_f ? F[j+1].val : 0)*(j+1)%M;
            DH0.resize(2*d);
            inplaceDFT(DH0);
            for(int j = 0; j < 2*d; j++) Delta[j].val = Delta[j].val*(ull)DH0[j].val%M;
            inplaceIDFT(Delta);
            iz = inverse_mod<ll,ll>(2*d, M);
            for (int i = 0; i < 2*d; i++) Delta[i].val = Delta[i].val*iz%M;
            for(int j = 0; j < d; j++){
                epsilon[j+d].val += M-Delta[j+d].val;
                if (epsilon[j+d].val >= M){
                    epsilon[j+d].val -= M;
                }
            }
        
            for (ull i = 2*d-1; i >= 1; i--){
                epsilon[i].val = epsilon[i-1].val*(ull)mtable.invmodlist[i]%M+M-(i < sz_f ? F[i].val : 0);
                if (epsilon[i].val >= M) epsilon[i].val -= M;
            }
            epsilon[0].val = (F[0].val != 0 ? M-F[0].val : 0);
            
            auto Epsilon = epsilon;
            inplaceDFT(Epsilon);
            for (int j = 0; j < d; j++){DH0[j].val = f0[j].val; DH0[j+d].val = 0;}
            inplaceDFT(DH0);
            for (int j = 0; j < 2*d; j++) Epsilon[j].val = Epsilon[j].val*(ull)DH0[j].val%M;
            inplaceIDFT(Epsilon);
            for (int i = 0; i < 2*d; i++) Epsilon[i].val = Epsilon[i].val*iz%M;
            f0.resize(2*d);
            for (int j = 0; j < d; j++){
                f0[j+d].val += M-Epsilon[j+d].val;
                if (f0[j+d].val >= M) f0[j+d].val -= M;
            }
            if(2*d >= n) break;
            
            G0.resize(2*d);
            for (int j = 0; j < d; j++) G0[j].val = g0[j].val;
            inplaceDFT(G0);
            F0 = f0;
            inplaceDFT(F0);
            FormalPowerSeries<M> T(2*d);
            for (int j = 0; j < 2*d; j++) T[j].val = F0[j].val*(ull)G0[j].val%M;
            inplaceIDFT(T);
            for (int i = 0; i < 2*d; i++) T[i].val = T[i].val*iz%M;
            for (int j = 0; j < d; j++){
                T[j].val = 0;
                if (T[j+d].val != 0) T[j+d].val = M-T[j+d].val;
            }
            inplaceDFT(T);
            for (int j = 0; j < 2*d; j++) T[j].val = T[j].val*(ull)G0[j].val%M;
            inplaceIDFT(T);
            for (int i = 0; i < 2*d; i++) T[i].val = T[i].val*iz%M;
            for (int j = 0; j < d; j++) T[j].val = g0[j].val;
            g0 = T;
        }
        f0.resize(n);
        return f0;
    }
    /// @brief log(F) mod x^n を求める F[x^0] == 1 が必要 
    FormalPowerSeries<M> log(const FormalPowerSeries<M>& F, const int n, const mod_table<M>& mtable) const {
        assert(F[0].val == 1);
        auto DF = F;
        for (ull i = 0, limit = DF.sz-1; i < limit; i++){
            DF[i].val = DF[i+1].val*(i+1)%M;
        }
        DF.f.pop_back();
        DF.sz--;
        auto Finv = inv(F, n);
        auto res = convolution(DF, Finv);
        res.resize(n);
        for (int i = n-1; i >= 1; i--){
            res[i].val = res[i-1].val*(ull)mtable.invmodlist[i]%M;
        }
        res[0].val = 0;
        return res;
    }
    /// @brief F^k mod x^n を求める
    FormalPowerSeries<M> pow(const FormalPowerSeries<M>& F, const ull k, const int n, const mod_table<M>& mtable) const {
        if (k == 0){
            FormalPowerSeries<M> ret(n,0);
            ret[0].val = 1;
            return ret;
        }
        uint lowest_deg = 0;
        ull lowest_coef = 0;
        while (lowest_deg < F.sz && F[lowest_deg].val == 0){
            lowest_deg++;
        }
        if (lowest_deg == F.sz) return FormalPowerSeries<M>(n);
        lowest_coef = F[lowest_deg].val;
        ull iz = inverse_mod<ll,ll>(lowest_coef, M);
        FormalPowerSeries<M> G(n);
        for (uint i = lowest_deg, limit = min(F.sz, lowest_deg + n); i < limit; i++){
            G[i-lowest_deg].val = F[i].val*iz%M;
        }
        auto logG = log(G, n, mtable);
        ull kmodm=k%M;
        for (int i = 0; i < n; i++){
            logG[i].val = logG[i].val*kmodm%M;
        }
        auto res = exp(logG, n, mtable);
        FormalPowerSeries<M> ret(n);
        ull t = modpow<M>(lowest_coef, k);
        int offset = lowest_deg == 0 ? 0 : k > (ull)n ? n : min(lowest_deg*k, (ull)n);
        for (int i = 0;; i++){
            if (i+offset < n){
                ret[i+offset].val = res[i].val*t%M;
            }
            else{
                break;
            }
        }
        return ret;
    }
    // f(g(x)) mod x^len(f) を求める
    FormalPowerSeries<M> composition(FormalPowerSeries<M> f, FormalPowerSeries<M> g, const mod_table<M>& mtable) const {
        f.resize(max(f.size(),g.size()));
        int N = f.size();
        if (N == 0) return FormalPowerSeries<M>(0);

        int n = 1;
        while (n < (int)f.size()) n *= 2;
        f.resize(n), g.resize(n);

        FormalPowerSeries<M> W(2*n);
        {
            // bit reverse order
            vector<int> btr(2*n);
            int log = 31-__builtin_clz(2*n);
            for (int i = 0; i < 2*n; i++){btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1));}
            int t = log_max_length;
            uint r = last_powroot;
            ull dw = modpow<M>(inverse_mod<ll,ll>(r, M), (1<<t)/(4*n));
            uint w = 1;
            for (auto i: btr) { W[i].val = w, w = w*dw%M; }
        }

        auto rec = [&](auto &rec, int n, int k, FormalPowerSeries<M> &Q) -> FormalPowerSeries<M> {
            if (n == 1) {
                reverse(f.f.begin(), f.f.end());
                FormalPowerSeries<M> p(2*k);
                for (int i = 0; i < k; i++) p[2 * i].val = f[i].val;
                return p;
            }
            Q.resize(4*n*k);
            Q[2*n*k].val = 1;
            inplaceDFT(Q);
            FormalPowerSeries<M> nxt_Q(2*n*k);
            for (int i = 0; i < 2*n*k; i++) nxt_Q[i].val = Q[2*i].val*(ull)Q[2*i+1].val%M;
            
            inplaceIDFT(nxt_Q);
            ull iz = inverse_mod<ll,ll>(nxt_Q.size(), M);
            for (uint i = 0; i < nxt_Q.size(); i++) nxt_Q[i].val = nxt_Q[i].val*iz%M;

            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) nxt_Q[n*j+i].val = 0;
            nxt_Q[0].val = 0;
            FormalPowerSeries p = rec(rec, n/2, 2*k, nxt_Q);
            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) p[n * j + i].val = 0;
            
            iz = mtable.invmodlist[p.size()];
            for (uint i = 0; i < p.size(); i++) p[i].val = p[i].val*iz%M;
            inplaceIDFT_T(p);
            
            p.resize(4*n*k);
            for (int i = 2*n*k-1; i >= 0; i--){
                p[2*i+1].val = (ull)(mtable.invmodlist[2]-1+M)%M*W[i].val%M*Q[2*i].val%M*p[i].val%M;
                p[2*i].val = (ull)mtable.invmodlist[2]*W[i].val%M*Q[2*i+1].val%M*p[i].val%M;
            }
            
            inplaceDFT_T(p);
            
            p.resize(2*n*k);
            return p;
        };

        FormalPowerSeries<M> Q(2*n);
        for(int i = 0; i < n; i++) Q[i].val = g[i].val == 0 ? 0 : M-g[i].val;
        FormalPowerSeries p = rec(rec, n, 1, Q);
        p.resize(n);
        reverse(p.f.begin(), p.f.end());
        p.resize(N);
        return p;
    }
    // f(x+a)を求める
    FormalPowerSeries<M> polynominal_taylor_shift(const FormalPowerSeries<M>& F, ll a, const mod_table<M>& mtable){
        ull b = M+a%M;
        if (b >= M){b -= M;}
        if (b == 0){return F;}
        int N = F.size();
        FormalPowerSeries<M> G(N), H(N);
        for (int i = 0; i < N; i++){
            G[i].val = F[N-1-i].val*(ull)mtable.factorialmodlist[N-1-i]%M;
            H[i].val = mtable.factorialmodinvlist[i];
        }
        ull t = b;
        for (int i = N-2; i >= 0; i--){
            G[i].val = G[i].val*t%M;
            t = t*b%M;
        }
        G = convolution(G,H);
        t = 1;
        b = inverse_mod<ll,ll>(b,M);
        for (int j = 0; j < N; j++){
            H[j].val = mtable.factorialmodinvlist[j]*t%M*G[N-1-j].val%M;
            t = t*b%M;
        }
        return H;
    }
    // fの微分を求める。
    FormalPowerSeries<M> differential(const FormalPowerSeries<M>& F){
        FormalPowerSeries<M> G = F;
        G.differential();
        return G;
    }
    // fの積分を求める。積分定数は0
    FormalPowerSeries<M> integral(const FormalPowerSeries<M>& F, const mod_table<M>& mtable){
        FormalPowerSeries<M> G = F;
        G.integral(mtable);
        return G;
    }
};

/// @brief `[x^N].val(P(x)/Q(x))` をmod Mで求める
template<ull M>
ll Bostan_Mori(const ll N, FormalPowerSeries<M> P, FormalPowerSeries<M> Q, const fps_operator<M>& op){
    assert(N >= 0);
    if (N == 0){
        return P[0].val*inverse_mod<ll,ll>(Q[0].val, M)%M;
    }
    FormalPowerSeries<M> Q_minus;
    const int maxloop = (N == 1 ? 1 : 65-__builtin_clzll(N-1));
    for (int _i_ = 0; _i_ < maxloop; _i_++){
        Q_minus.resize(Q.size());
        for (size_t i = 0; i < Q_minus.size(); i += 2){
            Q_minus[i].val = Q[i].val;
        }
        for (size_t i = 1; i < Q_minus.size(); i += 2){
            Q_minus[i].val = (M-1)*Q[i].val%M;
        }
        auto A = op.convolution(P,Q_minus);
        auto B = op.convolution(Q,Q_minus);
        Q.resize((B.size()+1)/2);
        for (size_t i = 0; i < B.size(); i += 2){
            Q[i/2].val = B[i].val;
        }

        P.resize((A.size()+!btest_for_fps(N,_i_))/2);
        for (size_t i = btest_for_fps(N,_i_); i < A.size(); i += 2){
            P[i/2].val = A[i].val;
        }
    }
    return P[0].val*inverse_mod<ll,ll>(Q[0].val, M)%M;
}


#endif /* FPS_OPERATION_HPP_ */