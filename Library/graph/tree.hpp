#ifndef TREE__HPP_
#define TREE__HPP_

#include <algorithm>
#include <array>
#include <cassert>
#include <deque.hpp>
#include <functional>
#include <list.hpp>
#include <utility>
using namespace std;
using pii = array<int,2>;

/// @brief 与えられた木の重心を1つ見つける 
template<typename INT> int tree_centroid(int N, const list<list<INT>>& E){
    if (N == 1){return 1;}
    list<bool> used(N+1, 0);
    list<int> subtreesize(N+1, 0);
    deque2<int> st;
    st.append(~1);
    st.append(1);
    used[1] = true;
    while (!st.empty()){
        int n = st.pop();
        if (n < 0){
            n = ~n;
            subtreesize[n] = 1;
            bool is_centroid = true;
            for (INT w : E[n]){
                if (subtreesize[w] > N/2){is_centroid = false;}
                subtreesize[n] += subtreesize[w];
            }
            if (is_centroid && N-subtreesize[n] <= N/2){return n;}
            continue;
        }
        for (INT w : E[n]){
            if (used[w]){continue;}
            used[w] = true;
            st.append(~w);
            st.append(w);
        }
    }
    assert(false);
    return -1;
}

/// @brief 与えられた木を指定した頂点で分解する。{{頂点数, 根(分解した頂点のすぐ隣の頂点)}, 隣接頂点リスト}の形で返ってくる
template<typename INT> list<pair<pair<int, int>, list<list<INT>>>> tree_decomposition(int N, const list<list<INT>>& E, int centroid){
    list<pair<pair<int, int>, list<list<INT>>>> res;
    list<bool> used(N+1, 0);
    deque2<int> st;
    used[centroid] = true;
    for (INT subtreeroot : E[centroid]){
        int subtreesize = 1;
        list<pii> edges;
        list<int> vertex_list;
        st.append(subtreeroot);
        used[subtreeroot] = 1;
        vertex_list.append(-2147483647);
        vertex_list.append(subtreeroot);
        while (!st.empty()){
            int n = st.pop();
            for (INT w : E[n]){
                if (used[w]){continue;}
                subtreesize++;
                vertex_list.append(w);
                used[w] = true;
                edges.append({n,(int)w});
                st.append(w);
            }
        }
        sort(vertex_list.begin(), vertex_list.end());
        res.append(make_pair(make_pair(subtreesize, (int)(lower_bound(vertex_list.begin(), vertex_list.end(), subtreeroot)-vertex_list.begin())), list<list<INT>>(subtreesize+1)));
        for (pii& e : edges){
            int u = lower_bound(vertex_list.begin(), vertex_list.end(), e[0]) - vertex_list.begin();
            int v = lower_bound(vertex_list.begin(), vertex_list.end(), e[1]) - vertex_list.begin();
            res.back().second[u].append(v);
            res.back().second[v].append(u);
        }
    }
    return res;
}

template <typename INT>
pair<list<INT>, list<pair<INT, list<INT>>>> subtree_size(int N, const list<list<INT>>& E, int root) {
    list<bool> used(N, false);
    list<INT> res(N, 0);
    list<pair<INT, list<INT>>> res2(N, {static_cast<INT>(-1), list<INT>()});
    function<INT(INT)> dfs = [&](INT n){
        used[n] = true;
        for (auto& w : E[n]){
            if (used[w]){continue;}
            res2[w].first = n;
            res2[n].second.append(w);
            res[n] += dfs(w);
        }
        res[n] += 1;
        return res[n];
    };
    used[root] = true;
    res[root] = dfs(root);
    return make_pair(res, res2);
}



#endif /* TREE__HPP_ */