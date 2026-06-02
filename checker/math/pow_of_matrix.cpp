#include <iostream>
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
#include <unistd.h>


using namespace std;
using ll = long long;
using lll = __int128_t;
using ull = unsigned long long;
using ld = long double;
using pii = array<int,2>;
using pll = array<ll,2>;
using plll = array<lll,2>;

#define vall(A) A.begin(), A.end()
template<typename T> inline void vin(T& A){for (int i = 0, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void vout(const T& A){for (int i = 0, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void vout2d(const T& A){for (int i = 0, H = A.size(); i < H; i++){vout(A[i]);}}
template<typename T> inline void adjvin(T& A){for (int i = 1, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void adjvout(const T& A){for (int i = 1, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void adjvout2d(const T& A){for (int i = 1, H = A.size(); i < H; i++){adjvout(A[i]);}}
template<typename T> inline bool btest(T K, int i){return K&(1ull<<i);}
template<typename T> void print(T object){cout << (object) << "\n";}
template<typename T, typename U> void print(T object1, U object2){cout << (object1) << " " << (object2) << "\n";}
template<typename T, typename U, typename V> void print(T object1, U object2, V object3){cout << (object1) << " " << (object2) << " " << (object3) << "\n";}
template<typename T, typename U, typename V, typename W> void print(T object1, U object2, V object3, W object4){cout << (object1) << " " << (object2) << " " << (object3) << " " << (object4) << "\n";}
const vector<ull> pow2ll{1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904, 9223372036854775808ull};
const vector<ull> pow10ll{1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000, 10000000000000000000ull};
const vector<ll> di{0,1,0,-1};
const vector<ll> di8{0,1,1,1,0,-1,-1,-1};
const vector<ll> dj{1,0,-1,0};
const vector<ll> dj8{1,1,0,-1,-1,-1,0,1};


#include <matrix.hpp>
#include <modint.hpp>


int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    ll K;
    cin >> N >> K;
    matrix<constant_modint<998244353>> A(N, N, 0);
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            cin >> A[i][j];
        }
    }
    auto ans = matrixpow(A, K);
    vout2d(ans);
}