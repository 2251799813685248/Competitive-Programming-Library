#include <bits/stdc++.h>
#define ll long long
using namespace std;

/// @brief ある区間内のすべての値xをax+bに変えたり、ある区間の最大値,最小値を取得したりできる。
struct LazySegTree{
    private:
    vector<ll> pow2ll{1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296};
    
    public:
    ll N;//セグメントツリーに載せた要素の数
    ll log2N = 0;//2^iの形で表せるような数のうち、N以上で最小のものに対するiの値

    vector<pair<pair<ll,ll>,tuple<ll,ll,ll>>> tree;//vector({{最大, 最小}, 遅延情報{あるかどうか, 乗算値(a), 加算値(b)}})

    /// @brief コンストラクタ1
    /// @param how_many_node 何個載せるか
    /// @param value 載せるときに全部この値にする
    LazySegTree(ll how_many_node, ll value){
        N = how_many_node;
        while (pow2ll[log2N] < N){
            log2N++;
        }
        tree = vector<pair<pair<ll,ll>,tuple<ll,ll,ll>>>(2*pow2ll[log2N], {{value,value},{0,0,0}});
    }
    
    /// @brief コンストラクタ2
    /// @param how_many_node 何個載せるか
    /// @param A このvectorをそのまま載せる
    LazySegTree(ll how_many_node, vector<ll> &A){
        N = how_many_node;
        while (pow2ll[log2N] < N){
            log2N++;
        }
        tree = vector<pair<pair<ll,ll>,tuple<ll,ll,ll>>>(2*pow2ll[log2N], {{0,0},{0,0,0}});
        for (ll i = 2*pow2ll[log2N]-1; i >= 1; i--){
            if (i >= pow2ll[log2N]){
                tree[i].first = i-pow2ll[log2N] < N ? pair<ll,ll>(A[i-pow2ll[log2N]], A[i-pow2ll[log2N]]) : pair<ll,ll>(-9223372036854775807, 9223372036854775807); //最大値ならLONGLONGMIN、最小値ならLONGLONGMAXなどを入れる。
            }
            else{
                tree[i].first.first = max(tree[2*i].first.first, tree[2*i+1].first.first);
                tree[i].first.second = min(tree[2*i].first.second, tree[2*i+1].first.second);
            }
        }
    }


    bool not_intersect(ll L1, ll R1, ll L2, ll R2){//閉区間が共通部分を持つかを判定
        if (max(R1,R2) - min(L1,L2) > R1-L1+R2-L2){
            return true;
        }
        return false;
    }
    bool completely_covered(ll L1, ll R1, ll L2, ll R2){//閉区間が完全に収まっているかを判定
        return L1 <= L2 && R2 <= R1;
    }
    
    void operate_pair(pair<ll,ll> &p, ll a, ll b){
        p.first = a*p.first + b;
        p.second = a*p.second + b;
        if (a < 0){
            swap(p.first,p.second);
        }
    }
    void merge_operation(tuple<ll,ll,ll> &X, tuple<ll,ll,ll> &Y){
        if (get<0>(X) == 0){
            X = Y;
            return;
        }
        get<1>(X) *= get<1>(Y);
        get<2>(X) *= get<1>(Y);
        get<2>(X) += get<2>(Y);
        return;
    }
    
    /// @brief 遅延伝播をする
    /// @param index_on_tree 
    /// @param depth 
    /// @param now 
    void tell_info(ll index_on_tree, ll depth, pair<ll,ll> &now){
        if (get<0>(tree[index_on_tree].second) == 1){
            if (depth == log2N){
                tree[index_on_tree].second = {0,0,0};
            }
            else{
                ll a = get<1>(tree[index_on_tree].second);
                ll b = get<2>(tree[index_on_tree].second);

                merge_operation(tree[2*index_on_tree].second, tree[index_on_tree].second);
                merge_operation(tree[2*index_on_tree+1].second, tree[index_on_tree].second);
                operate_pair(tree[2*index_on_tree].first, a,b);
                operate_pair(tree[2*index_on_tree+1].first, a,b);

                tree[index_on_tree].second = {0,0,0};
            }
        }
    }

    /// @brief indexにおける閉区間[L,R]での最大値を求める。
    /// @param L 左端
    /// @param R 右端
    /// @param depth 基本的に省略する
    /// @param where 基本的に省略する
    /// @return 最大値
    ll range_max(ll L, ll R, ll depth = 0, ll where = 0){
        pair<ll,ll> now(where*pow2ll[log2N-depth], (where+1)*pow2ll[log2N-depth]-1);//現在の地点の両端を取得
        ll index_on_tree = pow2ll[depth]+where;

        if (not_intersect(L,R,now.first,now.second)){//範囲外なら終わる
            return -9223372036854775807;
        }
        if (completely_covered(L,R,now.first,now.second)){//完全に範囲内なら採用
            return tree[index_on_tree].first.first;
        }
        tell_info(index_on_tree,depth,now);
        return max(range_max(L,R,depth+1,2*where), range_max(L,R,depth+1,2*where+1));//一部だけ範囲内なら分裂して再帰
    }

    /// @brief indexにおける閉区間[L,R]での最小値を求める。
    /// @param L 左端
    /// @param R 右端
    /// @param depth 基本的に省略する
    /// @param where 基本的に省略する
    /// @return 最小値
    ll range_min(ll L, ll R, ll depth = 0, ll where = 0){
        pair<ll,ll> now(where*pow2ll[log2N-depth], (where+1)*pow2ll[log2N-depth]-1);//現在の地点の両端を取得
        ll index_on_tree = pow2ll[depth]+where;

        if (not_intersect(L,R,now.first,now.second)){//範囲外なら終わる
            return 9223372036854775807;
        }
        if (completely_covered(L,R,now.first,now.second)){//完全に範囲内なら採用
            return tree[pow2ll[depth]+where].first.second;
        }
        tell_info(index_on_tree,depth,now);
        return min(range_min(L,R,depth+1,2*where), range_min(L,R,depth+1,2*where+1));//一部だけ範囲内なら分裂して再帰
    }

    /// @brief indexにおける閉区間[L,R]の値を全てをそれぞれa倍してbを加算する。
    /// @param L 左端
    /// @param R 右端
    /// @param a 乗算値
    /// @param b 加算値
    /// @param depth 基本的に省略する
    /// @param where 基本的に省略する
    /// @return ない
    void range_update(ll L, ll R, ll a, ll b, ll depth = 0, ll where = 0){
        pair<ll,ll> now(where*pow2ll[log2N-depth], (where+1)*pow2ll[log2N-depth]-1);
        ll index_on_tree = pow2ll[depth]+where;

        if (not_intersect(L,R,now.first,now.second)){//範囲外なら終わる
            return;
        }
        if (completely_covered(L,R,now.first,now.second)){//完全に範囲内なら採用
            tell_info(index_on_tree,depth,now);
            tree[index_on_tree] = {{tree[index_on_tree].first.first*a+b, tree[index_on_tree].first.second*a+b},{1,a,b}};//遅延情報を記録して更新
            if (a < 0){
                swap(tree[index_on_tree].first.first, tree[index_on_tree].first.second);
            }
            return;
        }

        tell_info(index_on_tree,depth,now);
        range_update(L,R,a,b,depth+1,2*where);//一部だけ範囲内なら分裂して再帰
        range_update(L,R,a,b,depth+1,2*where+1);
        tree[index_on_tree].first.first = max(tree[2*index_on_tree].first.first, tree[2*index_on_tree+1].first.first);
        tree[index_on_tree].first.second = min(tree[2*index_on_tree].first.second, tree[2*index_on_tree+1].first.second);
    }
};