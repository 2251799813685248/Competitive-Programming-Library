#ifndef SEG_TREE__DYNAMIC_LAZY_HPP_
#define SEG_TREE__DYNAMIC_LAZY_HPP_

#include <functional>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>
using namespace std;
using ll = long long;



/// @brief 抽象化動的遅延セグメントツリー
/// @attention コンストラクタ1 DynamicLazySegTree(A, e, op, mapping, composition, id)
/// @attention コンストラクタ2 DynamicLazySegTree(N, I, e, op, mapping, composition , id)
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
/// @param init_info 指定した区間内の初期状態を返す関数
/// @param mapping infoに対してfuncを作用させた結果を返す関数(アフィン変換ならx -> ax+b)
/// @param composition func同士の合成結果を1つのfuncにする関数((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
/// @param id funcの恒等写像(アフィン変換ならx -> 1x+0)
template<typename info, typename func>
struct DynamicLazySegTree{
    int log2N;//セグ木の高さ-1
    ll max_capacity;
    info e;///セグ木に載っているものの単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    function<info(ll,ll)> init_info; //区間[l,r]に対する初期値を返す関数
    function<info(func,info)> mapping;//更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    function<func(func,func)> composition;//mを合成した結果(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    func id;//mappingの恒等写像版(アフィン変換ならx -> 1x+0など)

    //セグ木のノードを扱う
    struct SegTreeNode{
        SegTreeNode* left = nullptr;
        SegTreeNode* right = nullptr;
        info I;
        func delay;
    };

    SegTreeNode* root;

    /// @brief N個のIで初期化
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param I 区間[l,r]に対する初期値を返す関数
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    /// @param c mを合成した結果((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    /// @param ididid mの恒等写像版(アフィン変換ならx -> 1x+0)
    DynamicLazySegTree(ll N, info eee, function<info(info,info)> op, function<info(ll,ll)> I, function<info(func,info)> m, function<func(func,func)> c, func ididid){
        //基本情報を登録
        e = eee;
        operation = op;
        init_info = I;
        mapping = m;
        composition = c;
        id = ididid;
        max_capacity = N;

        //セグ木のサイズを決定
        if (N == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        root = new SegTreeNode;
        root->delay = id;
        root->I = init_info(0, (1ll<<log2N)-1);
    }
    
    /// @brief 遅延情報の伝播を行う
    void tell_info(SegTreeNode* node){
        //左の子に伝播
        node->left->I = mapping(node->delay, node->left->I);
        node->left->delay = composition(node->delay, node->left->delay);
        //右の子に伝播
        node->right->I = mapping(node->delay, node->right->I);
        node->right->delay = composition(node->delay, node->right->delay);
        
        node->delay = id;
    }

    /// @brief index閉区間[L,R]において、集計を行う。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @return [L,R]での集計結果
    info range_get(const ll L, const ll R, ll nowleft = 0, int depth = 0, SegTreeNode* now = nullptr){
        if (L > R){return e;}
        if (nowleft > R || nowleft + (1ll<<(log2N-depth)) <= L){return e;}
        if (now == nullptr){now = root;}
        if (L <= nowleft && nowleft + (1ll<<(log2N-depth))-1 <= R){return now->I;}
        

        if (depth < log2N){
            if (now->left == nullptr){
                now->left = new SegTreeNode;
                now->left->delay = id;
                now->left->I = init_info(nowleft, nowleft + (1ll<<(log2N-depth-1)) - 1);
            }
            if (now->right == nullptr){
                now->right = new SegTreeNode;
                now->right->delay = id;
                now->right->I = init_info(nowleft + (1ll<<(log2N-depth-1)), nowleft + (1<<(log2N-depth)) - 1);
            }
        }
        tell_info(now);
        return operation(range_get(L,R,nowleft,depth+1,now->left), range_get(L,R,nowleft+(1ll<<(log2N-depth-1)),depth+1,now->right));
    }

    /// @brief index閉区間[L,R]に対してFをmappingする。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @param F 適用する写像(アフィン変換ならaとbを持った構造体など)
    void range_update(const int L, const int R, const func &F, int nowleft = 0, int depth = 0, SegTreeNode* now = nullptr){
        if (L > R || L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            return;
        }
        if (nowleft > R || nowleft + (1ll<<(log2N-depth)) <= L){return;}
        if (now == nullptr){now = root;}
        if (L <= nowleft && nowleft + (1ll<<(log2N-depth))-1 <= R){
            now->I = mapping(F, now->I);
            now->delay = composition(F, now->delay);
            return;
        }
        
        if (depth < log2N){
            if (now->left == nullptr){
                now->left = new SegTreeNode;
                now->left->delay = id;
                now->left->I = init_info(nowleft, nowleft + (1ll<<(log2N-depth-1)) - 1);
            }
            if (now->right == nullptr){
                now->right = new SegTreeNode;
                now->right->delay = id;
                now->right->I = init_info(nowleft + (1ll<<(log2N-depth-1)), nowleft + (1<<(log2N-depth)) - 1);
            }
        }
        tell_info(now);
        range_update(L,R,F,nowleft,depth+1,now->left);
        range_update(L,R,F,nowleft+(1ll<<(log2N-depth-1)),depth+1,now->right);
        now->I = operation(now->left->I, now->right->I);
    }
    
    //↓二分探索はめんどかったから書いてない
    // /// @brief 左端をLに固定したとき、条件式Gが成り立つ最小の右端indexを返す。もしなければINF(=2147483647)が返ってくる。
    // /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    // /// @param L 左端
    // /// @param G 判定関数...引数a,bがあり、引数aを動かし、引数bを比較対象tに固定した時に引数aによってtrue,falseが変動する。
    // /// @param t 比較対象のinfo
    // /// @return Gがtrueになる最小右端indexまたは2147483647
    // int min_right(int L, const function<bool(info,info)> &G, const info &t){
    //     info current_result = e;

    //     int ctz_init = L == 0 ? log2N : __builtin_ctz(L);
    //     for (int i = log2N; i > ctz_init; i--){
    //         tell_info(((1<<log2N)+L)>>i);
    //     }

    //     checkpoint:

    //     int ctz = L == 0 ? log2N : __builtin_ctz(L);
    //     tell_info(((1<<log2N)+L)>>ctz);
    //     if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz].first), t)){
    //         if (get_right(((1<<log2N)+L)>>ctz)+1 == 1<<log2N){
    //             return 2147483647;
    //         }
    //         current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz].first);
    //         L = get_right(((1<<log2N)+L)>>ctz)+1;
    //         goto checkpoint;
    //     }

    //     for (int i = ctz-1; i >= 0; i--){
    //         tell_info(((1<<log2N)+L)>>i);
    //         if (!G(operation(current_result, tree[((1<<log2N)+L)>>i].first), t)){
    //             current_result = operation(current_result, tree[((1<<log2N)+L)>>i].first);
    //             L = get_right(((1<<log2N)+L)>>i)+1;
    //             goto checkpoint;
    //         }
    //     }
    //     return L;
    // }
    // /// @brief 右端をRに固定したとき、条件式Gが成り立つ最大の左端indexを返す。もしなければ-INF-1(=-2147483648)が返ってくる。
    // /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。
    // /// @param R 右端 
    // /// @param G 判定関数...引数a,bがあり、引数aを動かし、引数bを比較対象tに固定した時に引数aによってtrue,falseが変動するようなものである。
    // /// @param t 比較対象のinfo
    // /// @return Gがtrueになる最大左端indexまたは-2147483648
    // int max_left(int R, const function<bool(info,info)> &G, const info &t){
    //     info current_result = e;

    //     int cto_init = __builtin_ctz(~R);
    //     for (int i = log2N; i > cto_init; i--){
    //         tell_info(((1<<log2N)+R)>>i);
    //     }

    //     checkpoint:

    //     int cto = __builtin_ctz(~R);//cto...count trailing one
    //     tell_info(((1<<log2N)+R)>>cto);
    //     if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto].first), t)){
    //         if (get_left(((1<<log2N)+R)>>cto) == 0){
    //             return -2147483648;
    //         }
    //         current_result = operation(current_result, tree[((1<<log2N)+R)>>cto].first);
    //         R = get_left(((1<<log2N)+R)>>cto)-1;
    //         goto checkpoint;
    //     }

    //     for (int i = cto-1; i >= 0; i--){
    //         tell_info(((1<<log2N)+R)>>i);
    //         if (!G(operation(current_result, tree[((1<<log2N)+R)>>i].first), t)){
    //             current_result = operation(current_result, tree[((1<<log2N)+R)>>i].first);
    //             R = get_left(((1<<log2N)+R)>>i)-1;
    //             goto checkpoint;
    //         }
    //     }
    //     return R;
    // }

    info operator[](const int t){
        return range_get(t,t);
    }
    
    int size(){
        return max_capacity;
    }
};




#endif /* SEG_TREE__DYNAMIC_LAZY_HPP_ */