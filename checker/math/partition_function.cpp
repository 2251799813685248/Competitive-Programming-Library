#include <iostream>
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

#define vall(A) A.begin(), A.end()
template<typename T> ostream& operator<<(ostream& os, const array<T, 2>& p) {os << p[0] << " " << p[1]; return os;}
                                    inline void print(){cout << "\n";}
                                    inline void printflush(){cout << endl;}
template<typename T, typename... U> inline void print(const T& obj1, const U&... obj2){cout << (obj1) << (sizeof...(U) == 0 ? "" : " "); print(obj2...);}
template<typename T, typename... U> inline void printflush(const T& obj1, const U&... obj2){cout << (obj1) << (sizeof...(U) == 0 ? "" : " "); printflush(obj2...);}
template<typename T> inline void vin(T& A){for (int i = 0, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void vout(const T& A){if (A.size() == 0ull){print();} for (int i = 0, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void vout2d(const T& A){if (A.size() == 0ull){print();} for (int i = 0, H = A.size(); i < H; i++){vout(A[i]);}}
template<typename T> inline void adjvin(T& A){for (int i = 1, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void adjvout(const T& A){for (int i = 1, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void adjvout2d(const T& A){if (A.size() == 0ull){print();} for (int i = 1, H = A.size(); i < H; i++){adjvout(A[i]);}}
template<typename T> inline bool btest(T K, int i){return K&(1ull<<i);}
#ifndef LEN_DEFINED
#define LEN_DEFINED
template<typename T> inline constexpr ll len(T A){return ssize(A);}
#endif
constexpr ll pow2ll[63] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904};
constexpr ll pow10ll[19] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000};
constexpr ll di[4] = {0,1,0,-1};
constexpr ll di8[8] = {0,1,1,1,0,-1,-1,-1};
constexpr ll dj[4] = {1,0,-1,0};
constexpr ll dj8[8] = {1,1,0,-1,-1,-1,0,1};


#include <fps.hpp>
#include <fps_operations.hpp>

fps_operator<998244353> op;


void solve(){

    deque<FormalPowerSeries> q;
    for (ll i = 0; i <= 500000; i++){
        q.push_back(FormalPowerSeries(i+1));
        q.back()[i] = 998244352;
        q.back()[0] = 1;
    }

    while (len(q) > 1){
        auto f = std::move(q.front());
        q.pop_front();
        auto g = std::move(q.front());
        q.pop_front();
        q.push_back(op.convolution(f, g));
        while (len(q.back()) > 500000){
            q.back().f.pop_back();
            q.back().sz--;
        }
    }

    q.front() = op.inv(q.front(), 500001);

    ll N;
    cin >> N;

    q.front().resize(N+1);

    vout(q.front());

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



