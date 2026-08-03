#ifndef FPS__HPP_
#define FPS__HPP_

#include <iostream>
#include <iterator>
#include <vector>
#include <cassert>
#include <modint.hpp>
#include <mod_table.hpp>
using namespace std;
using ll = long long;
using uint = unsigned;


template<uint T>
struct FormalPowerSeries{
    using mint = constant_modint<T>;
    vector<mint> f;
    uint sz;
    FormalPowerSeries(){
        sz = 0;
    }
    /// @brief 指定したサイズのfpsを作成する。
    FormalPowerSeries(uint _init_sz){
        f.resize(_init_sz, 0);
        sz = _init_sz;
    }
    FormalPowerSeries(uint _init_sz, mint _init_val){
        f.resize(_init_sz, _init_val);
        sz = _init_sz;
    }
    template<typename U> FormalPowerSeries(const vector<U>& _init){
        f.assign(_init.begin(), _init.end());
        sz = f.size();
    }
    template<typename U> FormalPowerSeries(initializer_list<U> _init) : FormalPowerSeries(vector<U>(_init)){}
    uint size() const {return sz;}
    void resize(uint _new_size){
        if (sz == _new_size){return;}
        f.resize(_new_size, 0);
        sz = _new_size;
    }
    inline mint& operator[](uint deg) {return f[deg];}
    inline const mint& operator[](uint deg) const {return f[deg];}
    template<typename U> operator vector<U>() const {
        vector<U> res(sz);
        for (uint i = 0; i < sz; i++){
            res[i] = f[i].val;
        }
        return res;
    }

    void differential(){
        if (sz == 0){return;}
        for (uint i = 0; i < sz-1; i++){
            f[i].val = (i+1)*(ull)f[i+1].val%T;
        }
        f.pop_back();
        sz--;
    }
    void integral(const mod_table<T>& mtable){
        if (sz == 0){return;}
        sz++;
        f.push_back(0);
        for (uint i = sz; i > 0; i--){
            f[i].val = mtable.invmodlist[i]*(ull)f[i-1].val%T;
        }
    }
};

#endif /* FPS__HPP_ */