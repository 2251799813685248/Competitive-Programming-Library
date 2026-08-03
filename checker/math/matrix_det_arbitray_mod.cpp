#include <iostream>
#include <iterator>
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
template<typename T> inline constexpr ll len(const T& A){return ssize(A);}
#endif
constexpr ll pow2ll[63] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904};
constexpr ll pow10ll[19] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000};
constexpr ll di[4] = {0,1,0,-1};
constexpr ll di8[8] = {0,1,1,1,0,-1,-1,-1};
constexpr ll dj[4] = {1,0,-1,0};
constexpr ll dj8[8] = {1,1,0,-1,-1,-1,0,1};


#include <math_functions.hpp>
#include <matrix.hpp>
#include <prime_and_divisors.hpp>
#include <CRT.hpp>

/// 行列式をmod P^nで求める。
ll det_mod_p_n(matrix<ll> A, ll P, ll n){
    ll N = A.size();
    ll W = A.empty() ? 0 : A[0].size();
    assert(N == W);
    ll Pn = powll(P,n);
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            A[i][j] %= Pn;
        }
    }
    int swap_count = 0;
    int non_zero_column = 0;
    for (int r = 0; r < N; r++){
        bool finished = false;
        while (true){
            if (non_zero_column == N){break;}
            for (int r2 = r; r2 < N; r2++){
                if (A[r2][non_zero_column] != 0){
                    ll min_vp = 9223372036854775807;
                    ll min_vp_idx = -1;
                    for (int r3 = r; r3 < N; r3++){
                        if (A[r3][non_zero_column] == 0){continue;}
                        ll temp = A[r3][non_zero_column];
                        ll vp = 0;
                        while (temp%P == 0){
                            vp++;
                            temp /= P;
                        }
                        if (min_vp > vp){
                            min_vp = vp;
                            min_vp_idx = r3;
                        }
                    }
                    if (r != min_vp_idx){swap_count++;}
                    swap(A[r], A[min_vp_idx]);
                    ll Pa = gcd(A[r][non_zero_column], Pn);
                    for (int r3 = r+1; r3 < N; r3++){
                        ll k = inverse_mod(A[r][non_zero_column]/Pa, Pn/Pa)*((Pn-A[r3][non_zero_column])/Pa)%(Pn/Pa);
                        for (int c = non_zero_column; c < N; c++){
                            A[r3][c] += k*A[r][c]%Pn;
                            if (A[r3][c] >= Pn){A[r3][c] -= Pn;}
                        }
                    }
                    finished = true; break;
                }
            }
            non_zero_column++;
            if (finished){break;}
        }
        if (non_zero_column == N){break;}
    }
    ll det = 1;
    for (int i = 0; i < N; i++){
        det = (det*A[i][i])%Pn;
    }
    if ((swap_count&1) && det != 0){
        det = Pn-det;
    }
    return det;
}


void solve(){

    ll N,M;
    cin >> N >> M;

    matrix<ll> A(N);
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            cin >> A[i][j];
        }
    }
    if (M == 1){
        print(0);
        return;
    }
    
    auto fact = factorize(M);

    vector<ll> r,m;

    for (auto& p : fact){
        m.push_back(powll(p[0], p[1]));
        r.push_back(det_mod_p_n(A, p[0], p[1]));
    }
    print(crt_relatively_prime(r,m));
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



