#ifndef MODINT__HPP_
#define MODINT__HPP_

#include <iostream>
#include <math_functions.hpp>

using ll = long long;
using uint = unsigned int;
using ull = unsigned long long;
using namespace std;

template <uint M>
struct constant_modint {
    uint val;
    constant_modint() : val(0) {}
    template <class T> constant_modint(T x) {
        ll y = (ll)(x % (ll)M);
        if (y < 0) y += M;
        val = y;
    }

    constant_modint& operator+=(const constant_modint& rhs) {
        val += rhs.val;
        if (val >= M) val -= M;
        return *this;
    }
    constant_modint& operator-=(const constant_modint& rhs) {
        if (val < rhs.val) val += M;
        val -= rhs.val;
        return *this;
    }
    constant_modint& operator*=(const constant_modint& rhs) {
        val = (ull)val * rhs.val % M;
        return *this;
    }
    constant_modint& operator/=(const constant_modint& rhs) {
        return *this *= rhs.inv();
    }

    constant_modint operator+() const { return *this; }
    constant_modint operator-() const { return constant_modint(0) - *this; }

    friend constant_modint operator+(const constant_modint& lhs, const constant_modint& rhs) { return constant_modint(lhs) += rhs; }
    friend constant_modint operator-(const constant_modint& lhs, const constant_modint& rhs) { return constant_modint(lhs) -= rhs; }
    friend constant_modint operator*(const constant_modint& lhs, const constant_modint& rhs) { return constant_modint(lhs) *= rhs; }
    friend constant_modint operator/(const constant_modint& lhs, const constant_modint& rhs) { return constant_modint(lhs) /= rhs; }

    friend bool operator==(const constant_modint& lhs, const constant_modint& rhs) { return lhs.val == rhs.val; }
    friend bool operator!=(const constant_modint& lhs, const constant_modint& rhs) { return lhs.val != rhs.val; }

    constant_modint pow(ull n) const {
        constant_modint res = 1, a = *this;
        while (n) {
            if (n&1) res *= a;
            a *= a;
            n >>= 1;
        }
        return res;
    }
    constant_modint inv() const {
        return inverse_mod((ll)val, M);
    }

    friend std::ostream& operator<<(std::ostream& os, const constant_modint& m) {
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, constant_modint& m) {
        ll x;
        is >> x;
        m = constant_modint(x);
        return is;
    }
};

template <int id = -1>
struct dynamic_modint {
    uint val;
    static uint& mod() {
        static uint M = 998244353;
        return M;
    }
    static void set_mod(uint m) {
        mod() = m;
    }

    dynamic_modint() : val(0) {}
    template <class T> dynamic_modint(T x) {
        ll y = x % (ll)mod();
        if (y < 0) y += mod();
        val = (uint)y;
    }

    dynamic_modint& operator+=(const dynamic_modint& rhs) {
        val += rhs.val;
        if (val >= mod()) val -= mod();
        return *this;
    }
    dynamic_modint& operator-=(const dynamic_modint& rhs) {
        if (val < rhs.val) val += mod();
        val -= rhs.val;
        return *this;
    }
    dynamic_modint& operator*=(const dynamic_modint& rhs) {
        val = (ull)val * rhs.val % mod();
        return *this;
    }
    dynamic_modint& operator/=(const dynamic_modint& rhs) {
        return *this *= rhs.inv();
    }

    dynamic_modint operator+() const { return *this; }
    dynamic_modint operator-() const { return dynamic_modint(0) - *this; }

    friend dynamic_modint operator+(const dynamic_modint& lhs, const dynamic_modint& rhs) { return dynamic_modint(lhs) += rhs; }
    friend dynamic_modint operator-(const dynamic_modint& lhs, const dynamic_modint& rhs) { return dynamic_modint(lhs) -= rhs; }
    friend dynamic_modint operator*(const dynamic_modint& lhs, const dynamic_modint& rhs) { return dynamic_modint(lhs) *= rhs; }
    friend dynamic_modint operator/(const dynamic_modint& lhs, const dynamic_modint& rhs) { return dynamic_modint(lhs) /= rhs; }

    friend bool operator==(const dynamic_modint& lhs, const dynamic_modint& rhs) { return lhs.val == rhs.val; }
    friend bool operator!=(const dynamic_modint& lhs, const dynamic_modint& rhs) { return lhs.val != rhs.val; }

    dynamic_modint pow(ull n) const {
        dynamic_modint res = 1, a = *this;
        while (n) {
            if (n & 1) res *= a;
            a *= a;
            n >>= 1;
        }
        return res;
    }
    dynamic_modint inv() const {
        ll a = val, b = mod(), u = 1, v = 0;
        while (b) {
            ll t = a / b;
            a -= t * b; swap(a, b);
            u -= t * v; swap(u, v);
        }
        return dynamic_modint(u);
    }

    friend std::ostream& operator<<(std::ostream& os, const dynamic_modint& m) {
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, dynamic_modint& m) {
        ll x;
        is >> x;
        m = dynamic_modint(x);
        return is;
    }
};

#endif /* MODINT__HPP_ */