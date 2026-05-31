#ifndef SEG_TREE_HPP_
#define SEG_TREE_HPP_

#include <functional>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;



/// @brief 抽象化セグメントツリー
/// @attention コンストラクタ1 SegTree(A, e, op, mapping)
/// @attention コンストラクタ2 SegTree(N, I, e, op, mapping)
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
template<typename info>
struct SegTree{
    int log2N;//セグ木の高さ-1
    info e;///単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)

    vector<info> tree;//セグ木本体
    int max_capacity;

    /// @brief N個のIで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    SegTree(int N, info I, info eee, function<info(info,info)> op){
        //基本情報を登録
        e = eee;
        operation = op;
        max_capacity = N;

        //セグ木のサイズを決定
        if (N == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<info>(1<<(log2N+1));
        tree[0] = e;
        for (int i = 0; i < N; i++){
            tree[i+(1<<log2N)] = I;
        }
        for (int i = N; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = e;
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = operation(tree[2*i], tree[2*i+1]);
        }
    }
    /// @brief vector Aで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    SegTree(const vector<info> &A, info eee, function<info(info,info)> op){
        //基本情報を登録
        e = eee;
        operation = op;
        max_capacity = A.size();

        //セグ木のサイズを決定
        if (A.size() == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<info>(1<<(log2N+1));
        tree[0] = e;
        for (int i = 0; i < max_capacity; i++){
            tree[i+(1<<log2N)] = A[i];
        }
        for (int i = max_capacity; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = e;
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = operation(tree[2*i], tree[2*i+1]);
        }
    }

    int get_left(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(index_on_tree-(1<<(31-__builtin_clz(index_on_tree))));
    }
    int get_right(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(1+index_on_tree-(1<<(31-__builtin_clz(index_on_tree))))-1;
    }    


    /// @brief index閉区間[L,R]において、集計を行う。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @return [L,R]での集計結果
    info range_get(const int L, const int R){
        if (L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            assert(false);
        }
        if (L > R){
            return e;
        }
        info ret = e;
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            ret = operation(ret, tree[(left+(1<<log2N))>>log2interval]);
            left += 1<<log2interval;
        }
        return ret;
    }

    /// @brief index tが指す要素を取得する。変更はできない
    /// @param t 
    /// @return 要素
    info get(const int t) const{
        if (t >= max_capacity || t < 0){
            assert(false);
        }
        return tree[t+(1<<log2N)];
    }

    /// @brief index tが指す要素をvalに置換する。
    /// @param L 置換対象のindex
    /// @param val 置換後の値
    void pointwise_update(const int t, const info &val){
        if (t >= max_capacity || t < 0){
            assert(false);
        }
        int start_index = t + (1<<log2N);
        tree[start_index] = val;
        start_index >>= 1;
        while (start_index){
            tree[start_index] = operation(tree[2*start_index],tree[2*start_index+1]);
            start_index >>= 1;
        }
    }

    /// @brief 左端をLに固定したとき、Gがtrueになる最小の右端indexを返す。もしなければINF(=2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(L, t)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最小が求まる。
    /// @return Gがtrueになる最小右端indexまたは2147483647
    int min_right(int L, const function<bool(info)> &G){
        if (L >= max_capacity || L < 0){
            assert(false);
        }
        info current_result = e;

        checkpoint:

        int ctz = L == 0 ? log2N : __builtin_ctz(L);
        if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz]))){
            if (get_right(((1<<log2N)+L)>>ctz)+1 == 1<<log2N){
                return 2147483647;
            }
            current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz]);
            L = get_right(((1<<log2N)+L)>>ctz)+1;
            goto checkpoint;
        }

        for (int i = ctz-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+L)>>i]))){
                current_result = operation(current_result, tree[((1<<log2N)+L)>>i]);
                L = get_right(((1<<log2N)+L)>>i)+1;
                goto checkpoint;
            }
        }
        return L;
    }
    /// @brief 右端をRに固定したとき、Gがtrueになる最大の左端indexを返す。もしなければ-INF-1(=-2147483648)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。
    /// @param R 右端 
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(t, R)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最大が求まる。
    /// @return Gがtrueになる最大左端index
    int max_left(int R, const function<bool(info)> &G){
        if (R >= max_capacity || R < 0){
            assert(false);
        }
        info current_result = e;

        checkpoint:

        int cto = __builtin_ctz(~R);//cto...count trailing one
        if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto]))){
            if (get_left(((1<<log2N)+R)>>cto) == 0){
                return -2147483648;
            }
            current_result = operation(current_result, tree[((1<<log2N)+R)>>cto]);
            R = get_left(((1<<log2N)+R)>>cto)-1;
            goto checkpoint;
        }

        for (int i = cto-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+R)>>i]))){
                current_result = operation(current_result, tree[((1<<log2N)+R)>>i]);
                R = get_left(((1<<log2N)+R)>>i)-1;
                goto checkpoint;
            }
        }
        return R;
    }

    info operator[](const int t)const{
        return get(t);
    }
    int size()const{
        return max_capacity;
    }
};




#endif /* SEG_TREE_HPP_ */