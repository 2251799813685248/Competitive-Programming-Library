#ifndef TREE__HPP_
#define TREE__HPP_

#include <vector>
#include <queue>
#include <algorithm>
#include <array>
using namespace std;
using pii = array<int,2>;

/// @brief 与えられた木の重心を1つ見つける 
template<typename INT> int tree_centroid(int N, const vector<vector<INT>>& E){
    if (N == 1){return 1;}
    vector<bool> used(N+1, 0);
    vector<int> subtreesize(N+1, 0);
    deque<int> st;
    st.push_back(~1);
    st.push_back(1);
    used[1] = true;
    while (!st.empty()){
        int n = st.back();
        st.pop_back();
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
            st.push_back(~w);
            st.push_back(w);
        }
    }
    assert(false);
    return -1;
}

/// @brief 与えられた木を指定した頂点で分解する。{{頂点数, 根(分解した頂点のすぐ隣の頂点)}, 隣接頂点リスト}の形で返ってくる
template<typename INT> vector<pair<pair<int, int>, vector<vector<INT>>>> tree_decomposition(int N, const vector<vector<INT>>& E, int centroid){
    vector<pair<pair<int, int>, vector<vector<INT>>>> res;
    vector<bool> used(N+1, 0);
    deque<int> st;
    used[centroid] = true;
    for (INT subtreeroot : E[centroid]){
        int subtreesize = 1;
        vector<pii> edges;
        vector<int> vertex_list;
        st.push_back(subtreeroot);
        used[subtreeroot] = 1;
        vertex_list.push_back(-2147483647);
        vertex_list.push_back(subtreeroot);
        while (!st.empty()){
            int n = st.back();
            st.pop_back();
            for (INT w : E[n]){
                if (used[w]){continue;}
                subtreesize++;
                vertex_list.push_back(w);
                used[w] = true;
                edges.push_back({n,(int)w});
                st.push_back(w);
            }
        }
        sort(vertex_list.begin(), vertex_list.end());
        res.emplace_back(make_pair(subtreesize, (int)(lower_bound(vertex_list.begin(), vertex_list.end(), subtreeroot)-vertex_list.begin())), vector<vector<INT>>(subtreesize+1));
        for (pii& e : edges){
            int u = lower_bound(vertex_list.begin(), vertex_list.end(), e[0]) - vertex_list.begin();
            int v = lower_bound(vertex_list.begin(), vertex_list.end(), e[1]) - vertex_list.begin();
            res.back().second[u].push_back(v);
            res.back().second[v].push_back(u);
        }
    }
    return res;
}


#endif /* TREE__HPP_ */