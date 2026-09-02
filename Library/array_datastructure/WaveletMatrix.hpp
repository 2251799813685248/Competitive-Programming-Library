#ifndef WAVELET_MATRIX_HPP_
#define WAVELET_MATRIX_HPP_

#include <iostream>
#include <vector>
#include <cassert>
#include <deque>
using ll = long long;
using ull = unsigned long long;
using uint = unsigned int;
using namespace std;



/// @brief 完備辞書
struct FullyIndexableDictionary{
    ull* data;
    uint* largeblock;
    uint* one_per_64;
    uint* zero_per_64;

    bool initialized = false;//初期化されたかを保持しておく
    size_t length;

    /// @brief vectorを渡して直接初期化して構築
    /// @tparam INTEGER int,bool,longなどのboolにキャストできるもの
    /// @param init 
    template<typename INTEGER>
    FullyIndexableDictionary(vector<INTEGER> init){
        initialized = true;
        length = init.size();
        ull targetsize = ((init.size()+63)>>6)<<6;
        while (init.size() < targetsize){
            init.push_back(0);
        }

        data = new ull[targetsize>>6];
        largeblock = new uint[(targetsize>>6)+1];


        for (size_t i = 0; i < (targetsize>>6); i++){
            data[i] = 0;
            for (int j = 0; j < 64; j++){
                if (init[(i<<6)+j]) data[i] += (1<<j);
            }
            if (i > 0){
                largeblock[i] = __builtin_popcountll(data[i-1]);
                largeblock[i] += largeblock[i-1];
            }
            else{
                largeblock[i] = 0;
            }
        }
        largeblock[targetsize>>6] = __builtin_popcountll(data[(targetsize>>6)-1]);

        ull ocnt = 0, zcnt = 0;
        for (size_t i = 0; i < targetsize; i++){
            ocnt += __builtin_popcountll(data[i]);
        }
        zcnt = length-ocnt;

        one_per_64 = new uint[1+((ocnt+63)>>6)];
        for (size_t i = 0; i <= (ocnt+63)>>6; i++){
            one_per_64[i] = 0;
        }
        one_per_64[(ocnt+63)>>6] = length-1;
        zero_per_64 = new uint[1+((zcnt+63)>>6)];
        for (size_t i = 0; i <= (zcnt+63)>>6; i++){
            zero_per_64[i] = 0;
        }
        zero_per_64[(zcnt+63)>>6] = length-1;
        ocnt = 0;
        zcnt = 0;
        for (size_t i = 0; i < targetsize; i++){
            for (int j = 0; j < 64; j++){
                if (data[i]&(1ull<<j)){
                    if ((ocnt&63) == 0){
                        one_per_64[ocnt>>6] = (i<<6)+j;
                    }
                    ocnt++;
                }
                else{
                    if ((zcnt&63) == 0){
                        zero_per_64[zcnt>>6] = (i<<6)+j;
                    }
                    zcnt++;
                }
            }
        }
    }
    /// @brief 要素数のみを指定してすべての要素を0として構築。初期化はされていないので、まだ変更可能
    /// @attention 初期化関数を呼ぶまで使えない
    /// @param N 
    FullyIndexableDictionary(size_t N){
        length = N;
        data = new ull[(N+63)>>6];
        largeblock = new uint[(N+127)>>6];
        for (size_t i = 0; i < ((N+63)>>6); i++){
            data[i] = 0;
        }
        for (size_t i = 0; i < ((N+127)>>6); i++){
            largeblock[i] = 0;
        }
    }
    FullyIndexableDictionary(){}

    /// @brief 指定位置を1にする
    void write_1(const ull idx){
        assert(!initialized && idx < length);
        data[idx>>6] |= 1ull<<(idx&63);
    }
    /// @brief 指定位置を0にする
    void write_0(const ull idx){
        assert(!initialized && idx < length);
        data[idx>>6] &= ~(1ull<<(idx&63));
    }
    /// @brief 初期化して、ブロックごとの累積和も更新
    void init(){
        if (initialized){return;}
        initialized = true;
        ull datasize = (length+63)>>6;
        for (size_t i = 1; i < datasize; i++){
            largeblock[i] = __builtin_popcountll(data[i-1]) + largeblock[i-1];
        }
        largeblock[datasize] = largeblock[datasize-1] + __builtin_popcountll(data[datasize-1]);


        ull ocnt = 0, zcnt = 0;
        for (size_t i = 0; i < datasize; i++){
            ocnt += __builtin_popcountll(data[i]);
        }
        zcnt = length-ocnt;

        one_per_64 = new uint[1+((ocnt+63)>>6)];
        for (size_t i = 0; i < (ocnt+63)>>6; i++){
            one_per_64[i] = 0;
        }
        one_per_64[(ocnt+63)>>6] = length-1;
        zero_per_64 = new uint[1+((zcnt+63)>>6)];
        for (size_t i = 0; i < (zcnt+63)>>6; i++){
            zero_per_64[i] = 0;
        }
        zero_per_64[(zcnt+63)>>6] = length-1;
        ocnt = 0;
        zcnt = 0;
        for (size_t i = 0; i < datasize; i++){
            for (int j = 0; j < 64; j++){
                if (data[i]&(1ull<<j)){
                    if ((ocnt&63) == 0){
                        one_per_64[ocnt>>6] = (i<<6)+j;
                    }
                    ocnt++;
                }
                else{
                    if ((zcnt&63) == 0){
                        zero_per_64[zcnt>>6] = (i<<6)+j;
                    }
                    zcnt++;
                }
            }
        }
    }

    inline bool at(const ull idx){
        assert(initialized && idx < length);
        return data[idx>>6]&(1ull<<(idx&63));
    }
    inline bool operator[](const ull idx){
        assert(initialized && idx < length);
        return data[idx>>6]&(1ull<<(idx&63));
    }
    inline size_t size(){return length;}

    /// @brief 区間[0, idx]の中にいくつ1があるか
    /// @param idx
    /// @return 
    inline int rank_1(const ull idx){
        assert(initialized && idx <= length);
        return largeblock[idx>>6] + __builtin_popcountll(data[idx>>6]&(((1ull<<(idx&63))-1)*2+1));
    }
    /// @brief 区間[0, idx]の中にいくつ0があるか
    /// @param idx 
    /// @return 
    inline int rank_0(const ull idx){
        return idx+1-rank_1(idx);
    }
    /// @brief 閉区間[idx1, idx2]の中にいくつ1があるか
    /// @param idx1 
    /// @param idx2 
    /// @return 
    inline int rank_1_range(const ull idx1, const ull idx2){
        if (idx1 > idx2){return 0;}
        if (idx1 == 0){return rank_1(idx2);}
        return rank_1(idx2)-rank_1(idx1-1);
    }
    /// @brief 閉区間[idx1, idx2]の中にいくつ0があるか
    /// @param idx1 
    /// @param idx2 
    /// @return 
    inline int rank_0_range(const ull idx1, const ull idx2){
        if (idx1 > idx2){return 0;}
        if (idx1 == 0){return rank_0(idx2);}
        return rank_1(idx2)-rank_0(idx1-1);
    }

    /// @brief k番目に現れる1のindexを取得する。
    /// @attention `k`は0-indexed
    /// @param k
    /// @attention そのような1がなければ-1が返る。
    int select_1(const ull k){
        if (!initialized){
            cerr << "Not Initialized" << endl;
            assert(false);
        }
        if (k >= largeblock[(length+63)>>6]){return -1;}
        ll left = (ll)one_per_64[k>>6]-1, right = one_per_64[(k>>6)+1];
        while (right-left > 1){
            ll mid = (left+right)/2;
            if ((ull)rank_1(mid) >= k+1){
                right = mid;
            }
            else{
                left = mid;
            }
        }
        return right;
    }
    /// @brief k番目に現れる1のindexを取得する。
    /// @attention `k`は0-indexed
    /// @param k
    /// @attention そのような1がなければ-1が返る。
    int select_0(const ull k){
        if (!initialized){
            cerr << "Not Initialized" << endl;
            assert(false);
        }
        if (k >= length-largeblock[(length+63)>>6]){return -1;}
        ll left = (ll)zero_per_64[k>>6]-1, right = zero_per_64[(k>>6)+1];
        while (right-left > 1){
            ll mid = (left+right)/2;
            if ((ull)rank_0(mid) >= k+1){
                right = mid;
            }
            else{
                left = mid;
            }
        }
        return right;
    }
};

/// @brief 符号なし整数列に関する様々なクエリを処理することができる
/// @tparam INTEGER 整数
/// @details 
template<typename INTEGER>
struct WaveletMatrix{
    vector<FullyIndexableDictionary> M;
    vector<uint> zcnt;
    vector<pair<INTEGER,uint>> first_appear;
    size_t max_size;
    int lbl;//longest_bitlength
    const INTEGER e = static_cast<INTEGER>(1);

    WaveletMatrix(vector<INTEGER> A){
        max_size = A.size();
        lbl = 64-__builtin_clzll(max(e, *max_element(vall(A))));
        M = vector<FullyIndexableDictionary>(lbl);
        zcnt = vector<uint>(lbl, 0);
        for (int i = lbl-1; i >= 0; i--){
            deque<INTEGER> B;//0
            deque<INTEGER> C;//1
            M[i] = FullyIndexableDictionary(max_size);
            for (size_t j = 0; j < max_size; j++){
                if (A[j]&(e<<i)){
                    M[i].write_1(j);
                    C.push_back(A[j]);
                }
                else{
                    B.push_back(A[j]);
                    zcnt[i]++;
                }
            }
            M[i].init();
            size_t temp = 0;
            for (auto v : B){
                A[temp] = v;
                temp++;
            }
            for (auto v : C){
                A[temp] = v;
                temp++;
            }
        }
        for (size_t i = 0; i < max_size;){
            size_t j = i;
            while (A[i] == A[j] && j < max_size){
                j++;
            }
            first_appear.push_back({A[i], i});
            i = j;
        }
        sort(vall(first_appear));
    }
    WaveletMatrix(){}

    inline size_t size(){return max_size;}

    /// @brief インデックスアクセスを行う。範囲外の時は例外を発生させる
    /// @param idx 
    /// @return 
    inline INTEGER access(uint idx){
        if (idx >= max_size){
            cerr << "Index Out of Bounds" << endl;
            cerr << "Index must be less than or equal " + to_string((int)max_size-1) << endl;
            assert(false);
        }
        INTEGER r = 0;
        for (int i = lbl-1; i >= 0; i--){
            if (M[i][idx]){
                r++;
                idx = zcnt[i]+M[i].rank_1(idx)-1;
            }
            else{
                idx = M[i].rank_0(idx)-1;
            }
            if (i){r <<= 1;}
        }
        return r;
    }
    inline INTEGER operator[](uint idx) const {
        return access(idx);
    }

    /// @brief 区間[0,idx)に`val`がいくつ現れるかを求める
    /// @param idx 
    /// @param val 
    /// @return 
    inline int rank(uint idx, const INTEGER val){
        if (idx > max_size){
            cerr << "Index Out of Bounds" << endl;
            cerr << "Index must be less than or equal " + to_string((int)max_size) << endl;
            assert(false);
        }
        if (idx == 0){return 0;}
        int idx2 = idx-1;
        auto tempitr = lower_bound(vall(first_appear), val, [](const pair<INTEGER,uint> a, const INTEGER b){return a.first < b;});
        if (tempitr == first_appear.end() || val != tempitr->first){return 0;}
        for (int i = lbl-1; i >= 0; i--){
            if (val&(e<<i)){
                idx2 = zcnt[i] + M[i].rank_1(idx2)-1;
            }
            else{
                idx2 = M[i].rank_0(idx2)-1;
            }
            if (idx2 < 0){return 0;}
        }
        return idx2-tempitr->second+1;
    }
    /// @brief 区間[idx1,idx2)の中に`val`がいくつ現れるかを求める
    /// @param idx1 
    /// @param idx2 
    /// @param val 
    /// @return 
    inline int range_count(uint idx1, uint idx2, const INTEGER val){
        if (idx1 >= idx2){return 0;}
        return rank(idx2, val)-rank(idx1, val);
    }

    /// @brief 先頭から`k`番目に現れる`val`のindexを返す。そのようなindexが存在しなければ-1が返る
    /// @param k 0-indexed
    /// @param val 
    /// @return 
    inline int select(uint k, const INTEGER val){
        auto tempitr = lower_bound(vall(first_appear), val, [](const pair<ull,ll> a, const ull b){return a.first < b;});
        if (tempitr == first_appear.end() || tempitr->first != val || k >= max_size){
            return -1;
        }
        int firstidx = tempitr->second+k;
        for (int i = 0; i < lbl; i++){
            if (val&(e<<i)){
                firstidx = M[i].select_1(firstidx - zcnt[i]);
                if (firstidx == -1){
                    return -1;
                }
            }
            else{
                firstidx = M[i].select_0(firstidx);
                if (firstidx == -1){
                    return -1;
                }
            }
        }
        return firstidx;
    }

    /// @brief 区間[l, r)において、小さい方からk番目の値を求める。存在しない場合は例外が発生する
    /// @param l 
    /// @param r 
    /// @param k 0-indexed
    inline INTEGER range_kth_min(uint l, uint r, uint k){
        if (l >= r || k >= r-l || r > max_size){
            cerr << "Invalid Range" << endl;
            cerr << "l,r,k = " + to_string(l) + ", " + to_string(r) + ", " + to_string(k) << endl;
            assert(false);
        }
        INTEGER ret_val = 0;
        r--;
        for (int i = lbl-1; i >= 0; i--){
            int range_ocnt = M[i].rank_1_range(l,r);
            if (k >= r-l+1-range_ocnt){
                ret_val++;
                k -= r-l+1-range_ocnt;
                l = zcnt[i] + (l == 0 ? 0 : M[i].rank_1(l-1));
                r = zcnt[i] + M[i].rank_1(r)-1;
            }
            else{
                l = (l == 0 ? 0 : M[i].rank_0(l-1));
                r = M[i].rank_0(r)-1;
            }
            if (i){ret_val <<= 1;}
        }
        return ret_val;
    }
    /// @brief 区間[l, r)において、大きい方からk番目の値を求める。存在しない場合は例外が発生する
    /// @param l 
    /// @param r 
    /// @param k 0-indexed
    inline INTEGER range_kth_max(uint l, uint r, uint k){
        if (l >= r || k >= r-l || r > max_size){
            cerr << "Invalid Range" << endl;
            cerr << "l,r,k = " + to_string(l) + ", " + to_string(r) + ", " + to_string(k) << endl;
            assert(false);
        }
        return range_kth_min(l,r, r-l-1-k);
    }

    /// @brief 区間[l,r)において、val以下の値がいくつ存在するかを求める
    /// @param l 0-indexed
    /// @param r 0-indexed
    /// @param val 自身を含む
    int range_frequency(uint l, uint r, INTEGER val){
        if (l >= r){
            return 0;
        }
        if (r > max_size){
            cerr << "Invalid Range" << endl;
            cerr << "l,r = " + to_string(l) + ", " + to_string(r) << endl;
            assert(false);
        }
        r--;
        if (lbl < 64-__builtin_clzll(val+1)){return r-l;}

        int ans = 0;
        for (int i = lbl-1; i >= 0; i--){
            int range_ocnt = M[i].rank_1_range(l,r);
            if ((val+1)&(e<<i)){
                ans += r-l+1-range_ocnt;
                l = zcnt[i] + (l == 0 ? 0 : M[i].rank_1(l-1));
                r = zcnt[i] + M[i].rank_1(r)-1;
            }
            else{
                l = (l == 0 ? 0 : M[i].rank_0(l-1));
                r = M[i].rank_0(r)-1;
            }
        }
        return ans;
    }
    /// @brief 区間[l,r)において、val_low以上val_high以下の値がいくつ存在するかを求める
    /// @param l 0-indexed
    /// @param r 0-indexed
    /// @param val_low 自身を含む
    /// @param val_high 自身を含む
    int range_frequency(uint l, uint r, INTEGER val_low, INTEGER val_high){
        return range_frequency(l,r,val_high)-(val_low == 0 ? 0 : range_frequency(l,r,val_low-1));
    }

    /// @brief 区間[l,r)において、val以上の最小値より小さい値のうち、最も大きい値を返す
    /// @attention もしそのような値が存在しない場合、-1が返る(18446744073709551616や4294967295など)
    /// @param l 
    /// @param r 
    /// @param val 
    /// @return 
    INTEGER range_prev_lower_bound(uint l, uint r, INTEGER val){
        int temp = range_frequency(l,r,val-1);
        if (val == 0 || temp == 0){return -1;}
        return range_kth_min(l,r,temp-1);
    }
    /// @brief 区間[l,r)において、val以上の最小値を返す
    /// @param l 
    /// @param r 
    /// @param val 
    /// @return 
    INTEGER range_lower_bound(uint l, uint r, INTEGER val){
        int temp = range_frequency(l,r,val-1);
        if (temp == r-l){return -1;}
        return range_kth_min(l,r,temp);
    }

};




#endif /* WAVELET_MATRIX_HPP_ */