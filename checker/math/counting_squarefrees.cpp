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
                                    inline void print(){cout << "\n";}
                                    inline void printflush(){cout << endl;}
template<typename T, typename... U> inline void print(T obj1, U... obj2){cout << (obj1) << " "; print(obj2...);}
template<typename T, typename... U> inline void printflush(T obj1, U... obj2){cout << (obj1) << " "; printflush(obj2...);}
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

#include <math_functions.hpp>
#include <quotients.hpp>
#include <prime_and_divisors.hpp>

ll count_square_free_integers(ll N){
    assert(N>0);
    if (N == 1){
        return 1;
    }
    ll ans = 0;
    ll I = max<ll>(1, pow(N, 0.2));
    ll D = 0;
    ll D2 = 2000000000;
    while (D2-D > 1){
        lll mid = (D+D2)/2;
        if (mid*mid*(I+1) <= (lll)N){
            D = mid;
        }
        else{
            D2 = mid;
        }
    }
    LinearSieve L(D+10);
    auto mobius = L.enumerate_mobius(D+10);
    vector<int> mobius_cum = mobius;
    for (ll i = 2; i <= D+10; i++){
        mobius_cum[i] += mobius_cum[i-1];
    }
    for (ll i = 1; i <= D; i++){
        ans += N/(i*i)*mobius[i];
    }
    vector<int> M(I+2, 0);
    for (int i = I+1; i >= 1; i--){
        ll x = sqrt(N/i);
        while (x*x*i > N){
            x--;
        }
        while ((x+1)*(x+1)*i <= N){
            x++;
        }
        M[i] = 1;
        for (auto& p : enumerate_quotient(x,x)){
            if (p[0] == x){continue;}
            if (p[0] <= D+10){
                M[i] -= (p[2]-p[1]+1)*mobius_cum[p[0]];
            }
            else{
                M[i] -= M[p[1]*p[1]*i];
            }
        }
    }
    for (int i = 1; i <= I; i++){
        ans += i*(M[i]-M[i+1]);
    }
    return ans;
}


int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ll N;
    cin >> N;

    print(count_square_free_integers(N));

}