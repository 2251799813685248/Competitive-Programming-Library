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



#include <modint.hpp>
using mint = constant_modint<998244353>;

#include <matrix.hpp>



void solve(){

    
    ll N;
    cin >> N;
    matrix<mint> A(N);

    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            cin >> A[i][j];
        }
    }

    auto B = A;
    B.transpose();

    mint det = det_mod_P(A);

    auto C = row_simplification(A);

    int rank = -1;
    for (int i = N-1; i >= 0; i--){
        for (int j = 0; j < N; j++){
            if (C[i][j] != 0){
                rank = i;
                break;
            }
        }
        if (rank != -1){break;}
    }
    rank++;

    if (rank == N){
        auto inv = inverse_matrix(A).second*det;
        vout2d(inv);
        return;
    }
    else if(rank < N-1){
        vout2d(matrix<int>(N,N,0));
    }
    else{
        for (int i = 0; i < N; i++){A[i].push_back(0);B[i].push_back(0);}
        A.W++;
        B.W++;
        auto x = solve_linear_equations(A)[1];
        auto y = solve_linear_equations(B)[1];
        pii non_zero{0,0};
        A.W--;
        B.W--;
        for (int i = 0; i < N; i++){A[i].pop_back();B[i].pop_back();}
        for (int i = 0; i < N; i++){
            if (C[i][i] == 0){
                non_zero[1] = i;
                break;
            }
        }
        C = row_simplification(B);
        for (int i = 0; i < N; i++){
            if (C[i][i] == 0){
                non_zero[0] = i;
                break;
            }
        }
        C = matrix<mint>(N,N,0);
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                C[i][j] = x[i]*y[j];
            }
        }

        mint k = 1;

        for (int i = 0; i < N; i++){
            A[i].erase(A[i].begin()+non_zero[1]);
        }
        A.M.erase(A.M.begin()+non_zero[0]);
        A.H--;
        A.W--;

        k = mint(-1).pow(non_zero[0]+non_zero[1])*det_mod_P(A)/C[non_zero[1]][non_zero[0]];
        vout2d(C*k);

    }





    
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



