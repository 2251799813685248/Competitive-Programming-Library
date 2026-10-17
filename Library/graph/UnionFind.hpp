#ifndef UNION_FIND_HPP_
#define UNION_FIND_HPP_

#include <list.hpp>
using namespace std;



//UnionFind.hpp
/// @brief UnionFind木
struct UnionFind{
    list<int> A;//根でないとき、どう辿れば根になるか(すでに根なら-1×(要素数))
    list<int> P;//連結成分管理用
    int groups;//連結成分数


    /// @brief 頂点番号が0,1,2...N-1のUnionFind木を構築する。全部同一の状態で初期化される
    /// @param N 頂点数の上限
    UnionFind(const int N): A(N,-1), P(N), groups(N){
        for (int i = 0; i < N; i++){P[i] = i;}
    }

    /// @brief nodeの親を見つける
    /// @param node 
    /// @return root
    inline int findroot(int node) const {
        while (A[node] >= 0){
            node = A[node];
        }
        return node;
    }
    /// @brief node以上のノードをすべてrootに直接接続する
    /// @param node 
    /// @param root 
    inline void compress_path(int node, const int root){
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

        return root1 == root2;
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。合成後の根が返される。
    /// @param node1 
    /// @param node2 
    int merge(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);

        if (root1 == root2){
            return root1;
        }

        groups--;
        swap(P[node1], P[node2]);
        
        if (-A[root1] > -A[root2]){
            A[root1] += A[root2];
            A[root2] = root1;
            return root1;
        }
        else{
            A[root2] += A[root1];
            A[root1] = root2;
            return root2;
        }
    }

    list<int> enumerate_cc(int node){
        list<int> res;
        res.append(node);
        int n = P[node];
        while (n != node){
            res.append(n);
            n = P[n];
        }
        return res;
    }
    list<list<int>> enumerate_all_cc(){
        list<list<int>> res_all;
        list<bool> used(A.size(), false);
        for (int i = 0; i < ssize(A); i++){
            if (used[i]){continue;}
            list<int> res;
            res.append(i);
            used[i] = true;
            int n = P[i];
            while (n != i){
                res.append(n);
                used[n] = true;
                n = P[n];
            }
            res_all.append(std::move(res));
        }
        return res_all;
    }
};




#endif /* UNION_FIND_HPP_ */