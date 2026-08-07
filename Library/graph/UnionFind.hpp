#ifndef UNION_FIND_HPP_
#define UNION_FIND_HPP_

#include <vector>
#include <functional>
#include <numeric>
using namespace std;



//UnionFind.hpp
/// @brief UnionFind木
/// @tparam nodeinfo 
template<typename nodeinfo>
struct UnionFind{
    vector<int> A;//根でないとき、どう辿れば根になるか(すでに根なら-1×(要素数))
    vector<int> P;//連結成分管理用
    int groups;//連結成分数
    
    vector<nodeinfo> B;//各根に載っている状態を保存する。
    function<void(nodeinfo&, nodeinfo, int, int)> merge_info;//異なる連結成分をマージするときの関数{マージ先, マージ元, マージ先代表元, マージ元代表元}
    function<void(nodeinfo&, int, int)> modify_info;//同一連結成分内に対する操作を行う関数{変更される要素, それの代表元2つ}


    /// @brief 頂点番号が(0,)1,2...N-1のUnionFind木を構築する。全部同一の状態で初期化される
    /// @param N 頂点数の上限
    /// @param e 
    /// @param one_indexed 1-indexedかどうか
    UnionFind(const int N,
        const nodeinfo &init,
        function<void(nodeinfo&, nodeinfo, int, int)> mergefunc,
        function<void(nodeinfo&, int, int)> modifyfunc):
            A(N,-1),
            P([N](){vector<int> v(N); iota(v.begin(), v.end(), 0); return v;}),
            groups(N),
            B(N, init),
            merge_info(mergefunc),
            modify_info(modifyfunc)
    {}

    /// @brief nodeの親を見つける
    /// @param node 
    /// @return root
    int findroot(int node) const {
        while (A[node] >= 0){
            node = A[node];
        }
        return node;
    }
    /// @brief node以上のノードをすべてrootに直接接続する
    /// @param node 
    /// @param root 
    void compress_path(int node, const int root){
        int temp = node;
        while (A[temp] >= 0){
            temp = A[temp];
            A[node] = root;
            node = temp;
        }
    }

    /// @brief  二つのノードが同じグループであるかを返す
    /// @param node1 
    /// @param node2 
    /// @return true/false
    bool same_group(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);
        
        compress_path(node1, root1);
        compress_path(node2, root2);

        return root1 == root2;//判定
    }

    /// @brief node1を含むグループの根が持っている情報を返す。
    /// @param node1 
    /// @return 
    nodeinfo getinfo(int node1){
        int root1 = findroot(node1);
        compress_path(node1, root1);
        return B[root1];
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。
    /// @param node1 
    /// @param node2 
    void merge(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);

        if (root1 == root2){
            modify_info(B[root1], node1, node2);
            return;
        }

        groups--;
        swap(P[node1], P[node2]);
        
        if (-A[root1] > -A[root2]){
            A[root1] += A[root2];
            A[root2] = root1;
            merge_info(B[root1], B[root2], node1, node2);
        }
        else{
            A[root2] += A[root1];
            A[root1] = root2;
            merge_info(B[root2], B[root1], node2, node1);
        }
    }

    vector<int> enumerate_cc(int node){
        vector<int> res;
        res.push_back(node);
        int n = P[node];
        while (n != node){
            res.push_back(n);
            n = P[n];
        }
        return res;
    }
    vector<vector<int>> enumerate_all_cc(){
        vector<vector<int>> res_all;
        vector<bool> used(A.size(), false);
        for (unsigned i = 0; i < A.size(); i++){
            if (used[i]){continue;}
            vector<int> res;
            res.push_back(i);
            used[i] = true;
            int n = P[i];
            while (n != i){
                res.push_back(n);
                used[n] = true;
                n = P[n];
            }
            res_all.push_back(std::move(res));
        }
        return res_all;
    }
};




#endif /* UNION_FIND_HPP_ */