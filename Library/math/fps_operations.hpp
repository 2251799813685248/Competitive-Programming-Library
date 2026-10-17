#ifndef FPS_OPERATION_HPP_
#define FPS_OPERATION_HPP_

#include <array>
#include <list.hpp>
#include <algorithm>
#include <functional>
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif
#include <deque.hpp>
#include <fps.hpp>
#include <math_functions.hpp>
#include <mod_table.hpp>
#include <prime_and_divisors.hpp>
#include <modint.hpp>



using namespace std;
using ll = long long;
using uint = unsigned;
using ull = unsigned long long;
#define vall(A) A.begin(), A.end()

template<typename T> inline bool btest_for_fps(T K, int i){return K&(1ull<<i);}


/// @brief mod M上での形式的冪級数の計算を行う構造体
template<uint M>
struct fps_operator{
    template<uint T> using fps = FormalPowerSeries<T>;
    template<uint T> using mint = constant_modint<T>;
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
        list<ll> powroot{1};
        list<ll> powrootinv;
        while (powroot.back() >= 0){
            powroot.append(cipolla<M>(powroot.back()));
            if (powroot.back() == powroot[powroot.size()-2]){
                powroot.back() = M-powroot.back();
            }
        }
        powroot.pop();
        log_max_length = powroot.size()-1;
        last_powroot = powroot.back();
        for (auto v : powroot){
            powrootinv.append(inverse_mod<ll,ll>(v,M));
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

    // Public transforms return canonical residues in the original bit-reversed
    // order. IDFT remains unnormalized. Transposes share the same kernels.
private:
    template<bool InverseButterfly, bool InverseRoots>
    void transform_scalar(mint<M>* a, int n) const {
        const uint* rate = InverseRoots ? sum_ie : sum_e;
        const int h = __builtin_ctz(unsigned(n));
        for (int stage = 0; stage < h; ++stage) {
            const int p = InverseButterfly ? 1 << stage : n >> (stage + 1);
            uint w = to_montgomery(1);
            for (int offset = 0, s = 0; offset < n; offset += 2*p, ++s) {
                for (int i = 0; i < p; ++i) {
                    const uint l = a[offset+i].val, r = a[offset+i+p].val;
                    if constexpr (InverseButterfly) {
                        a[offset+i].val = mod_add(l, r);
                        a[offset+i+p].val = montgomery_mul(mod_sub(l, r), w);
                    } else {
                        const uint t = montgomery_mul(r, w);
                        a[offset+i].val = mod_add(l, t);
                        a[offset+i+p].val = mod_sub(l, t);
                    }
                }
                w = montgomery_mul(w, rate[__builtin_ctz(~unsigned(s))]);
            }
        }
    }

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    // Internal SIMD residues are in [0, 2M). M < 2^30 guarantees that
    // additions fit uint32_t and Montgomery products reduce to [0, 2M).
    __attribute__((target("avx2")))
    static inline __m256i mul_lazy256(__m256i a, __m256i b) {
        const __m256i mod = _mm256_set1_epi32(M);
        const __m256i ni = _mm256_set1_epi32(NEG_INV);
        const __m256i x = _mm256_mul_epu32(a, b);
        const __m256i y = _mm256_mul_epu32(_mm256_srli_epi64(a, 32), _mm256_srli_epi64(b, 32));
        // Only the low 32 bits of each 64-bit lane are consumed below; a
        // widening multiply avoids computing the unused odd 32-bit products.
        const __m256i qx = _mm256_mul_epu32(_mm256_mul_epu32(x, ni), mod);
        const __m256i qy = _mm256_mul_epu32(_mm256_mul_epu32(y, ni), mod);
        const __m256i rx = _mm256_srli_epi64(_mm256_add_epi64(x, qx), 32);
        const __m256i ry = _mm256_add_epi64(y, qy);
        const __m256i r = _mm256_blend_epi32(rx, ry, 0xaa);
        return r;
    }
    __attribute__((target("avx2")))
    static inline __m256i mul256(__m256i a, __m256i b) {
        const __m256i r = mul_lazy256(a,b);
        return _mm256_min_epu32(r,_mm256_sub_epi32(r,_mm256_set1_epi32(M)));
    }
    __attribute__((target("avx2")))
    static inline __m256i add_lazy256(__m256i a, __m256i b) {
        const __m256i r = _mm256_add_epi32(a, b);
        return _mm256_min_epu32(r, _mm256_sub_epi32(r, _mm256_set1_epi32(2*M)));
    }
    __attribute__((target("avx2")))
    static inline __m256i sub_lazy256(__m256i a, __m256i b) {
        const __m256i r = _mm256_sub_epi32(a, b);
        return _mm256_min_epu32(r, _mm256_add_epi32(r, _mm256_set1_epi32(2*M)));
    }

    __attribute__((target("avx2")))
    static inline __m256i shoup256(__m256i a, __m256i b, __m256i bq) {
        // b is an ordinary residue; bq = floor(b*2^32/M). The result is
        // congruent to a*b and lies in [0, 2M), without a final correction.
        const __m256i q0 = _mm256_srli_epi64(_mm256_mul_epu32(a,bq),32);
        const __m256i q1 = _mm256_mul_epu32(_mm256_srli_epi64(a,32),_mm256_srli_epi64(bq,32));
        const __m256i q = _mm256_blend_epi32(q0,q1,0xaa);
        const __m256i mod = _mm256_set1_epi32(M);
        const __m256i r = _mm256_sub_epi32(_mm256_mullo_epi32(a,b),_mm256_mullo_epi32(q,mod));
        return r;
    }

    template<bool InverseButterfly, bool InverseRoots, bool Canonical = true, bool ZeroUpperHalf = false>
    __attribute__((target("avx2")))
    void transform_avx2(mint<M>* a, int n) const {
        const uint* rate = InverseRoots ? sum_ie : sum_e;
        const int h = __builtin_ctz(unsigned(n));
        uint rate4[29];
        for (int i = 0; i+1 < h; ++i) rate4[i] = montgomery_mul(rate[0],rate[i+1]);
        const __m256i imag = _mm256_set1_epi32(rate[0]);
        const uint im = from_montgomery(rate[0]);
        const __m256i imv = _mm256_set1_epi32(im);
        const __m256i imq = _mm256_set1_epi32((ull(im)<<32)/M);
        int first_stage = 0;
        if constexpr (ZeroUpperHalf && !InverseButterfly) {
            // The first radix-4 block has no twiddle and its upper two inputs
            // are zero. Do not load padding or execute the redundant butterflies.
            const int p = n/4;
            for (int i = 0; i < p; i += 8) {
                const __m256i x = _mm256_loadu_si256((const __m256i*)(a+i));
                const __m256i y = _mm256_loadu_si256((const __m256i*)(a+i+p));
                const __m256i t = shoup256(y,imv,imq);
                _mm256_storeu_si256((__m256i*)(a+i),add_lazy256(x,y));
                _mm256_storeu_si256((__m256i*)(a+i+p),sub_lazy256(x,y));
                _mm256_storeu_si256((__m256i*)(a+i+2*p),add_lazy256(x,t));
                _mm256_storeu_si256((__m256i*)(a+i+3*p),sub_lazy256(x,t));
            }
            first_stage = 2;
        }
        for (int stage = first_stage; stage < h; ++stage) {
            const int p4 = InverseButterfly ? 1 << stage : n >> (stage + 2);
            if (stage + 1 < h && p4 >= 8) {
                uint w = to_montgomery(1);
                for (int offset = 0, s = 0; offset < n; offset += 4*p4, ++s) {
                    const uint w2 = montgomery_mul(w,w), w3 = montgomery_mul(w2,w);
                    const uint b1 = from_montgomery(w), b2 = from_montgomery(w2), b3 = from_montgomery(w3);
                    const __m256i v1 = _mm256_set1_epi32(b1), q1 = _mm256_set1_epi32((ull(b1)<<32)/M);
                    const __m256i v2 = _mm256_set1_epi32(b2), q2 = _mm256_set1_epi32((ull(b2)<<32)/M);
                    const __m256i v3 = _mm256_set1_epi32(b3), q3 = _mm256_set1_epi32((ull(b3)<<32)/M);
                    for (int i = 0; i < p4; i += 8) {
                        __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+offset+i));
                        __m256i a1 = _mm256_loadu_si256((const __m256i*)(a+offset+i+p4));
                        __m256i a2 = _mm256_loadu_si256((const __m256i*)(a+offset+i+2*p4));
                        __m256i a3 = _mm256_loadu_si256((const __m256i*)(a+offset+i+3*p4));
                        __m256i x,y,z,t;
                        if constexpr (InverseButterfly) {
                            x = add_lazy256(a0,a1); y = sub_lazy256(a0,a1);
                            z = add_lazy256(a2,a3); t = shoup256(sub_lazy256(a2,a3),imv,imq);
                            a0 = add_lazy256(x,z); a1 = add_lazy256(y,t);
                            a2 = sub_lazy256(x,z); a3 = sub_lazy256(y,t);
                            if (s != 0) {
                                a1 = shoup256(a1,v1,q1); a2 = shoup256(a2,v2,q2); a3 = shoup256(a3,v3,q3);
                            }
                        } else {
                            if (s != 0) {
                                a1 = shoup256(a1,v1,q1); a2 = shoup256(a2,v2,q2); a3 = shoup256(a3,v3,q3);
                            }
                            x = add_lazy256(a0,a2); y = add_lazy256(a1,a3);
                            z = sub_lazy256(a0,a2); t = shoup256(sub_lazy256(a1,a3),imv,imq);
                            a0 = add_lazy256(x,y); a1 = sub_lazy256(x,y);
                            a2 = add_lazy256(z,t); a3 = sub_lazy256(z,t);
                        }
                        _mm256_storeu_si256((__m256i*)(a+offset+i),a0);
                        _mm256_storeu_si256((__m256i*)(a+offset+i+p4),a1);
                        _mm256_storeu_si256((__m256i*)(a+offset+i+2*p4),a2);
                        _mm256_storeu_si256((__m256i*)(a+offset+i+3*p4),a3);
                    }
                    w = montgomery_mul(w,rate4[__builtin_ctz(~unsigned(s))]);
                }
                ++stage;
                continue;
            }
            if (stage + 1 < h && n >= 32) {
                const int p = p4, groups = 8/p;
                alignas(32) uint lane1[8], lane2[8], lane3[8];
                uint w = to_montgomery(1), last = w;
                for (int s = 0; s < groups; ++s) {
                    const uint w2 = montgomery_mul(w,w), w3 = montgomery_mul(w2,w);
                    for (int i = 0; i < p; ++i) {
                        const int j = p == 1 ? (s/2 + (s%2)*4) : s*p+i;
                        lane1[j] = w; lane2[j] = w2; lane3[j] = w3;
                    }
                    last = w;
                    w = montgomery_mul(w,rate4[__builtin_ctz(~unsigned(s))]);
                }
                __m256i v1 = _mm256_load_si256((const __m256i*)lane1);
                __m256i v2 = _mm256_load_si256((const __m256i*)lane2);
                __m256i v3 = _mm256_load_si256((const __m256i*)lane3);
                const int shift = __builtin_ctz(unsigned(groups));
                uint advance1[30], advance2[30], advance3[30];
                for (int i = 0; i+shift+1 < h; ++i) {
                    const uint d = montgomery_mul(last,rate4[i+shift]);
                    advance1[i] = d;
                    advance2[i] = montgomery_mul(d,d);
                    advance3[i] = montgomery_mul(advance2[i],d);
                }
                for (int offset = 0, s = 0; offset < n; offset += 32, ++s) {
                    __m256i x0 = _mm256_loadu_si256((const __m256i*)(a+offset));
                    __m256i x1 = _mm256_loadu_si256((const __m256i*)(a+offset+8));
                    __m256i x2 = _mm256_loadu_si256((const __m256i*)(a+offset+16));
                    __m256i x3 = _mm256_loadu_si256((const __m256i*)(a+offset+24));
                    __m256i a0,a1,a2,a3;
                    if (p == 1) {
                        const __m256i t0 = _mm256_unpacklo_epi32(x0,x1), t1 = _mm256_unpackhi_epi32(x0,x1);
                        const __m256i t2 = _mm256_unpacklo_epi32(x2,x3), t3 = _mm256_unpackhi_epi32(x2,x3);
                        a0 = _mm256_unpacklo_epi64(t0,t2); a1 = _mm256_unpackhi_epi64(t0,t2);
                        a2 = _mm256_unpacklo_epi64(t1,t3); a3 = _mm256_unpackhi_epi64(t1,t3);
                    } else if (p == 2) {
                        const __m256i t0 = _mm256_unpacklo_epi64(x0,x1), t1 = _mm256_unpackhi_epi64(x0,x1);
                        const __m256i t2 = _mm256_unpacklo_epi64(x2,x3), t3 = _mm256_unpackhi_epi64(x2,x3);
                        a0 = _mm256_permute2x128_si256(t0,t2,0x20); a1 = _mm256_permute2x128_si256(t1,t3,0x20);
                        a2 = _mm256_permute2x128_si256(t0,t2,0x31); a3 = _mm256_permute2x128_si256(t1,t3,0x31);
                    } else {
                        a0 = _mm256_permute2x128_si256(x0,x2,0x20); a1 = _mm256_permute2x128_si256(x0,x2,0x31);
                        a2 = _mm256_permute2x128_si256(x1,x3,0x20); a3 = _mm256_permute2x128_si256(x1,x3,0x31);
                    }
                    __m256i x,y,z,t;
                    if constexpr (InverseButterfly) {
                        x = add_lazy256(a0,a1); y = sub_lazy256(a0,a1);
                        z = add_lazy256(a2,a3); t = mul_lazy256(sub_lazy256(a2,a3),imag);
                        a0 = add_lazy256(x,z); a1 = mul_lazy256(add_lazy256(y,t),v1);
                        a2 = mul_lazy256(sub_lazy256(x,z),v2); a3 = mul_lazy256(sub_lazy256(y,t),v3);
                    } else {
                        a1 = mul_lazy256(a1,v1); a2 = mul_lazy256(a2,v2); a3 = mul_lazy256(a3,v3);
                        x = add_lazy256(a0,a2); y = add_lazy256(a1,a3);
                        z = sub_lazy256(a0,a2); t = mul_lazy256(sub_lazy256(a1,a3),imag);
                        a0 = add_lazy256(x,y); a1 = sub_lazy256(x,y);
                        a2 = add_lazy256(z,t); a3 = sub_lazy256(z,t);
                    }
                    if (p == 1) {
                        const __m256i t0 = _mm256_unpacklo_epi32(a0,a1), t1 = _mm256_unpackhi_epi32(a0,a1);
                        const __m256i t2 = _mm256_unpacklo_epi32(a2,a3), t3 = _mm256_unpackhi_epi32(a2,a3);
                        x0 = _mm256_unpacklo_epi64(t0,t2); x1 = _mm256_unpackhi_epi64(t0,t2);
                        x2 = _mm256_unpacklo_epi64(t1,t3); x3 = _mm256_unpackhi_epi64(t1,t3);
                    } else if (p == 2) {
                        const __m256i t0 = _mm256_permute2x128_si256(a0,a2,0x20), t1 = _mm256_permute2x128_si256(a1,a3,0x20);
                        const __m256i t2 = _mm256_permute2x128_si256(a0,a2,0x31), t3 = _mm256_permute2x128_si256(a1,a3,0x31);
                        x0 = _mm256_unpacklo_epi64(t0,t1); x1 = _mm256_unpackhi_epi64(t0,t1);
                        x2 = _mm256_unpacklo_epi64(t2,t3); x3 = _mm256_unpackhi_epi64(t2,t3);
                    } else {
                        x0 = _mm256_permute2x128_si256(a0,a1,0x20); x1 = _mm256_permute2x128_si256(a2,a3,0x20);
                        x2 = _mm256_permute2x128_si256(a0,a1,0x31); x3 = _mm256_permute2x128_si256(a2,a3,0x31);
                    }
                    _mm256_storeu_si256((__m256i*)(a+offset),x0);
                    _mm256_storeu_si256((__m256i*)(a+offset+8),x1);
                    _mm256_storeu_si256((__m256i*)(a+offset+16),x2);
                    _mm256_storeu_si256((__m256i*)(a+offset+24),x3);
                    const int k = __builtin_ctz(~unsigned(s));
                    v1 = mul256(v1,_mm256_set1_epi32(advance1[k]));
                    v2 = mul256(v2,_mm256_set1_epi32(advance2[k]));
                    v3 = mul256(v3,_mm256_set1_epi32(advance3[k]));
                }
                ++stage;
                continue;
            }
            const int p = InverseButterfly ? 1 << stage : n >> (stage + 1);
            uint w = to_montgomery(1);
            if (p >= 8) {
                for (int offset = 0, s = 0; offset < n; offset += 2*p, ++s) {
                    const __m256i vw = _mm256_set1_epi32(w);
                    for (int i = 0; i < p; i += 8) {
                        __m256i l = _mm256_loadu_si256((const __m256i*)(a+offset+i));
                        __m256i r = _mm256_loadu_si256((const __m256i*)(a+offset+i+p));
                        __m256i x, y;
                        if constexpr (InverseButterfly) {
                            x = add_lazy256(l, r); y = mul_lazy256(sub_lazy256(l, r), vw);
                        } else {
                            r = mul_lazy256(r, vw); x = add_lazy256(l, r); y = sub_lazy256(l, r);
                        }
                        _mm256_storeu_si256((__m256i*)(a+offset+i), x);
                        _mm256_storeu_si256((__m256i*)(a+offset+i+p), y);
                    }
                    w = montgomery_mul(w, rate[__builtin_ctz(~unsigned(s))]);
                }
            } else {
                // Pack 8 butterflies even when each block has only 1, 2 or 4 pairs.
                alignas(32) uint lane[8]{};
                uint t = to_montgomery(1), last = t;
                const int groups = 8/p;
                for (int s = 0; s < groups; ++s) {
                    for (int i = 0; i < p; ++i) lane[s*p+i] = t;
                    last = t;
                    t = montgomery_mul(t, rate[__builtin_ctz(~unsigned(s))]);
                }
                const __m256i base = _mm256_load_si256((const __m256i*)lane);
                const __m256i unpack = p == 1 ? _mm256_setr_epi32(0,2,4,6,1,3,5,7)
                                             : _mm256_setr_epi32(0,1,4,5,2,3,6,7);
                const __m256i pack = p == 1 ? _mm256_setr_epi32(0,4,1,5,2,6,3,7) : unpack;
                const int shift = __builtin_ctz(unsigned(groups));
                uint advance[30];
                for (int i = 0; i + shift < h; ++i) advance[i] = montgomery_mul(last, rate[i+shift]);
                for (int offset = 0, s = 0; offset < n; offset += 16, ++s) {
                    __m256i x = _mm256_loadu_si256((const __m256i*)(a+offset));
                    __m256i y = _mm256_loadu_si256((const __m256i*)(a+offset+8));
                    if (p != 4) {
                        x = _mm256_permutevar8x32_epi32(x, unpack);
                        y = _mm256_permutevar8x32_epi32(y, unpack);
                    }
                    __m256i l = _mm256_permute2x128_si256(x,y,0x20);
                    __m256i r = _mm256_permute2x128_si256(x,y,0x31);
                    const __m256i vw = mul256(base, _mm256_set1_epi32(w));
                    if constexpr (InverseButterfly) {
                        x = add_lazy256(l,r); y = mul_lazy256(sub_lazy256(l,r),vw);
                    } else {
                        r = mul_lazy256(r,vw); x = add_lazy256(l,r); y = sub_lazy256(l,r);
                    }
                    l = _mm256_permute2x128_si256(x,y,0x20);
                    r = _mm256_permute2x128_si256(x,y,0x31);
                    if (p != 4) {
                        l = _mm256_permutevar8x32_epi32(l,pack);
                        r = _mm256_permutevar8x32_epi32(r,pack);
                    }
                    _mm256_storeu_si256((__m256i*)(a+offset),l);
                    _mm256_storeu_si256((__m256i*)(a+offset+8),r);
                    w = montgomery_mul(w,advance[__builtin_ctz(~unsigned(s))]);
                }
            }
        }
        const __m256i mod = _mm256_set1_epi32(M);
        if constexpr (Canonical) for (int i = 0; i < n; i += 8) {
            __m256i x = _mm256_loadu_si256((const __m256i*)(a+i));
            x = _mm256_min_epu32(x,_mm256_sub_epi32(x,mod));
            _mm256_storeu_si256((__m256i*)(a+i),x);
        }
    }

    template<bool Canonical = true>
    __attribute__((target("avx2")))
    static void multiply_avx2(mint<M>* dst, const mint<M>* a, const mint<M>* b, int n, uint scale) {
        const __m256i vs = _mm256_set1_epi32(scale);
        for (int i = 0; i < n; i += 8) {
            __m256i x = _mm256_loadu_si256((const __m256i*)(a+i));
            const __m256i y = _mm256_loadu_si256((const __m256i*)(b+i));
            x = mul_lazy256(mul_lazy256(x,y),vs);
            if constexpr (Canonical)
                x = _mm256_min_epu32(x,_mm256_sub_epi32(x,_mm256_set1_epi32(M)));
            _mm256_storeu_si256((__m256i*)(dst+i),x);
        }
    }
#endif

    static bool use_avx2() {
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        // This check also makes ordinary -O2/-O3 builds use SIMD on supported CPUs.
#ifdef FPS_DISABLE_SIMD
        return false;
#else
        return M < (1u << 30) && __builtin_cpu_supports("avx2");
#endif
#else
        return false;
#endif
    }

    // Pointwise product with optional normalization, all operands in ordinary
    // representation. dst may alias either input.
    void pointwise_product(fps<M>& dst, const fps<M>& lhs, const fps<M>& rhs,
                           int n, mint<M> factor = 1) const {
        if (n == 0) return;
        auto* out = &dst[0];
        const auto* a = &lhs[0];
        const auto* b = &rhs[0];
        int i = 0;
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        if (use_avx2()) {
            i = n & ~7;
            multiply_avx2(out,a,b,i,ull(R2)*factor.val%M);
        }
#endif
        for (; i < n; ++i) out[i] = a[i]*b[i]*factor;
    }

    template<bool InverseButterfly, bool InverseRoots>
    void transform(fps<M>& F) const {
        if (F.empty()) return;
        F.f.reserve(upperpow2(F.sz));
        F.resize(upperpow2(F.sz));
        const int n = F.size();
        assert(unsigned(n) <= (1u << log_max_length));
        auto* a = &F[0];
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        if (n >= 16 && use_avx2()) {
            transform_avx2<InverseButterfly, InverseRoots>(a,n);
            return;
        }
#endif
        transform_scalar<InverseButterfly, InverseRoots>(a,n);
    }
public:
    void inplaceDFT(fps<M>& F) const { transform<false,false>(F); }
    void inplaceIDFT(fps<M>& F) const { transform<true,true>(F); }
    void inplaceDFT_T(fps<M>& F) const { transform<true,false>(F); }
    void inplaceIDFT_T(fps<M>& F) const { transform<false,true>(F); }

private:
    static fps<M> convolution_copy(const fps<M>& src, int capacity) {
        assert(capacity > 0 && src.size() <= unsigned(capacity));
        fps<M> result(capacity);
        result.resize(src.size());
        if (!src.empty()) std::copy_n(src.data(),src.size(),result.data());
        return result;
    }
    static int convolution_capacity(const fps<M>& a, const fps<M>& b) {
        if (a.empty() || b.empty() || std::min(a.size(),b.size()) <= 60) return 0;
        if (std::max(a.size(),b.size())/std::min(a.size(),b.size()) >= 16) return 0;
        return upperpow2(a.size()+b.size()-1);
    }
    fps<M> convolution_owned(fps<M> F1, fps<M> F2) const {
        int n = F1.size(), m = F2.size();
        if (n == 0 || m == 0) return fps<M>(0);
        if (std::min(n,m) <= 60) {
            if (n < m) { std::swap(n,m); std::swap(F1,F2); }
            fps<M> ans(n+m-1);
            auto* c = &ans[0];
            const auto* a = &F1[0];
            const auto* b = &F2[0];
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j) c[i+j] += a[i]*b[j];
            return ans;
        }
        if (std::max(n,m)/std::min(n,m) >= 16) {
            if (n < m) { std::swap(n,m); std::swap(F1,F2); }
            // Overlap-add: reuse the short operand's transform for each block
            // instead of padding it to the length of the entire long operand.
            const int length = upperpow2(2*m-1), step = length-m+1;
            F2.f.reserve(length); F2.resize(length);
            inplaceDFT(F2);
            fps<M> block(length), ans(n+m-1);
            const mint<M> scale = mint<M>(length).inv();
            for (int start = 0; start < n; start += step) {
                const int count = std::min(step,n-start);
                std::copy_n(F1.data()+start,count,block.data());
                std::fill(block.data()+count,block.data()+length,mint<M>(0));
                inplaceDFT(block);
                pointwise_product(block,block,F2,length,scale);
                inplaceIDFT(block);
                for (int i = 0; i < count+m-1; ++i) ans.data()[start+i] += block.data()[i];
            }
            return ans;
        }
        const int size = upperpow2(n+m-1);
        assert(unsigned(size) <= (1u << log_max_length));
        F1.f.reserve(size); F2.f.reserve(size);
        F1.resize(size); F2.resize(size);
        auto* a = &F1[0];
        const auto* b = &F2[0];
        // Two Montgomery reductions cancel R^2. Include 1/size here, so the
        // inverse transform directly returns normalized, ordinary residues.
        const uint scale = ull(R2)*modpow<M>(size,M-2)%M;
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        if (use_avx2()) {
            if (n <= size/2) transform_avx2<false,false,false,true>(a,size);
            else transform_avx2<false,false,false>(a,size);
            if (m <= size/2) transform_avx2<false,false,false,true>(&F2[0],size);
            else transform_avx2<false,false,false>(&F2[0],size);
            multiply_avx2<false>(a,a,b,size,scale);
            transform_avx2<true,true>(a,size);
        } else
#endif
        {
            inplaceDFT(F1); inplaceDFT(F2);
            for (int i = 0; i < size; ++i)
                a[i].val = montgomery_mul(montgomery_mul(a[i].val,b[i].val),scale);
            inplaceIDFT(F1);
        }
        F1.resize(n+m-1);
        return F1;
    }
public:
    // Copy lvalues directly into the final NTT allocation. Rvalues retain their
    // storage, so std::move still avoids copying when the caller is finished.
    fps<M> convolution(const fps<M>& a, const fps<M>& b) const {
        const int capacity = convolution_capacity(a,b);
        if (capacity == 0) return convolution_owned(a,b);
        return convolution_owned(convolution_copy(a,capacity),convolution_copy(b,capacity));
    }
    fps<M> convolution(fps<M>&& a, fps<M>&& b) const {
        // Moving the same series twice must retain the original square semantics.
        if (&a == &b) return convolution_owned(a,b);
        return convolution_owned(std::move(a),std::move(b));
    }
    fps<M> convolution(fps<M>&& a, const fps<M>& b) const {
        const int capacity = convolution_capacity(a,b);
        auto copy = capacity ? convolution_copy(b,capacity) : b;
        return convolution_owned(std::move(a),std::move(copy));
    }
    fps<M> convolution(const fps<M>& a, fps<M>&& b) const {
        const int capacity = convolution_capacity(a,b);
        auto copy = capacity ? convolution_copy(a,capacity) : a;
        return convolution_owned(std::move(copy),std::move(b));
    }
    fps<M> add(const fps<M>& F1, const fps<M>& F2) const {
        fps<M> ret(max(F1.size(),F2.size()));
        for (uint i = 0; i < F1.size(); ++i) ret[i] = F1[i];
        for (uint i = 0; i < F2.size(); ++i) ret[i] += F2[i];
        return ret;
    }
    fps<M> subtract(const fps<M>& F1, const fps<M>& F2) const {
        fps<M> ret(max(F1.size(),F2.size()));
        for (uint i = 0; i < F1.size(); ++i) ret[i] = F1[i];
        for (uint i = 0; i < F2.size(); ++i) ret[i] -= F2[i];
        return ret;
    }
    /// @brief F*G == 1 mod x^n となるGを求める F[x^0] != 0 が必要 
    fps<M> inv(const fps<M>& F, const int n) const {
        assert(n >= 0);
        if (n == 0) return fps<M>(0);
        assert(!F.empty() && F[0] != 0);
        fps<M> res{F[0].inv()};
        for (int d = 1; d < n; d <<= 1) {
            fps<M> f(2*d), g(2*d);
            for (int j = 0, limit = min<int>(F.size(),2*d); j < limit; ++j) f[j] = F[j];
            for (int j = 0; j < d; ++j) g[j] = res[j];
            inplaceDFT(f); inplaceDFT(g);
            const mint<M> iz = mint<M>(2*d).inv();
            pointwise_product(f,f,g,2*d,iz);
            inplaceIDFT(f);
            for (int j = 0; j < d; ++j) { f[j] = 0; f[j+d] = -f[j+d]; }
            inplaceDFT(f);
            pointwise_product(f,f,g,2*d,iz);
            inplaceIDFT(f);
            for (int j = 0; j < d; ++j) f[j] = res[j];
            res = std::move(f);
        }
        res.resize(n);
        return res;
    }
    /// @brief F÷G の商と余りを求める。{商, 余り}として返される。
    array<fps<M>,2> div(fps<M> F, fps<M>G) const {
        while (!F.f.empty() && F.f.back() == 0){
            F.f.pop();
            F.sz--;
        }
        while (!G.f.empty() && G.f.back() == 0){
            G.f.pop();
            G.sz--;
        }
        int n = F.sz, m = G.sz;

        if (n < m){return {fps<M>{}, F};}

        if (min(m, n-m+1) <= 100){
            fps<M> res(n-m+1);
            mint<M> t = G.f.back().inv();
            for (int i = n-1; i >= m-1; i--){
                auto u = t*F[i];
                res[i-m+1] = u;
                for (int j = 0; j < m; j++){
                    F[i-j] -= G[m-1-j]*u;
                }
            }
            while (!F.f.empty() && F.f.back() == 0){
                F.f.pop();
                F.sz--;
            }
            while (!G.f.empty() && G.f.back() == 0){
                G.f.pop();
                G.sz--;
            }
            return {res, F};
        }

        auto F2 = F;
        auto G2 = G;

        reverse(vall(F.f));
        reverse(vall(G.f));
        while (len(F.f) > n-m+1){
            F.f.pop();
            F.sz--;
        }
        while (len(G.f) > n-m+1){
            G.f.pop();
            G.sz--;
        }
        auto res = convolution(F, inv(G, n-m+1));
        while (len(res.f) > n-m+1){
            res.f.pop();
            res.sz--;
        }
        reverse(vall(res.f));
        auto res2 = subtract(F2, convolution(res, G2));
        while (!res2.f.empty() && res2.f.back() == 0){
            res2.f.pop();
            res2.sz--;
        }
        return {res, res2};
    }
    /// @brief F を G で割った余りを求める。
    fps<M> remainder(fps<M> F, const fps<M>& G) const {
        while (!F.f.empty() && F.f.back() == 0){
            F.f.pop();
            F.sz--;
        }
        assert(!G.f.empty() && G.f.back() != 0);

        int n = F.sz, m = G.sz;
        if (n < m) return F;

        if (min(m, n-m+1) <= 100){
            const bool monic = (G[m-1] == mint<M>(1));
            const mint<M> inverse_leading_coefficient = monic ? mint<M>(1) : G[m-1].inv();
            for (int i = n-1; i >= m-1; i--){
                if (F[i] == 0) continue;
                mint<M> q = monic ? F[i] : inverse_leading_coefficient*F[i];
                for (int j = 0; j < m; j++){
                    F[i-j] -= G[m-1-j]*q;
                }
            }
            F.resize(m-1);
            while (!F.f.empty() && F.f.back() == 0){
                F.f.pop();
                F.sz--;
            }
            return F;
        }

        auto qr = div(std::move(F), G);
        return std::move(qr[1]);
    }
    /// @brief exp(F) mod x^n を求める F[x^0] == 0 が必要
    fps<M> exp(const fps<M>& F, const int n, const mod_table<M>& mtable) const {
        assert(n >= 0);
        if (n == 0) return fps<M>(0);
        assert(F.empty() || F[0] == 0);
        ull sz_f = F.sz;
        fps<M> F0{1},f0{1},g0{1};
        for(int d = 1; d < n; d<<=1){
            auto G0 = g0;
            inplaceDFT(G0);
            const mint<M> iz = mint<M>(d).inv(), iz2 = mint<M>(2*d).inv();
            fps<M> Delta(d);
            pointwise_product(Delta,F0,G0,d,iz);
            inplaceIDFT(Delta);
            Delta[0] -= 1;
            fps<M> delta(2*d);
            for(int j = 0; j < d; j++) delta[d+j] = Delta[j];
            fps<M> epsilon(2*d);

            fps<M> DF0(d-1);
            for (ull j = 0, ulld = d; j < ulld-1; j++) DF0[j] = f0[j+1]*mint<M>(j+1);
            DF0.f.append(0);
            DF0.sz++;
            inplaceDFT(DF0);
            pointwise_product(DF0,DF0,G0,d,iz);
            inplaceIDFT(DF0);
            for (ull j = 0, limit = d-1; j < limit; ++j) {
                const auto df = j+1 < sz_f ? F[j+1]*mint<M>(j+1) : mint<M>(0);
                epsilon[j] = df;
                epsilon[j+d] = DF0[j]-df;
            }
            epsilon[d-1] = DF0[d-1];
            Delta = std::move(delta);
            inplaceDFT(Delta);
            fps<M> DH0(d-1);
            for (ull j = 0, limit = d-1; j < limit; j++) DH0[j] = j+1 < sz_f ? F[j+1]*mint<M>(j+1) : mint<M>(0);
            DH0.resize(2*d);
            inplaceDFT(DH0);
            pointwise_product(Delta,Delta,DH0,2*d,iz2);
            inplaceIDFT(Delta);
            for(int j = 0; j < d; j++){
                epsilon[j+d] -= Delta[j+d];
            }
        
            for (ull i = 2*d-1; i >= 1; i--){
                epsilon[i] = epsilon[i-1]*mtable.inverse_mod(i)-(i < sz_f ? F[i] : mint<M>(0));
            }
            epsilon[0] = 0;
            
            auto Epsilon = std::move(epsilon);
            inplaceDFT(Epsilon);
            for (int j = 0; j < d; j++){DH0[j] = f0[j]; DH0[j+d].val = 0;}
            inplaceDFT(DH0);
            pointwise_product(Epsilon,Epsilon,DH0,2*d,iz2);
            inplaceIDFT(Epsilon);
            f0.resize(2*d);
            for (int j = 0; j < d; j++){
                f0[j+d] -= Epsilon[j+d];
            }
            if(2*d >= n) break;
            
            G0.resize(2*d);
            for (int j = 0; j < d; j++) G0[j] = g0[j];
            inplaceDFT(G0);
            F0 = f0;
            inplaceDFT(F0);
            fps<M> T(2*d);
            pointwise_product(T,F0,G0,2*d,iz2);
            inplaceIDFT(T);
            for (int j = 0; j < d; j++){
                T[j].val = 0;
                T[j+d] = -T[j+d];
            }
            inplaceDFT(T);
            pointwise_product(T,T,G0,2*d,iz2);
            inplaceIDFT(T);
            for (int j = 0; j < d; j++) T[j] = g0[j];
            g0 = std::move(T);
        }
        f0.resize(n);
        return f0;
    }
    /// @brief log(F) mod x^n を求める F[x^0] == 1 が必要 
    fps<M> log(const fps<M>& F, const int n, const mod_table<M>& mtable) const {
        assert(n >= 0);
        if (n == 0) return fps<M>(0);
        assert(!F.empty() && F[0] == 1);
        const int degree = min<int>(F.size(),n)-1;
        fps<M> derivative(degree);
        for (int i = 0; i < degree; ++i) derivative[i] = F[i+1]*mint<M>(i+1);
        auto res = convolution(std::move(derivative),inv(F,n));
        res.resize(n);
        for (int i = n-1; i >= 1; --i) res[i] = res[i-1]*mtable.inverse_mod(i);
        res[0] = 0;
        return res;
    }
    /// @brief F^k mod x^n を求める
    fps<M> pow(const fps<M>& F, const ull k, const int n, const mod_table<M>& mtable) const {
        assert(n >= 0);
        if (n == 0) return fps<M>(0);
        fps<M> ret(n);
        if (k == 0) { ret[0] = 1; return ret; }
        int lowest = 0;
        while (lowest < min<int>(F.size(),n) && F[lowest] == 0) ++lowest;
        if (lowest == min<int>(F.size(),n)) return ret;
        // Check before multiplying, including k = ULLONG_MAX.
        if (lowest > 0 && k > ull((n-1)/lowest)) return ret;
        const int offset = ull(lowest)*k;
        const int precision = n-offset;
        const auto leading = F[lowest], inverse_leading = leading.inv();
        fps<M> g(precision);
        for (int i = 0, limit = min<int>(precision,F.size()-lowest); i < limit; ++i)
            g[i] = F[lowest+i]*inverse_leading;
        auto logarithm = log(g,precision,mtable);
        const mint<M> exponent = k;
        for (auto& x : logarithm.f) x *= exponent;
        auto result = exp(logarithm,precision,mtable);
        const auto scale = leading.pow(k);
        for (int i = 0; i < precision; ++i) ret[offset+i] = result[i]*scale;
        return ret;
    }
    // f(g(x)) mod x^len(f) を求める
    fps<M> composition(fps<M> f, fps<M> g, const mod_table<M>& mtable) const {
        f.resize(max(f.size(),g.size()));
        int N = f.size();
        if (N == 0) return fps<M>(0);

        int n = 1;
        while (n < (int)f.size()) n *= 2;
        f.resize(n), g.resize(n);

        fps<M> W(2*n);
        {
            // bit reverse order
            list<int> btr(2*n);
            int log = 31-__builtin_clz(2*n);
            for (int i = 0; i < 2*n; i++){btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1));}
            int t = log_max_length;
            uint r = last_powroot;
            ull dw = modpow<M>(inverse_mod<ll,ll>(r, M), (1<<t)/(4*n));
            uint w = 1;
            for (auto i: btr) { W[i].val = w, w = w*dw%M; }
        }

        auto rec = [&](auto &rec, int n, int k, fps<M> &Q) -> fps<M> {
            if (n == 1) {
                reverse(f.f.begin(), f.f.end());
                fps<M> p(2*k);
                for (int i = 0; i < k; i++) p[2 * i] = f[i];
                return p;
            }
            Q.resize(4*n*k);
            Q[2*n*k].val = 1;
            inplaceDFT(Q);
            fps<M> nxt_Q(2*n*k);
            for (int i = 0; i < 2*n*k; i++) nxt_Q[i] = Q[2*i]*Q[2*i+1];
            
            inplaceIDFT(nxt_Q);
            ull iz = inverse_mod<ll,ll>(nxt_Q.size(), M);
            for (uint i = 0; i < nxt_Q.size(); i++) nxt_Q[i].val = nxt_Q[i].val*iz%M;

            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) nxt_Q[n*j+i].val = 0;
            nxt_Q[0].val = 0;
            fps<M> p = rec(rec, n/2, 2*k, nxt_Q);
            for (int j = 0; j < 2*k; j++) for (int i = n/2; i < n; i++) p[n * j + i].val = 0;
            
            iz = mtable.inverse_mod(p.size()).val;
            for (uint i = 0; i < p.size(); i++) p[i].val = p[i].val*iz%M;
            inplaceIDFT_T(p);
            
            p.resize(4*n*k);
            for (int i = 2*n*k-1; i >= 0; i--){
                const auto t = mtable.inverse_mod(2)*W[i]*p[i];
                p[2*i+1] = -t*Q[2*i];
                p[2*i] = t*Q[2*i+1];
            }
            
            inplaceDFT_T(p);
            
            p.resize(2*n*k);
            return p;
        };

        fps<M> Q(2*n);
        for(int i = 0; i < n; i++) Q[i].val = g[i].val == 0 ? 0 : M-g[i].val;
        fps<M> p = rec(rec, n, 1, Q);
        p.resize(n);
        reverse(p.f.begin(), p.f.end());
        p.resize(N);
        return p;
    }
    fps<M> power_projection(fps<M> f, fps<M> wt, int m, const mod_table<M>& mtable) const {
        assert(len(f) == len(wt));
        if (f.empty()) { return fps<M>(m + 1, 0); }
        if (f[0] != 0) {
            auto c = f[0];
            f[0] = 0;
            fps<M> A = power_projection(f, wt, m, mtable);
            for (int p = 0; p < m + 1; p++) A[p] *= mtable.factorialmodinv(p);
            fps<M> B(m + 1);
            mint<M> pow = 1;
            for (int q = 0; q < m + 1; q++) B[q] = pow * mtable.factorialmodinv(q), pow *= c;
            A = convolution(A, B);
            A.resize(m + 1);
            for (int i = 0; i < m + 1; i++) A[i] *= mtable.factorialmod(i);
            return A;
        }

        int n = 1;
        while (n < len(f)) n *= 2;

        for (auto& x: f.f) x = -x;
        f.resize(n), wt.resize(n);
        reverse(vall(wt.f));
        fps<M>&P = wt, &Q = f;
        P.resize(4 * n), Q.resize(4 * n);

        fps<M> W(n);
        {
            // bit reverse order
            list<int> btr(n);
            int log = 31-__builtin_clz(n);
            for (int i = 1; i < n; i++) { btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (log - 1)); }
            int t = log_max_length;
            mint<M> r = last_powroot;
            mint<M> dw = r.inv().pow((1 << t) / (2 * n));
            mint<M> w = 1;
            for (auto i: btr) { W[i] = w, w *= dw; }
        }

        int k = 1;
        while (n > 1) {
            auto normalized_IDFT = [&](fps<M>& A) {
                inplaceIDFT(A);
                mint<M> iz = mint<M>(A.size()).inv();
                for (auto& x: A.f) x *= iz;
            };
            auto doubling_y = [&](fps<M>& A, int l, int r) {
                mint<M> z = W[k / 2].inv();
                fps<M> t(k);
                for (int i = l; i < r; i++) {
                    for (int j = 0; j < k; j++) t[j] = A[2 * n * j + i];
                    normalized_IDFT(t);
                    mint<M> x = 1;
                    for (int j = 0; j < k; j++) t[j] *= x, x *= z;
                    inplaceDFT(t);
                    for (int j = 0; j < k; j++) A[2 * n * (k + j) + i] = t[j];
                }
            };
            auto FFT_x = [&](fps<M>& A, int l, int r) {
                fps<M> t(2 * n);
                for (int j = l; j < r; j++) {
                    copy(A.f.begin() + 2 * n * j, A.f.begin() + 2 * n * (j + 1), t.f.begin());
                    inplaceDFT(t);
                    copy(vall(t.f), A.f.begin() + 2 * n * j);
                }
            };

            if (n <= k) {
                doubling_y(P, 0, n), doubling_y(Q, 1, n);
                FFT_x(P, 0, 2 * k), FFT_x(Q, 0, 2 * k);
            } else {
                FFT_x(P, 0, k), FFT_x(Q, 0, k);
                doubling_y(P, 0, 2 * n), doubling_y(Q, 0, 2 * n);
            }
            for (int i = 0; i < 2 * n * k; i++) Q[i] += 1;
            for (int i = 2 * n * k; i < 4 * n * k; i++) Q[i] -= 1;

            fps<M> F(2 * n), G(2 * n), p(n), q(n);
            for (int j = 0; j < 2 * k; j++) {
                copy(P.f.begin() + 2 * n * j, P.f.begin() + 2 * n * (j + 1), F.f.begin());
                copy(Q.f.begin() + 2 * n * j, Q.f.begin() + 2 * n * (j + 1), G.f.begin());
                for (int i = 0; i < n; i++) {
                    p[i] = W[i] * (F[2 * i] * G[2 * i + 1] - F[2 * i + 1] * G[2 * i]);
                    q[i] = G[2 * i] * G[2 * i + 1];
                }
                normalized_IDFT(p), normalized_IDFT(q);
                fill(p.f.begin() + n / 2, p.f.end(), 0);
                fill(q.f.begin() + n / 2, q.f.end(), 0);
                copy(vall(p.f), P.f.begin() + n * j);
                copy(vall(q.f), Q.f.begin() + n * j);
            }
            fill(P.f.begin() + 2 * n * k, P.f.end(), 0);
            fill(Q.f.begin() + 2 * n * k, Q.f.end(), 0);
            for (int j = 0; j < 4 * k; j++) Q[n * j] = 0;
            n /= 2, k *= 2;
        }
        for (int i = 0; i < k; i++) P[i] = P[2 * i];
        P.resize(k);
        mint<M> ik = mint<M>(k).inv();
        for (auto& x: P.f) x *= ik;
        inplaceIDFT(P);
        for (auto& x: P.f) x *= ik;
        reverse(vall(P.f));
        P.resize(m + 1);
        return P;
    }
    fps<M> compositional_inverse(fps<M> f, mod_table<M>& mtable) const {
        /*
        仮定：[x^1]f = c は 0 ではない.
        f を改めて cf と書く. f の逆関数を g とすれば
        g(cf(x)/c)=x なので g(x/c) が求めるもの．よって [x^1]f=1 に帰着.

        f, g は n 次とする. Lagrange inversion formula
        n[x^n]f(x)^i = [x^{n-i}] i(g(x)/x)^{-n}
        左辺を i=1,...,n で求めると (g(x)/x)^{-n} が n-1 次まで求まる.
        [x^1]g=1 に帰着していたので -1/n 乗根がとれて, g(x)/x が n-1 次まで求まる.
        */
        const int n = len(f) - 1;
        if (n == -1) return {};
        assert(f[0] == 0);
        if (n == 0) return f;
        assert(f[1] != 0);
        mint<M> c = f[1];
        mint<M> ic = c.inv();
        for (auto& x: f.f) x *= ic;
        fps<M> wt(n + 1);
        wt[n] = 1;

        fps<M> A = power_projection(f, wt, n, mtable);
        fps<M> g(n);
        for (int i = 1; i < n + 1; i++) g[n - i] = mint<M>(n) * A[i] * mint<M>(i).inv();
        g = pow(g, M - mint<M>(n).inv().val, len(g), mtable);
        g.f.insert(g.f.begin(), 0);
        g.sz++;

        mint<M> pow = 1;
        for (int i = 0; i < len(g); i++) g[i] *= pow, pow *= ic;
        return g;
    }
    // f(x+a)を求める
    fps<M> polynominal_taylor_shift(const fps<M>& F, ll a, const mod_table<M>& mtable){
        const int n = F.size();
        const mint<M> shift = a;
        if (n == 0 || shift == 0) return F;
        fps<M> g(n), h(n);
        mint<M> power = 1;
        for (int i = 0; i < n; ++i) {
            g[n-1-i] = F[i]*mtable.factorialmod(i);
            h[i] = power*mtable.factorialmodinv(i);
            power *= shift;
        }
        g = convolution(std::move(g),std::move(h));
        fps<M> result(n);
        for (int i = 0; i < n; ++i) result[i] = g[n-1-i]*mtable.factorialmodinv(i);
        return result;
    }
    // fの微分を求める。
    fps<M> differential(const fps<M>& F) const {
        fps<M> G = F;
        G.differential();
        return G;
    }
    // fの積分を求める。積分定数は0
    fps<M> integral(const fps<M>& F, const mod_table<M>& mtable) const {
        fps<M> G = F;
        G.integral(mtable);
        return G;
    }
};



/// @brief `[x^N].val(P(x)/Q(x))` をmod Mで求める
template<uint M>
constant_modint<M> Bostan_Mori(const ll N, FormalPowerSeries<M> P, FormalPowerSeries<M> Q, const fps_operator<M>& op){
    assert(N >= 0);
    if (N == 0){
        return P[0].val*inverse_mod<ll,ll>(Q[0].val, M)%M;
    }
    FormalPowerSeries<M> Q_minus;
    const int maxloop = (N == 1 ? 1 : 65-__builtin_clzll(N-1));
    for (int _i_ = 0; _i_ < maxloop; _i_++){
        Q_minus.resize(Q.size());
        for (size_t i = 0; i < Q_minus.size(); i += 2){
            Q_minus[i] = Q[i];
        }
        for (size_t i = 1; i < Q_minus.size(); i += 2){
            Q_minus[i].val = (M-1)*Q[i].val%M;
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
    return P[0].val*inverse_mod<ll,ll>(Q[0].val, M)%M;
}

template<uint M>
list<FormalPowerSeries<M>> multipoint_product_tree(const list<constant_modint<M>>& samples, const fps_operator<M>& op){
    const int n = len(samples);
    if (n == 0) return {};

    list<FormalPowerSeries<M>> tree(2*n-1);
    int next_id = 0;
    auto build = [&](auto&& self, int l, int r) -> int {
        const int node = next_id++;
        if (r-l == 1){
            tree[node] = FormalPowerSeries<M>{-samples[l], constant_modint<M>(1)};
            return node;
        }

        const int mid = (l+r)/2;
        const int left = self(self, l, mid);
        const int right = self(self, mid, r);
        if (r-l == 2){
            const auto x0 = samples[l], x1 = samples[l+1];
            tree[node] = FormalPowerSeries<M>{x0*x1, -(x0+x1), constant_modint<M>(1)};
        }
        else if (r-l == 3){
            const auto x0 = samples[l], x1 = samples[l+1], x2 = samples[l+2];
            tree[node] = FormalPowerSeries<M>{
                -x0*x1*x2,
                x0*x1+x0*x2+x1*x2,
                -(x0+x1+x2),
                constant_modint<M>(1)
            };
        }
        else{
            tree[node] = op.convolution(tree[left], tree[right]);
        }
        return node;
    };
    build(build, 0, n);
    return tree;
}

template<uint M>
list<constant_modint<M>> multipoint_evaluation_with_product_tree(const FormalPowerSeries<M>& F, const list<constant_modint<M>>& samples, const list<FormalPowerSeries<M>>& tree, const fps_operator<M>& op){
    const int n = len(samples);
    if (n == 0) return {};

    list<constant_modint<M>> ans(n);
    list<FormalPowerSeries<M>> remainders;
    remainders.reserve(n);
    list<array<int, 4>> stack;
    stack.reserve(64);

    remainders.append(F);
    stack.append({0,n,0,0});

    while (!stack.empty()){
        auto [l,r,node,remainder_index] = stack.pop();


        if (l >= r){continue;}
        if (r-l == 1){
            constant_modint<M> value = 0;
            for (int i = int(remainders[remainder_index].sz)-1; i >= 0; i--){
                value = value*samples[l]+remainders[remainder_index][i];
            }
            ans[l] = value;
            continue;
        }

        if (remainders[remainder_index].size() >= tree[node].size()){
            remainders.append(op.remainder(remainders[remainder_index], tree[node]));
            remainder_index = int(remainders.size())-1;
        }
        const int mid = (l+r)/2;
        const int left = node+1;
        const int right = node+2*(mid-l);
        stack.append({mid,r,right,remainder_index});
        stack.append({l,mid,left,remainder_index});
    }

    return ans;
}

template <uint M>
list<constant_modint<M>> multipoint_evaluation(const FormalPowerSeries<M>& F, const list<constant_modint<M>>& samples, const fps_operator<M>& op){
    auto tree = multipoint_product_tree(samples, op);
    return multipoint_evaluation_with_product_tree(F, samples, tree, op);
}

template<uint M>
FormalPowerSeries<M> product_of_polynomial_sequence(const list<FormalPowerSeries<M>>& A, const fps_operator<M>& op){
    delist<FormalPowerSeries<M>> dq;
    dq.append(FormalPowerSeries<998244353>{1});
    for (auto& f : A){
        dq.append(std::move(f));
    }
    while (dq.size() > 1){
        dq.append(op.convolution(std::move(*dq.begin()), std::move(*++dq.begin())));
        dq.popleft();
        dq.popleft();
    }
    return dq.front();
}

template<uint M>
FormalPowerSeries<M> polynomial_interpolation(const list<constant_modint<M>>& X, const list<constant_modint<M>>& Y, const fps_operator<M>& op){
    assert(len(X) == len(Y));
    const int N = len(X);
    if (N == 0) return {};

    const auto tree = multipoint_product_tree(X, op);
    auto product_derivative = op.differential(tree[0]);
    list<constant_modint<M>> Z =
        multipoint_evaluation_with_product_tree(product_derivative, X, tree, op);

    list<constant_modint<M>> prefix_product(N+1);
    prefix_product[0] = 1;
    for (int i = 0; i < N; i++){
        prefix_product[i+1] = prefix_product[i]*Z[i];
    }
    constant_modint<M> inverse_suffix_product = prefix_product[N].inv();
    for (int i = N-1; i >= 0; i--){
        const auto denominator = Z[i];
        Z[i] = Y[i]*prefix_product[i]*inverse_suffix_product;
        inverse_suffix_product *= denominator;
    }

    auto interpolate = [&](auto&& self, int l, int r, int node) -> FormalPowerSeries<M> {
        if (r-l == 1){
            return FormalPowerSeries<M>{Z[l]};
        }
        if (r-l == 2){
            const auto x0 = X[l], x1 = X[l+1];
            const auto z0 = Z[l], z1 = Z[l+1];
            return FormalPowerSeries<M>{-(z0*x1+z1*x0), z0+z1};
        }
        if (r-l == 3){
            const auto x0 = X[l], x1 = X[l+1], x2 = X[l+2];
            const auto z0 = Z[l], z1 = Z[l+1], z2 = Z[l+2];
            return FormalPowerSeries<M>{
                z0*x1*x2+z1*x0*x2+z2*x0*x1,
                -(z0*(x1+x2)+z1*(x0+x2)+z2*(x0+x1)),
                z0+z1+z2
            };
        }

        const int mid = (l+r)/2;
        const int left = node+1;
        const int right = node+2*(mid-l);
        auto left_interpolation = self(self, l, mid, left);
        auto right_interpolation = self(self, mid, r, right);
        return op.add(
            op.convolution(std::move(left_interpolation), tree[right]),
            op.convolution(tree[left], std::move(right_interpolation))
        );
    };
    return interpolate(interpolate, 0, N, 0);
}

#undef vall
#endif /* FPS_OPERATION_HPP_ */
