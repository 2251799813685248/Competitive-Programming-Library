/// @brief 抽象化セグメントツリー
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
/// @param mapping infoに対してfuncを作用させた結果を返す関数(アフィン変換ならx -> ax+b)
template<typename info, typename func>
struct SegTree{

    int log2N;//セグ木の高さ+1

    info e;///単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    function<info(func,info)> mapping;//更新を行うとどうなるか？(アフィン変換ならx -> ax+b)

    /// @brief セグ木を扱うためのノード
    struct SegNode{
        pair<int,int> range;//自身のカバーする範囲
        info I;//自身が持っている情報
        SegNode(const int &L, const int &R, const info &eee) : range({L,R}), I(eee){}
        SegNode(){};//空コンストラクタ
    };

    vector<SegNode> tree;//セグ木本体

    /// @brief N個のIで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    SegTree(info I, int N, info eee, function<info(info,info)> op, function<info(func,info)> m){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;

        //セグ木のサイズを決定
        log2N = 0;
        while ((1<<log2N) < N){
            log2N++;
        }

        //セグ木に実際に乗せるvectorを構築
        tree = vector<SegNode>(1<<(log2N+1));
        for (int i = 0; i < N; i++){
            tree[i+(1<<log2N)] = SegNode(i,i,I);
        }
        for (int i = N; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = SegNode(i,i,e);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = SegNode(tree[2*i].range.first,tree[2*i+1].range.second,operation(tree[2*i].I, tree[2*i+1].I));
        }
    }
    /// @brief vector Aで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    SegTree(vector<info> &A, info eee, function<info(info,info)> op, function<info(func,info)> m){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;

        //セグ木のサイズを決定
        log2N = 0;
        int sz = A.size();
        while ((1ULL<<log2N) < A.size()){
            log2N++;
        }

        //セグ木に実際に乗せるvectorを構築
        tree = vector<SegNode>(1<<(log2N+1));
        for (int i = 0; i < sz; i++){
            tree[i+(1<<log2N)] = SegNode(i,i,A[i]);
        }
        for (int i = sz; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = SegNode(i,i,e);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = SegNode(tree[2*i].range.first,tree[2*i+1].range.second,operation(tree[2*i].I, tree[2*i+1].I));
        }
    }

    /// @brief 区間の共通部分が空集合であるか判定
    /// @return range1 ⋀ {a,a} == Ø
    private: bool not_intersect(const pair<int,int> &range1, const int &a, const int &b){
        return max(range1.second,b)-min(range1.first,a) > range1.second+b-range1.first-a;
    }
    /// @brief range1がrange2に完全に覆われているかを判定
    /// @return range1 ⊂ {a,b}
    private: bool completely_covered(const pair<int,int> &range1, const int &a, const int &b){
        return a <= range1.first && range1.second <= b;
    }

    public:



    /// @brief index閉区間[L,R]において、集計を行う。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @return [L,R]での集計結果
    info range_get(const int &L, const int &R){
        info ret = e;
        int left = L;
        while (left < R+1){
            int log2interval = min(__builtin_ctz(left),31-__builtin_clz(R+1-left));
            ret = operation(ret, tree[(left+(1<<log2N))>>log2interval].I);
            left += 1<<log2interval;
        }
        return ret;
        //↓再帰だったもの
        //if (not_intersect(tree[index_on_tree].range, L, R)){
        //    return e;
        //}
        //if (completely_covered(tree[index_on_tree].range, L, R)){
        //    return tree[index_on_tree].I;
        //}
        //return operation(range_get(L,R,2*index_on_tree), range_get(L,R,2*index_on_tree+1));
    }

    /// @brief index tに対してFをmappingする。
    /// @param L index
    /// @param F 適用する写像(アフィン変換ならaとbを持った構造体など)
    void pointwise_update(const int &t, const func &F){
        int start_index = t + (1<<log2N);
        tree[start_index].I = mapping(F,tree[start_index].I);
        start_index >>= 1;
        while (start_index){
            tree[start_index].I = operation(tree[2*start_index].I,tree[2*start_index+1].I);
            start_index >>= 1;
        }
        //↓再帰だったもの
        //if (not_intersect(tree[index_on_tree].range, t,t)){
        //    return;
        //}
        //if (tree[index_on_tree].bottom && t == tree[index_on_tree].range.first){
        //    tree[index_on_tree].I = mapping(F, tree[index_on_tree].I);
        //    return;
        //}
        //pointwise_update(t,F,2*index_on_tree);
        //pointwise_update(t,F,2*index_on_tree+1);
        //tree[index_on_tree].I = operation(tree[2*index_on_tree].I, tree[2*index_on_tree+1].I);
    }


    /// @brief 左端をLに固定したとき、条件式Gが成り立つ最小の右端indexを返す。もしなければINF(=2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...引数a,bがあり、引数aを動かし、引数bを比較対象tに固定した時に引数aによってtrue,falseが変動する。
    /// @param t 比較対象
    /// @return Gがtrueになる最小右端indexまたは2147483647
    int min_right(int L, const function<bool(info,info)> &G, const info &t){

        info current_result = e;

        checkpoint:

        int ctz = L == 0 ? log2N : __builtin_ctz(L);
        if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz].I), t)){
            if (tree[((1<<log2N)+L)>>ctz].range.second+1 == 1<<log2N){
                return 2147483647;
            }
            current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz].I);
            L = tree[((1<<log2N)+L)>>ctz].range.second+1;
            goto checkpoint;
        }

        for (int i = ctz-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+L)>>i].I), t)){
                current_result = operation(current_result, tree[((1<<log2N)+L)>>i].I);
                L = tree[((1<<log2N)+L)>>i].range.second+1;
                goto checkpoint;
            }
        }
        return L;
    }
    /// @brief 右端をRに固定したとき、条件式Gが成り立つ最大の左端indexを返す。もしなければ-INF-1(=-2147483648)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。
    /// @param R 右端 
    /// @param G 判定関数...引数a,bがあり、引数aを動かし、引数bを比較対象tに固定した時に引数aによってtrue,falseが変動するようなものである。
    /// @param t 比較対象
    /// @return Gがtrueになる最大左端index
    int max_left(int R, const function<bool(info,info)> &G, const info &t){
        
        info current_result = e;

        checkpoint:

        int cto = __builtin_ctz(~R);//cto...count trailing one
        if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto].I), t)){
            if (tree[((1<<log2N)+R)>>cto].range.first == 0){
                return -2147483648;
            }
            current_result = operation(current_result, tree[((1<<log2N)+R)>>cto].I);
            R = tree[((1<<log2N)+R)>>cto].range.first-1;
            goto checkpoint;
        }

        for (int i = cto-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+R)>>i].I), t)){
                current_result = operation(current_result, tree[((1<<log2N)+R)>>i].I);
                R = tree[((1<<log2N)+R)>>i].range.first-1;
                goto checkpoint;
            }
        }
        return R;
    }
};