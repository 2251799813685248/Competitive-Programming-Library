#include <iostream>
#include <math_functions.hpp>
template<typename T> struct fraction{
    T numerator;
    T denominator;
    fraction(){
        numerator = 0;
        denominator = 1;
    }
    template<typename U> constexpr fraction(U _init){
        numerator = _init;
        denominator = 1;
    }
    constexpr fraction(T _init_num, T _init_den){
        if (_init_den < 0){
            _init_num *= -1;
            _init_den *= -1;
        }
        numerator = _init_num;
        denominator = _init_den;
    }
    friend std::ostream& operator<<(std::ostream& os, const fraction& f) {
        if (f.denominator == 1){
            return os << f.numerator;
        }
        return os << f.numerator << "/" << f.denominator;
    }
    

    inline constexpr T floor(){
        return floor2(numerator, denominator);
    }
    inline constexpr T ceil(){
        return ceil2(numerator, denominator);
    }
    inline constexpr fraction& reduce(){
        T g = gcd(numerator, denominator);
        if (g < 0){g *= -1;}
        if (denominator < 0){g *= -1;}
        numerator /= g;
        denominator /= g;
        return *this;
    }
    inline constexpr fraction& inv(){
        swap(denominator, numerator);
        return *this;
    }
    fraction& operator+=(const fraction& rhs) {
        numerator *= rhs.denominator;
        numerator += denominator*rhs.numerator;
        denominator *= rhs.denominator;
        return reduce();
    }
    fraction& operator-=(const fraction& rhs) {
        numerator *= rhs.denominator;
        numerator -= denominator*rhs.numerator;
        denominator *= rhs.denominator;
        return reduce();
    }
    fraction& operator*=(const fraction& rhs) {
        numerator *= rhs.numerator;
        denominator *= rhs.denominator;
        return reduce();
    }
    fraction& operator/=(const fraction& rhs) {
        numerator *= rhs.denominator;
        denominator *= rhs.numerator;
        return reduce();
    }

    fraction operator+() const { return *this; }
    fraction operator-() const { return fraction(0) - *this; }

    friend fraction operator+(const fraction& lhs, const fraction& rhs){return fraction(lhs) += rhs;}
    friend fraction operator-(const fraction& lhs, const fraction& rhs){return fraction(lhs) -= rhs;}
    friend fraction operator*(const fraction& lhs, const fraction& rhs){return fraction(lhs) *= rhs;}
    friend fraction operator/(const fraction& lhs, const fraction& rhs){return fraction(lhs) /= rhs;}

    friend bool operator==(const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator == rhs.numerator*lhs.denominator;}
    friend bool operator!=(const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator != rhs.numerator*lhs.denominator;}

    friend bool operator < (const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator < rhs.numerator*lhs.denominator;}
    friend bool operator <= (const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator <= rhs.numerator*lhs.denominator;}
    friend bool operator > (const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator > rhs.numerator*lhs.denominator;}
    friend bool operator >= (const fraction& lhs, const fraction& rhs){return lhs.numerator*rhs.denominator >= rhs.numerator*lhs.denominator;}




};