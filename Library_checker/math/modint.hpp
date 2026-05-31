#ifndef MODINT__HPP_
#define MODINT__HPP_

#include <iostream>
#include <math_functions.hpp>
using namespace std;
using ll = long long;



/// @brief modint
template<const int M>
struct mint{
    int val = 0;
    mint(const ll x){
        val = x%M;
    }
    mint(const mint &m){
        val = m.val;
    }
    mint(){}

    mint inv(){
        val = math::inverse_mod(val, M);
        return *this;
    }
    mint pow(ll N){
        val = math::modpow(val, N, M);
        return *this;
    }
    

    // 入出力用オーバーロード
    friend std::ostream& operator<<(std::ostream& os, const mint m){
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, mint &m){
        ll a;
        is >> a;
        m = a;
        return is;
    }


    void operator=(const ll x){
        val = x%M;
    }
    void operator=(const mint &a){
        val = a.val;
    }
    
    bool operator==(const ll x) const{
        return val == x;
    }
    bool operator==(const mint &a) const{
        return val == a.val;
    }
    bool operator!=(const ll x) const{
        return val != x;
    }
    bool operator!=(const mint &a) const{
        return val != a.val;
    }

    mint operator+(){
        return *this;
    }
    mint operator-(){
        if (val != 0) val = M-val;
        return *this;
    }

    mint operator+(const mint &a) const{
        return mint(val+a.val);
    }
    void operator+=(const mint &a){
        val += a.val;
        if (val >= M) val -= M;
    }
    mint operator+(const ll b) const{
        return mint(val+b%M);
    }
    void operator+=(const ll b){
        val += b%M;
        if (val >= M) val -= M;
    }

    mint operator-(const mint &a) const{
        return mint(M+val-a.val);
    }
    void operator-=(const mint &a){
        val += M-a.val;
        if (val >= M) val -= M;
    }
    mint operator-(const ll b) const{
        return mint(val+M-b%M);
    }
    void operator-=(const ll b){
        val += M-b%M;
        if (val >= M) val -= M;
    }

    mint operator*(const mint &a) const{
        return mint((ll)val*a.val);
    }
    void operator*=(const mint &a){
        val = ((ll)val*a.val)%M;
    }
    mint operator*(const ll b) const{
        return mint(val*(b%M));
    }
    void operator*=(const ll b){
        val = (val*(b%M))%M;
    }

    mint operator/(const mint &a) const{
        return mint((ll)val*inverse_mod(a.val,M));
    }
    void operator/=(const mint &a){
        val = ((ll)val*inverse_mod(a.val,M))%M;
    }
    mint operator/(const ll b) const{
        return mint(val*inverse_mod(b%M,M));
    }
    void operator/=(const ll b){
        val = (val*inverse_mod(b%M,M))%M;
    }
};
struct dynamic_mint{
    int val = 0;
    int M = -1;
    dynamic_mint(const ll x){
        val = x;
    }
    dynamic_mint(const ll x, const ll MOD){
        M = MOD;
        val = x%M;
    }
    dynamic_mint(const dynamic_mint &m){
        update_and_check_mod(m.M);
        val = m.val;
    }
    dynamic_mint(){}

    void update_and_check_mod(const int new_MOD){
        if (M == -1 && new_MOD == -1){
            //cerr << "M is invalid" << endl;
            //assert(false);
        }
        M = max(M, new_MOD);
    }

    dynamic_mint inv(){
        val = inverse_mod(val, M);
        return *this;
    }
    dynamic_mint pow(ll N){
        val = modpow(val, N, M);
        return *this;
    }
    

    // 入出力用オーバーロード
    friend std::ostream& operator<<(std::ostream& os, const dynamic_mint m){
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, dynamic_mint &m){
        ll a;
        is >> a;
        m = a;
        return is;
    }


    void operator=(const ll x){
        val = x;
    }
    void operator=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = a.val;
    }
    
    bool operator==(const ll x) const{
        return val == x;
    }
    bool operator==(const dynamic_mint &a) const{
        return val == a.val;
    }
    bool operator!=(const ll x) const{
        return val != x;
    }
    bool operator!=(const dynamic_mint &a) const{
        return val != a.val;
    }

    dynamic_mint operator+(){
        return *this;
    }
    dynamic_mint operator-(){
        if (val != 0) val = M-val;
        return *this;
    }

    dynamic_mint operator+(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint(val+a.val, M);
    }
    void operator+=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val += a.val;
        if (val >= M) val -= M;
    }
    dynamic_mint operator+(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val+b%M, M);
    }
    void operator+=(const ll b){
        update_and_check_mod(-1);
        val += b%M;
        if (val >= M) val -= M;
    }

    dynamic_mint operator-(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint(M+val-a.val, M);
    }
    void operator-=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val += M-a.val;
        if (val >= M) val -= M;
    }
    dynamic_mint operator-(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val+M-b%M, M);
    }
    void operator-=(const ll b){
        update_and_check_mod(-1);
        val += M-b%M;
        if (val >= M) val -= M;
    }

    dynamic_mint operator*(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint((ll)val*a.val, M);
    }
    void operator*=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = ((ll)val*a.val)%M;
    }
    dynamic_mint operator*(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val*(b%M), M);
    }
    void operator*=(const ll b){
        update_and_check_mod(-1);
        val = (val*(b%M))%M;
    }

    dynamic_mint operator/(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint((ll)val*inverse_mod(a.val,M), M);
    }
    void operator/=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = ((ll)val*inverse_mod(a.val,M))%M;
    }
    dynamic_mint operator/(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val*inverse_mod(b%M,M), M);
    }
    void operator/=(const ll b){
        update_and_check_mod(-1);
        val = (val*inverse_mod(b%M,M))%M;
    }
};




#endif /* MODINT__HPP_ */