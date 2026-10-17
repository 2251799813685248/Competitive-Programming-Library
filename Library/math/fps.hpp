#ifndef FPS__HPP_
#define FPS__HPP_

#include <iostream>
#include <iterator>
#include <list.hpp>
#include <cassert>
#include <modint.hpp>
#include <mod_table.hpp>
using namespace std;
using ll = long long;
using uint = unsigned;

#ifndef LEN_DEFINED
#define LEN_DEFINED
template<typename T> inline constexpr ll len(const T& A){return ssize(A);}
#endif


template<uint T>
struct FormalPowerSeries{
    using mint = constant_modint<T>;
    list<mint> f;
    ll sz;
    constexpr FormalPowerSeries(){
        sz = 0;
    }
    /// @brief 指定したサイズのfpsを作成する。
    constexpr FormalPowerSeries(uint _init_sz) : f(_init_sz), sz(_init_sz) {}
    constexpr FormalPowerSeries(uint _init_sz, mint _init_val) : f(_init_sz,_init_val), sz(_init_sz) {}
    template<typename U> constexpr FormalPowerSeries(const list<U>& _init){
        f.assign(_init.begin(), _init.end());
        sz = f.size();
    }
    template<typename U> inline constexpr FormalPowerSeries(initializer_list<U> _init) : FormalPowerSeries(list<U>(_init)){}
    inline constexpr uint size() const {return sz;}
    inline constexpr bool empty() const {return size() == 0;}
    inline constexpr mint* data() {return empty() ? nullptr : &f[0];}
    inline constexpr const mint* data() const {return empty() ? nullptr : &f[0];}
    constexpr void resize(uint _new_size){
        if (sz == _new_size){return;}
        f.resize(_new_size, 0);
        sz = _new_size;
    }
    inline constexpr mint& operator[](uint deg) {return f[deg];}
    inline constexpr const mint& operator[](uint deg) const {return f[deg];}
    template<typename U> constexpr operator list<U>() const {
        list<U> res(sz);
        for (int i = 0; i < sz; i++){
            res[i] = f[i].val;
        }
        return res;
    }

    constexpr void differential(){
        if (sz == 0){return;}
        for (int i = 0; i < sz-1; i++){
            f[i] = mint(i+1)*f[i+1];
        }
        f.pop();
        sz--;
    }
    constexpr void integral(const mod_table<T>& mtable){
        if (sz == 0){return;}
        sz++;
        f.append(0);
        for (int i = sz-1; i > 0; i--){
            f[i] = mtable.inverse_mod(i)*f[i-1];
        }
        f[0] = 0;
    }
    constexpr mint evaluate(const mint& x){
        mint res = 0;
        for (int i = 0; i < sz; i++){
            res *= x;
            res += f[sz-1-i];
        }
        return res;
    }
};

#endif /* FPS__HPP_ */
