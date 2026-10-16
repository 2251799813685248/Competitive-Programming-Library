// #define _GLIBCXX_DEBUG

#include <iostream>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <functional>
#include <random>
#include <bitset>

// #include <boost/multiprecision/cpp_int.hpp>



using namespace std;
using ll = long long;
using lll = __int128_t;
using ull = unsigned long long;
#ifdef BOOST_VERSION
using bll = boost::multiprecision::cpp_int;
#endif
using ld = long double;
using pii = array<int,2>;
using pll = array<ll,2>;
using plll = array<lll,2>;
using pld = array<ld, 2>;
using tll = array<ll,3>;

#define vall(A) A.begin(), A.end()
template<typename T> ostream& operator<<(ostream& os, const array<T, 2>& p) {os << p[0] << " " << p[1]; return os;}
                                    inline void print(){cout << "\n";}
                                    inline void printflush(){cout << endl;}
template<typename T, typename... U> inline void print(const T& obj1, const U&... obj2){cout << (obj1) << (sizeof...(U) == 0 ? "" : " "); print(obj2...);}
template<typename T, typename... U> inline void printflush(const T& obj1, const U&... obj2){cout << (obj1) << (sizeof...(U) == 0 ? "" : " "); printflush(obj2...);}
template<typename T> inline void vin(T& A){for (int i = 0, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void vin(T& A, int begin, int end){for (int i = begin, sz = end; i < sz; i++){cin >> A[i];}}
template<typename T> inline void vout(const T& A){if (A.size() == 0ull){print();} for (int i = 0, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void vout(const T& A, int begin, int end){if (A.size() == 0ull){print();} for (int i = begin; i < end; i++){cout << A[i] << " \n"[i == end-1];}}
template<typename T> inline void vout2d(const T& A){if (A.size() == 0ull){print();} for (int i = 0, H = A.size(); i < H; i++){vout(A[i]);}}
template<typename T> inline void vout2d(const T& A, int r1, int c1, int r2, int c2){for (int i = r1; i < r2; i++){vout(A[i], c1, c2);}}
template<typename T> inline bool btest(T K, int i){return K&(1ull<<i);}
#ifndef LEN_DEFINED
#define LEN_DEFINED
template<typename T> inline constexpr ll len(const T& A){return ssize(A);}
#endif
constexpr ll pow2ll[63] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904};
constexpr ll pow10ll[19] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000};
constexpr ll di[4] = {0,1,0,-1};
constexpr ll di8[8] = {0,1,1,1,0,-1,-1,-1};
constexpr ll dj[4] = {1,0,-1,0};
constexpr ll dj8[8] = {1,1,0,-1,-1,-1,0,1};


#include <fps_operations.hpp>
#include <fastio.hpp>

using mint = constant_modint<998244353>;
fps_operator<998244353> op;



void solve(){

    int N,M;
    fastio::read(N,M);
    // cin >> N >> M;

    vector<mint> F(N), G(M);

    fastio::read(F,G);
    // vin(F); vin(G);

    array<FormalPowerSeries<998244353>,4> FF{FormalPowerSeries<998244353>{},{},{},{}};
    array<FormalPowerSeries<998244353>,4> GG{FormalPowerSeries<998244353>{},{},{},{}};

    auto clamp2 = [](ll a, ll b, ll c){
        return max(b, min(a,c));
    };

    FF[0].resize(clamp2(4194304ll, 0ll, N));
    for (ll i = 0; i < 4194304; i++){
        if (i >= N){break;}
        FF[0][i] = F[i];
    }
    FF[1].resize(clamp2(4194304ll, 0ll, N-4194304));
    for (ll i = 4194304; i < 2*4194304; i++){
        if (i >= N){break;}
        FF[1][i-4194304] = F[i];
    }
    FF[2].resize(clamp2(4194304ll, 0ll, N-2*4194304));
    for (ll i = 2*4194304; i < 3*4194304; i++){
        if (i >= N){break;}
        FF[2][i-2*4194304] = F[i];
    }
    FF[3].resize(clamp2(4194304ll, 0ll, N-3*4194304));
    for (ll i = 3*4194304; i < 4*4194304; i++){
        if (i >= N){break;}
        FF[3][i-3*4194304] = F[i];
    }

    GG[0].resize(clamp2(4194304ll, 0ll, M));
    for (ll i = 0; i < 4194304; i++){
        if (i >= M){break;}
        GG[0][i] = G[i];
    }
    GG[1].resize(clamp2(4194304ll, 0ll, M-4194304));
    for (ll i = 4194304; i < 2*4194304; i++){
        if (i >= M){break;}
        GG[1][i-4194304] = G[i];
    }
    GG[2].resize(clamp2(4194304ll, 0ll, M-2*4194304));
    for (ll i = 2*4194304; i < 3*4194304; i++){
        if (i >= M){break;}
        GG[2][i-2*4194304] = G[i];
    }
    GG[3].resize(clamp2(4194304ll, 0ll, M-3*4194304));
    for (ll i = 3*4194304; i < 4*4194304; i++){
        if (i >= M){break;}
        GG[3][i-3*4194304] = G[i];
    }

    FormalPowerSeries<998244353> A2,B2,C2,D2,E2,F2,G2;

    A2 = op.convolution(FF[0], GG[0]);
    B2 = op.add(op.convolution(FF[0], GG[1]), op.convolution(FF[1], GG[0]));
    C2 = op.add(op.add(op.convolution(FF[0], GG[2]), op.convolution(FF[1], GG[1])), op.convolution(FF[2], GG[0]));
    D2 = op.add(op.add(op.add(op.convolution(FF[0], GG[3]), op.convolution(FF[1], GG[2])), op.convolution(FF[2], GG[1])), op.convolution(FF[3], GG[0]));
    FF[0].f = list<mint>();
    GG[0].f = list<mint>();
    E2 = op.add(op.add(op.convolution(FF[1], GG[3]), op.convolution(FF[2], GG[2])), op.convolution(FF[3], GG[1]));
    FF[1].f = list<mint>();
    GG[1].f = list<mint>();
    F2 = op.add(op.convolution(FF[2], GG[3]), op.convolution(FF[3], GG[2]));
    FF[2].f = list<mint>();
    GG[2].f = list<mint>();
    G2 = op.convolution(FF[3], GG[3]);
    FF[3].f = list<mint>();
    GG[3].f = list<mint>();
    

    FormalPowerSeries<998244353> ans(33554432);

    for (uint i = 0; i < A2.sz; i++){
        ans[i] += A2[i];
    }
    for (uint i = 0; i < B2.sz; i++){
        ans[4194304+i] += B2[i];
    }
    for (uint i = 0; i < C2.sz; i++){
        ans[2*4194304+i] += C2[i];
    }
    for (uint i = 0; i < D2.sz; i++){
        ans[3*4194304+i] += D2[i];
    }
    for (uint i = 0; i < E2.sz; i++){
        ans[4*4194304+i] += E2[i];
    }
    for (uint i = 0; i < F2.sz; i++){
        ans[5*4194304+i] += F2[i];
    }
    for (uint i = 0; i < G2.sz; i++){
        ans[6*4194304+i] += G2[i];
    }

    ans.resize(F.size()+G.size()-1);
    fastio::print(ans);

}



int main(){

    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ll T = 1;
    // cin >> T;
    while (T--){
        solve();
    }
}



