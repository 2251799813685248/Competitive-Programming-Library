#include <iostream>
#include <list.hpp>
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
#include <unistd.h>
#include <fstream>
#include <chrono>


using namespace std;
using ll = long long;
using lll = __int128_t;
using ull = unsigned long long;
using ld = long double;
using pii = array<int,2>;
using pll = array<ll,2>;
using plll = array<lll,2>;

#define vall(A) A.begin(), A.end()
ostream& operator<<(ostream& os, const pll& p) {os << p[0] << " " << p[1]; return os;}
                                    inline void print(){cout << "\n";}
                                    inline void printflush(){cout << endl;}
template<typename T, typename... U> inline void print(const T& obj1, const U&... obj2){cout << (obj1) << " "; print(obj2...);}
template<typename T, typename... U> inline void printflush(const T& obj1, const U&... obj2){cout << (obj1) << " "; printflush(obj2...);}
template<typename T> inline void vin(T& A){for (int i = 0, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void vout(const T& A){if (A.size() == 0ull){print();} for (int i = 0, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void vout2d(const T& A){if (A.size() == 0ull){print();} for (int i = 0, H = A.size(); i < H; i++){vout(A[i]);}}
template<typename T> inline void adjvin(T& A){for (int i = 1, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void adjvout(const T& A){for (int i = 1, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void adjvout2d(const T& A){if (A.size() == 0ull){print();} for (int i = 1, H = A.size(); i < H; i++){adjvout(A[i]);}}
template<typename T> inline bool btest(T K, int i){return K&(1ull<<i);}
constexpr ll pow2ll[63] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904};
constexpr ll pow10ll[19] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000};
constexpr ll di[4] = {0,1,0,-1};
constexpr ll di8[8] = {0,1,1,1,0,-1,-1,-1};
constexpr ll dj[4] = {1,0,-1,0};
constexpr ll dj8[8] = {1,1,0,-1,-1,-1,0,1};


#include <fps.hpp>
#include <fps_operations.hpp>

#include <tree.hpp>


fps_operator<998244353> op;
fps_operator<167772161> op2;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ll N;
    cin >> N;
    list<list<ll>> E(N);
    for (ll i = 0; i < N-1; i++){
        ll u,v;
        cin >> u >> v;
        E[u].append(v);
        E[v].append(u);
    }
    
    list<constant_modint<998244353>> ans(N+1);
    list<constant_modint<167772161>> ans2(N+1);

    list<pair<pair<int, int>, list<list<ll>>>> st;
    st.append({{N, 0}, E});

    while (!st.empty()){
        auto T = st.pop();
        if (T.first.first == 1){continue;}
        int centroid = tree_centroid(T.first.first, T.second);
        auto decomposed = tree_decomposition(T.first.first, T.second, centroid);

        delist<ll> dq;

        list<list<ll>> U;
        ull maxsize = 0;

        for (auto& subT : decomposed){
            st.append(subT);
            list<ll> F{0, 1};
            dq.append(subT.first.second);
            list<ll> dist(subT.second.size(), -1);
            dist[subT.first.second] = 1;
            ans[1] += 1;
            ans2[1] += 1;
            while (!dq.empty()){
                ll n = dq.pop();
                for (auto w : subT.second[n]){
                    if (dist[w] >= 0){continue;}
                    dist[w] = dist[n]+1;
                    dq.append(w);
                    F.resize(max<ull>(F.size(), dist[w]+1), 0);
                    ans[dist[w]] += 1;
                    ans2[dist[w]] += 1;
                    F[dist[w]]++;
                }
            }
            maxsize = max<ull>(maxsize, F.size());
            U.append(F);
        }

        FormalPowerSeries<998244353> S1(maxsize, 0);
        FormalPowerSeries<167772161> S2(maxsize, 0);
        for (auto& f : U){
            for (uint i = 0; i < f.size(); i++){
                S1[i] += f[i];
                S2[i] += f[i];
            }
        }
        S1 = op.convolution(S1,S1);
        S2 = op2.convolution(S2,S2);
        for (auto& f : U){
            auto g = op.convolution(f,f);
            auto g2 = op2.convolution(f,f);
            for (uint i = 0; i < g.size(); i++){
                S1[i] += 998244353-g[i];
            }
            for (uint i = 0; i < g.size(); i++){
                S2[i] += 167772161-g2[i];
            }
        }
        for (uint i = 0; i < S1.size(); i++){
            ans[i] += S1[i]/2;
            ans2[i] += S2[i]/2;
        }
    }
    list<ll> ans3(N);
    auto temp = axby1<lll>(998244353, 167772161);
    temp[1] += 998244353ll*167772161;
    for (ll i = 0; i < N; i++){
        ans3[i] = (ans[i].val*167772161ll*temp[1]+ans2[i].val*998244353ll*temp[0])%(998244353ll*167772161);
    }
    adjvout(ans3);
}