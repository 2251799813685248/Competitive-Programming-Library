#include <vector>
#include <set>
#include <unordered_map>
using namespace std;



struct UnionFind{
    vector<int> A;//根でないとき、どう辿れば根になるか(すでに根なら　-1×(その連結成分の要素数))


    /// @brief 頂点番号が0,1,2...NのUnionFind木を構築する。
    /// @param N 
    UnionFind(int N): A(N+1,-1){}

    /// @brief  二つのノードが同じグループであるかを返す
    /// @param node1 
    /// @param node2 
    /// @return true/false
    bool same_group(int node1, int node2){
        int root1 = node1;
        while (A[root1] >= 0){//ノードを根までたどる
            root1 = A[root1];
        }
        int root2 = node2;
        while (A[root2] >= 0){//たどる
            root2 = A[root2];
        }

        int finalroot1 = root1;//最終的な根を保持する。
        int finalroot2 = root2;
        
        root1 = node1;
        while (A[root1] >= 0){//経路圧縮
            root1 = A[root1];
            A[node1] = finalroot1;
            node1 = root1;
        }
        root2 = node2;
        while (A[root2] >= 0){//経路圧縮
            root2 = A[root2];
            A[node2] = finalroot2;
            node2 = root2;
        }

        return root1 == root2;//判定
    }

    /// @brief node1を含むグループの要素数を返す。
    /// @param node1 
    /// @return 
    int howmanynodes(int node1){
        int root1 = node1;
        while (A[root1] >= 0){//ノードを根までたどる
            root1 = A[root1];
        }
        return -A[root1];

        int finalroot1 = root1;

        root1 = node1;
        while (A[root1] >= 0){//経路圧縮
            root1 = A[root1];
            A[node1] = finalroot1;
            node1 = root1;
        }
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。
    /// @param node1 
    /// @param node2 
    void merge(int node1, int node2){
        if (same_group(node1, node2)){
            return;
        }
        int root1 = node1;
        while (A[root1] >= 0){//ノードを根までたどる
            root1 = A[root1];
        }
        int root2 = node2;
        while (A[root2] >= 0){//たどる
            root2 = A[root2];
        }
        
        if (howmanynodes(root1) > howmanynodes(root2)){
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