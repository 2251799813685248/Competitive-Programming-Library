#include <vector>
using namespace std;
#define ll long long
struct UnionFind{
    vector<ll> A;//根でないとき、どう辿れば根になるか(すでに根なら-1)
    vector<ll> HowMany;

    UnionFind(ll N){
        for (ll i = 1; i <= N+1; i++){
            A.push_back(-1);
            HowMany.push_back(1);
        }
    }

    /// @brief  二つのノードが同じグループであるかを返す
    /// @param node1 
    /// @param node2 
    /// @return 
    bool same_group(ll node1, ll node2){
        ll root1 = node1;
        while (A[root1] != -1){//ノードを根までたどる
            root1 = A[root1];
        }
        ll root2 = node2;
        while (A[root2] != -1){//たどる
            root2 = A[root2];
        }
        return root1 == root2;//判定

        ll finalroot1 = root1;
        ll finalroot2 = root2;

        root1 = node1;
        while (A[root1] != -1){//経路圧縮
            root1 = A[root1];
            A[node1] = finalroot1;
            node1 = root1;
        }
        root2 = node2;
        while (A[root2] != -1){//経路圧縮
            root2 = A[root2];
            A[node2] = finalroot2;
            node2 = root2;
        }
    }

    /// @brief node1を含むグループの要素数を返す。
    /// @param node1 
    /// @return 
    ll howmanynodes(ll node1){
        ll root1 = node1;
        while (A[root1] != -1){//ノードを根までたどる
            root1 = A[root1];
        }
        return HowMany[root1];

        ll finalroot1 = root1;

        root1 = node1;
        while (A[root1] != -1){//経路圧縮
            root1 = A[root1];
            A[node1] = finalroot1;
            node1 = root1;
        }
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。
    /// @param node1 
    /// @param node2 
    void merge(ll node1, ll node2){
        if (same_group(node1, node2)){
            return;
        }
        ll root1 = node1;
        while (A[root1] != -1){//ノードを根までたどる
            root1 = A[root1];
        }
        ll root2 = node2;
        while (A[root2] != -1){//たどる
            root2 = A[root2];
        }
        
        if (howmanynodes(root1) > howmanynodes(root2)){
            A[root2] = root1;
            HowMany[root1] += HowMany[root2];
        }
        else{
            A[root1] = root2;
            HowMany[root2] += HowMany[root1];
        }
    }
};