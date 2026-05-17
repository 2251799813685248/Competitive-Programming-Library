#ifndef SEG_TREE__LAZY_HPP_
#define SEG_TREE__LAZY_HPP_

#include <functional>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;



/// @brief 抽象化遅延セグメントツリー
/// @attention コンストラクタ1 LazySegTree(A, e, op, mapping, composition, id)
/// @attention コンストラクタ2 LazySegTree(N, I, e, op, mapping, composition , id)
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
/// @param mapping infoに対してfuncを作用させた結果を返す関数(アフィン変換ならx -> ax+b)
/// @param composition func同士の合成結果を1つのfuncにする関数((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
/// @param id funcの恒等写像(アフィン変換ならx -> 1x+0)
template<typename info, typename func>
struct LazySegTree{
    int log2N;//セグ木の高さ-1
    info e;///セグ木に載っているものの単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    function<info(func,info)> mapping;//更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    function<func(func,func)> composition;//mを合成した結果(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    func id;//mappingの恒等写像版(アフィン変換ならx -> 1x+0など)

    vector<pair<info,func>> tree;//セグ木本体(info, delay)
    int max_capacity;

    /// @brief N個のIで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    /// @param c mを合成した結果((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    /// @param ididid mの恒等写像版(アフィン変換ならx -> 1x+0)
    LazySegTree(int N, info I, info eee, function<info(info,info)> op, function<info(func,info)> m, function<func(func,func)> c, func ididid){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;
        composition = c;
        id = ididid;
        max_capacity = N;

        //セグ木のサイズを決定
        if (N == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<pair<info,func>>(1<<(log2N+1));
        tree[0] = make_pair(e,id);
        for (int i = 0; i < N; i++){
            tree[i+(1<<log2N)] = make_pair(I,id);
        }
        for (int i = N; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = make_pair(e,id);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = make_pair(operation(tree[2*i].first, tree[2*i+1].first),id);
        }
    }
    /// @brief vector Aで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    /// @param c mを合成した結果((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    /// @param ididid mの恒等写像版(アフィン変換ならx -> 1x+0)
    LazySegTree(const vector<info> &A, info eee, function<info(info,info)> op, function<info(func,info)> m, function<func(func,func)> c, func ididid){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;
        composition = c;
        id = ididid;
        max_capacity = A.size();

        //セグ木のサイズを決定
        if (A.size() == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));


        //セグ木に実際に乗せるvectorを構築
        tree = vector<pair<info,func>>(1<<(log2N+1));
        tree[0] = make_pair(e,id);
        for (int i = 0; i < max_capacity; i++){
            tree[i+(1<<log2N)] = make_pair(A[i],id);
        }
        for (int i = max_capacity; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = make_pair(e,id);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = make_pair(operation(tree[2*i].first, tree[2*i+1].first),id);
        }
    }

    int get_left(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(index_on_tree-(1<<(31-__builtin_clz(index_on_tree))));
    }
    int get_right(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(1+index_on_tree-(1<<(31-__builtin_clz(index_on_tree))))-1;
    }

    /// @brief 遅延情報の伝播を行う
    void tell_info(const int index_on_tree){
        if (index_on_tree >= (1<<log2N)){
            tree[index_on_tree].second = id;
            return;
        }
        //左の子に伝播
        tree[2*index_on_tree].first = mapping(tree[index_on_tree].second, tree[2*index_on_tree].first);
        tree[2*index_on_tree].second = composition(tree[index_on_tree].second, tree[2*index_on_tree].second);
        //右の子に伝播
        tree[2*index_on_tree+1].first = mapping(tree[index_on_tree].second,tree[2*index_on_tree+1].first);
        tree[2*index_on_tree+1].second = composition(tree[index_on_tree].second, tree[2*index_on_tree+1].second);

        tree[index_on_tree].second = id;
    }


    deque<int> lazy_node;//区間集計、区間更新に使う。
    deque<int> lazy_node_flipped;//区間更新で使う。


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
        int Lstart = L + (1<<log2N);
        int Rstart = R+1 + (1<<log2N);
        int lm = (Lstart / (Lstart & -Lstart)) >> 1;
        int rm = (Rstart / (Rstart & -Rstart)) >> 1;
        while (Lstart < Rstart){
            if (Rstart <= rm){
                lazy_node.push_back(Rstart);
            }
            if (Lstart <= lm){
                lazy_node.push_back(Lstart);
            }
            Lstart >>= 1;
            Rstart >>= 1;
        }
        while (Lstart){
            lazy_node.push_back(Lstart);
            Lstart >>=1;
        }


        while (!lazy_node.empty()){
            tell_info(lazy_node.back());
            lazy_node.pop_back();
        }
        info ret = e;
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            ret = operation(ret, tree[(left+(1<<log2N))>>log2interval].first);
            left += 1<<log2interval;
        }
        return ret;
    }

    /// @brief index閉区間[L,R]に対してFをmappingする。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @param F 適用する写像(アフィン変換ならaとbを持った構造体など)
    void range_update(const int L, const int R, const func &F){
        if (L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            assert(false);
        }
        if (L > R){
            return;
        }
        int Lstart = L + (1<<log2N);
        int Rstart = R+1 + (1<<log2N);
        int lm = (Lstart / (Lstart & -Lstart)) >> 1;
        int rm = (Rstart / (Rstart & -Rstart)) >> 1;
        while (Lstart && Lstart < Rstart){
            if (Rstart <= rm){
                lazy_node.push_back(Rstart);
            }
            if (Lstart <= lm){
                lazy_node.push_back(Lstart);
            }
            Lstart >>= 1;
            Rstart >>= 1;
        }
        while (Lstart){
            lazy_node.push_back(Lstart);
            Lstart >>=1;
        }


        while (!lazy_node.empty()){
            tell_info(lazy_node.back());
            lazy_node_flipped.push_back(lazy_node.back());
            lazy_node.pop_back();
        }
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            tree[(left+(1<<log2N))>>log2interval].first = mapping(F,tree[(left+(1<<log2N))>>log2interval].first);
            tree[(left+(1<<log2N))>>log2interval].second = composition(F,tree[(left+(1<<log2N))>>log2interval].second);
            left += 1<<log2interval;
        }
        while (!lazy_node_flipped.empty()){
            if (lazy_node_flipped.back()>>log2N){lazy_node_flipped.pop_back();continue;}
            tree[lazy_node_flipped.back()].first = operation(tree[2*lazy_node_flipped.back()].first, tree[2*lazy_node_flipped.back()+1].first);
            lazy_node_flipped.pop_back();
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

        int ctz_init = L == 0 ? log2N : __builtin_ctz(L);
        for (int i = log2N; i > ctz_init; i--){
            tell_info(((1<<log2N)+L)>>i);
        }

        checkpoint:

        int ctz = L == 0 ? log2N : __builtin_ctz(L);
        tell_info(((1<<log2N)+L)>>ctz);
        if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz].first))){
            if (get_right(((1<<log2N)+L)>>ctz)+1 == 1<<log2N){
                return 2147483647;
            }
            current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz].first);
            L = get_right(((1<<log2N)+L)>>ctz)+1;
            goto checkpoint;
        }

        for (int i = ctz-1; i >= 0; i--){
            tell_info(((1<<log2N)+L)>>i);
            if (!G(operation(current_result, tree[((1<<log2N)+L)>>i].first))){
                current_result = operation(current_result, tree[((1<<log2N)+L)>>i].first);
                L = get_right(((1<<log2N)+L)>>i)+1;
                goto checkpoint;
            }
        }
        return L;
    }
    /// @brief 右端をRに固定したとき、Gがtrueになる最大の左端indexを返す。もしなければ-INF-1(=-2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(t, R)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最小が求まる。
    /// @return Gがtrueになる最大左端indexまたは-2147483648
    int max_left(int R, const function<bool(info)> &G){
        if (R >= max_capacity || R < 0){
            assert(false);
        }
        info current_result = e;

        int cto_init = __builtin_ctz(~R);
        for (int i = log2N; i > cto_init; i--){
            tell_info(((1<<log2N)+R)>>i);
        }

        checkpoint:

        int cto = __builtin_ctz(~R);//cto...count trailing one
        tell_info(((1<<log2N)+R)>>cto);
        if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto].first))){
            if (get_left(((1<<log2N)+R)>>cto) == 0){
                return -2147483648;
            }
            current_result = operation(current_result, tree[((1<<log2N)+R)>>cto].first);
            R = get_left(((1<<log2N)+R)>>cto)-1;
            goto checkpoint;
        }

        for (int i = cto-1; i >= 0; i--){
            tell_info(((1<<log2N)+R)>>i);
            if (!G(operation(current_result, tree[((1<<log2N)+R)>>i].first))){
                current_result = operation(current_result, tree[((1<<log2N)+R)>>i].first);
                R = get_left(((1<<log2N)+R)>>i)-1;
                goto checkpoint;
            }
        }
        return R;
    }

    info operator[](const int t)const{
        return range_get(t,t);
    }
    int size()const{
        return max_capacity;
    }
};




#endif /* SEG_TREE__LAZY_HPP_ */