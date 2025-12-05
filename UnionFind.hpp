#include <vector>
#include <set>
#include <unordered_map>
using namespace std;



struct UnionFind{
    vector<int> A;//根でないとき、どう辿れば根になるか(すでに根なら　-1×(その連結成分の要素数))
    int groups;

    /// @brief 頂点番号が0,1,2...NのUnionFind木を構築する。
    /// @param N 
    /// @param one_indexed 1-indexedかどうか
    UnionFind(int N, bool one_indexed = true): A(N+1,-1), groups(one_indexed ? N : N+1){}

    /// @brief nodeの親を見つける
    /// @param node 
    /// @return root
    int findroot(int node){
        while (A[node] >= 0){
            node = A[node];
        }
        return node;
    }
    /// @brief node以上のノードをすべてrootに直接接続する
    /// @param node 
    /// @param root 
    void compress_path(int node, const int &root){
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

    /// @brief node1を含むグループの要素数を返す。
    /// @param node1 
    /// @return 
    int howmanynodes(int node1){
        int root1 = findroot(node1);
        compress_path(node1,root1);
        return -A[root1];
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。
    /// @param node1 
    /// @param node2 
    void merge(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);

        if (root1 == root2){
            return;
        }

        groups--;
        
        if (-A[root1] > -A[root2]){
            A[root1] += A[root2];
            A[root2] = root1;
        }
        else{
            A[root2] += A[root1];
            A[root1] = root2;
        }
    }

    /// @brief 連結成分ごとに分解し、各成分に属する頂点をまとめたリストを作成する。
    /// @param one_indexed 1-indexedかどうか(デフォルトで1-indexed)
    /// @return 
    vector<vector<int>> connected_groups(bool one_indexed = true){
        unordered_map<int,vector<int>> B;//仮の答え保存用
        vector<int> C(A.size(),-1);//どの連結成分にいるかを管理

        for (int i = one_indexed, sz = A.size(); i <= sz-1; i++){
            if (A[i] < 0){
                B[i].push_back(i);
                C[i] = i;
            }
        }

        vector<int> passed_node;//たどっている途中の頂点を保持するスタック

        for (int i = one_indexed, sz = A.size(); i <= sz-1; i++){
            if (C[i] != -1){
                continue;
            }
            int temp = i;
            while (C[temp] == -1){
                passed_node.push_back(temp);
                temp = A[temp];
            }
            vector<int> &temp2 = B[C[temp]];
            while (!passed_node.empty()){
                temp2.push_back(passed_node.back());
                C[passed_node.back()] = C[temp];
                passed_node.pop_back();
            }
        }
        vector<vector<int>> ret;
        for (auto &pvec : B){
            ret.push_back(pvec.second);
        }
        return ret;
    }
};
