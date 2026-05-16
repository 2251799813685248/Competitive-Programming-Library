#ifndef FPS__HPP_
#define FPS__HPP_

#include <iostream>
#include <vector>
#include <cassert>
using namespace std;
using ll = long long;
using uint = unsigned;

struct FormalPowerSeries{
    vector<uint> f;
    uint sz;
    FormalPowerSeries(){
        sz = 0;
    }
    /// @brief 指定したサイズのfpsを作成する。
    FormalPowerSeries(uint _init_sz){
        f.resize(_init_sz, 0);
        sz = _init_sz;
    }
    FormalPowerSeries(uint _init_sz, uint _init_val){
        f.resize(_init_sz, _init_val);
        sz = _init_sz;
    }
    template<typename T> FormalPowerSeries(const vector<T>& _init){
        f.assign(_init.begin(), _init.end());
        sz = f.size();
    }
    template<typename T> FormalPowerSeries(initializer_list<T> _init) : FormalPowerSeries(vector<T>(_init)){}
    uint size() const {return sz;}
    void resize(uint _new_size){
        if (sz == _new_size){return;}
        f.resize(_new_size, 0);
        sz = _new_size;
    }
    inline uint& operator[](uint deg) {return f[deg];}
    inline const uint& operator[](uint deg) const {return f[deg];}
    template<typename T> operator vector<T>() const {
        vector<T> res(sz);
        for (uint i = 0; i < sz; i++){
            res[i] = f[i];
        }
        return res;
    }
};

#endif /* FPS__HPP_ */