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
#include <variant>


using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = array<int,2>;
using pll = array<ll,2>;

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

#include <boost/multiprecision/cpp_int.hpp>

using BigInt = boost::multiprecision::cpp_int;

class PythonicInt {
private:
    // ll か BigInt のどちらかを保持する
    variant<ll, BigInt> value;

    // ヘルパー関数: 現在の値を必ず BigInt として取得する
    inline BigInt to_bigint() const {
        if (std::holds_alternative<ll>(value)) {
            return BigInt(std::get<ll>(value));
        }
        return std::get<BigInt>(value);
    }

public:
    // コンストラクタ
    PythonicInt() : value(0ll) {}
    PythonicInt(ll v) : value(v) {}
    PythonicInt(const BigInt& v) : value(v) {}

    PythonicInt operator+=(const PythonicInt& other){
        if (std::holds_alternative<ll>(value) && std::holds_alternative<ll>(other.value)) {
            ll a = std::get<ll>(value);
            ll b = std::get<ll>(other.value);
            long long result;
            if (__builtin_saddll_overflow(a, b, &result)){
                value = BigInt(a) + BigInt(b);
                return *this;
            }
            value = result;
            return *this;
        }
        value = this->to_bigint() + other.to_bigint();
        return *this;
    }
    PythonicInt operator-=(const PythonicInt& other){
        if (std::holds_alternative<ll>(value) && std::holds_alternative<ll>(other.value)) {
            ll a = std::get<ll>(value);
            ll b = -std::get<ll>(other.value);
            long long result;
            if (__builtin_saddll_overflow(a, b, &result)){
                value = BigInt(a) - BigInt(b);
                return *this;
            }
            value = result;
            return *this;
        }
        value = this->to_bigint() - other.to_bigint();
        return *this;
    }
    PythonicInt operator*=(const PythonicInt& other){
        if (std::holds_alternative<ll>(value) && std::holds_alternative<ll>(other.value)) {
            ll a = std::get<ll>(value);
            ll b = std::get<ll>(other.value);
            long long result;
            if (__builtin_smulll_overflow(a, b, &result)){
                value = BigInt(a)*BigInt(b);
                return *this;
            }
            value = result;
            return *this;
        }
        value = this->to_bigint() * other.to_bigint();
        return *this;
    }
    PythonicInt operator/=(const PythonicInt& other){
        if (std::holds_alternative<ll>(value) && std::holds_alternative<ll>(other.value)) {
            std::get<ll>(value) /= std::get<ll>(other.value);
            return *this;
        }
        value = this->to_bigint() / other.to_bigint();
        return *this;
    }
    PythonicInt operator%=(const PythonicInt& other){
        if (std::holds_alternative<ll>(value) && std::holds_alternative<ll>(other.value)) {
            std::get<ll>(value) %= std::get<ll>(other.value);
            return *this;
        }
        value = this->to_bigint() % other.to_bigint();
        return *this;
    }

    friend PythonicInt operator+(const PythonicInt& lhs, const PythonicInt& rhs) { return PythonicInt(lhs) += rhs; }
    friend PythonicInt operator-(const PythonicInt& lhs, const PythonicInt& rhs) { return PythonicInt(lhs) -= rhs; }
    friend PythonicInt operator*(const PythonicInt& lhs, const PythonicInt& rhs) { return PythonicInt(lhs) *= rhs; }
    friend PythonicInt operator/(const PythonicInt& lhs, const PythonicInt& rhs) { return PythonicInt(lhs) /= rhs; }
    friend PythonicInt operator%(const PythonicInt& lhs, const PythonicInt& rhs) { return PythonicInt(lhs) %= rhs; }

    friend bool operator==(const PythonicInt& lhs, const PythonicInt& rhs) { return lhs.value == rhs.value; }
    friend bool operator!=(const PythonicInt& lhs, const PythonicInt& rhs) { return lhs.value != rhs.value; }

    PythonicInt operator+() const { return *this; }
    PythonicInt operator-() const { return PythonicInt(0) - *this; }

    PythonicInt operator++(int){ PythonicInt temp = *this; *this += 1; return temp;}
    PythonicInt operator--(int){ PythonicInt temp = *this; *this -= 1; return temp;}
    PythonicInt operator++(){ return *this += 1;}
    PythonicInt operator--(){ return *this -= 1;}

    operator bool() const{
        if (holds_alternative<ll>(value)){
            return get<ll>(value) != 0;
        }
        return get<BigInt>(value) != 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const PythonicInt& m) {
        if (holds_alternative<ll>(m.value)){
            return os << get<ll>(m.value);
        }
        return os << get<BigInt>(m.value);
    }
    friend std::istream& operator>>(std::istream& is, PythonicInt& m) {
        ll x;
        is >> x;
        m = PythonicInt(x);
        return is;
    }
    void input_BigInt(){
        string S;
        cin >> S;
        value = BigInt(S);
    }

};
PythonicInt operator"" _lll(const char* raw_int){
    if (strlen(raw_int) <= 18){
        return PythonicInt(stoll(raw_int));
    }
    else if (strlen(raw_int) == 19 && string(raw_int) < "9223372036854775808"){
        return PythonicInt(stoll(raw_int));
    }
    return PythonicInt(BigInt(raw_int));
}


#define ll PythonicInt


void solve(){

    ll a,b;
    a.input_BigInt();
    b.input_BigInt();
    print(a+b);


}


int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);


    ll T = 1;
    cin >> T;
    while (T--){
        solve();
    }
}