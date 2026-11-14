#ifndef GRAPH__HPP_
#define GRAPH__HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <deque.hpp>
#include <priority_deque.hpp>
#include <iostream>
#include <list.hpp>
#include <utility>
#define vall(A) A.begin(), A.end()
using namespace std;
using ll = long long;
using pii = array<int, 2>;
using pll = array<ll, 2>;

// Graph.hpp
struct NoWeightGraph {
    int N;                  // 頂点数
    list<list<pii>> E;      // 隣接頂点リスト(重みなし)
    list<array<ll, 3>> E2;  // 辺番号による辺の管理
    int M = 0;              // 辺の本数

    /// @brief 頂点数を指定して辺のないグラフを構築する。頂点は0-indexedである。
    /// @param vertex_num
    NoWeightGraph(int vertex_num = 0) {
        N = vertex_num;
        E = list<list<pii>>(N);
    }
    // /// @brief 事前に作った隣接頂点リストでグラフを構築する。
    // /// @param E
    // NoWeightGraph(const list<list<int>> &E_init){
    //     N = E_init.size();
    //     E = E_init;
    // }

    // 頂点数を変更する。
    void resize(int new_size) {
        N = new_size;
        E.resize(new_size);
    }
    // 辺を追加する。辺にはそれぞれ一意な番号を振らなければならない。省略すれば自動で振ってくれる。
    void add_undirected_edge(int u, int v, int id = -2147483647) {
        if (id == -2147483647) {
            id = M;
        }
        E[u].append({v, id});
        E[v].append({u, id});
        E2.append({u, v, id});
        M++;
    }
    void add_directed_edge(int u, int v, int id = -2147483647) {
        if (id == -2147483647) {
            id = M;
        }
        E[u].append({v, id});
        E2.append({u, v, id});
        M++;
    }

    /// @brief 始点集合を指定し、そこからの距離のリストを作成する。
    /// @param startpoints
    /// @return
    list<int> BFS_distance_array(const list<int>& startpoints) {
        deque2<int> q;
        list<int> dist(N, -1);

        for (auto& v : startpoints) {
            dist[v] = 0;
            q.append(v);
        }

        while (!q.empty()) {
            int now = q.front();
            q.pop();
            for (auto v : E[now]) {
                if (dist[v[0]] == -1) {
                    dist[v[0]] = dist[now] + 1;
                    q.append(v[0]);
                }
            }
        }
        return dist;
    }
    /// @brief 始点集合を指定し、それを始点とする経路復元のための情報を作成する。
    /// @param startpoints
    /// @return
    list<pii> BFS_path_construction_info(const list<int>& startpoints) {
        deque2<int> q;
        list<pii> previous_vertex(N, {-1, -1});  //-1...未確定、0以上...確定

        for (auto& v : startpoints) {
            previous_vertex[v] = {0, v};
            q.append(v);
        }

        while (!q.empty()) {
            int now = q.front();
            q.pop();
            for (auto v : E[now]) {
                if (previous_vertex[v[0]][1] == -1) {
                    previous_vertex[v[0]] = {previous_vertex[now][0] + 1, now};
                    q.append(v[0]);
                }
            }
        }
        return previous_vertex;
    }

    /// @brief 頂点gにたどり着くためのパスを求める。ないなら空のリストが返ってくる
    /// @param g
    /// @return
    list<int> path(int g, const list<pii>& path_info) {
        if (path_info[g][1] == -1) {
            return {};
        }
        list<int> res;
        while (path_info[g][1] != g) {
            res.append(g);
            g = path_info[g][1];
        }
        res.append(g);
        reverse(vall(res));
        return res;
    }

    // サイクルがあるかを判定し、あるならば一つ見つけて復元する。もしサイクルがなければ全て空のリストが返ってくる。
    //  {頂点番号, 辺番号}
    pair<list<int>, list<int>> cycle_detection() {
        list<bool> used(N), visiting(N);
        list<int> origin(N, -1);  // ある頂点に到達したとき、何のidの辺を通過して侵入したか。
        deque2<int> q;
        for (int i = 0; i < N; i++) {
            if (used[i]) {
                continue;
            }
            bool cycle_found = false;
            q.append(~i);
            q.append(i);
            while (!q.empty()) {
                int now = q.back();
                q.pop();
                if (now < 0) {
                    visiting[~now] = false;
                    origin[~now] = -1;
                    continue;
                }
                if (used[now]) {
                    continue;
                }
                used[now] = true;
                visiting[now] = true;
                for (auto& w : E[now]) {
                    if (origin[now] == w[1]) {
                        continue;
                    }  // 同一idの辺を逆順に辿るのは禁止
                    origin[w[0]] = w[1];
                    q.append(~w[0]);
                    if (visiting[w[0]]) {
                        cycle_found = true;
                        break;
                    }
                    q.append(w[0]);
                }
                if (cycle_found) {
                    break;
                }
            }
            if (cycle_found) {
                list<int> cycle_edge_id, cycle_vertex;
                int start = ~q.back();
                cycle_edge_id.append(origin[start]);
                q.pop();
                while (true) {
                    int now = q.back();
                    q.pop();
                    if (now >= 0) {
                        continue;
                    }
                    now = ~now;
                    if (!visiting[now]) {
                        continue;
                    }
                    if (now == start) {
                        break;
                    }
                    cycle_edge_id.append(origin[now]);
                    cycle_vertex.append(now);
                    origin[now] = -1;
                    visiting[now] = false;
                }
                cycle_vertex.append(start);
                reverse(vall(cycle_vertex));
                reverse(vall(cycle_edge_id));
                return make_pair(cycle_vertex, cycle_edge_id);
            }
        }
        return {{}, {}};
    }

    // void scc_dfs(list<list<ll>> &E, ll now, list<bool> &used, ll &temp,
    // list<pair<ll,ll>> &order){
    //     for (auto v : E[now]){
    //         if (used[v]){continue;}
    //         used[v] = true;
    //         scc_dfs(E,v,used,temp,order);
    //     }
    //     order[now].second = temp;
    //     temp++;
    // }

    // list<list<ll>> SCC(){
    //     list<list<ll>> Edge_inverse(N+1);//辺が逆のグラフ
    //     for (int i = 1; i <= N; i++){
    //         for (auto v : Edge[i]){
    //             Edge_inverse[v].append(i);
    //         }
    //     }

    //     ll temp = 1;
    //     list<bool> used(N+1,0);
    //     list<pair<ll,ll>> order(N+1);

    //     for (int i = 1; i <= N; i++){
    //         order[i].first = i;
    //     }
    //     for (int i = 1; i <= N; i++){
    //         if (!used[i]){
    //             used[i] = true;
    //             scc_dfs(Edge,i,used,temp,order);//一回目のdfsで、帰った順番を記録
    //         }
    //     }

    //     sort(vall(order), [](const pair<ll,ll> &a, const pair<ll,ll> &b){return
    //     a.second > b.second;}); order.pop_back();

    //     temp = 1;
    //     list<ll> groups(N+1,-1);
    //     for (int i = 0; i < N; i++){
    //         if (groups[order[i].first] ==
    //         -1){//2回目のBFSで連結成分ごとに分解して、番号をつける
    //             queue<ll> Q;
    //             Q.push(order[i].first);
    //             groups[order[i].first] = temp;
    //             while (!Q.empty()){
    //                 ll now = Q.front();
    //                 Q.pop();
    //                 for (auto v : Edge_inverse[now]){
    //                     if (groups[v] == -1){
    //                         groups[v] = temp;
    //                         Q.push(v);
    //                     }
    //                 }
    //             }
    //             temp++;
    //         }
    //     }

    //     list<list<ll>> small_graph(temp);//縮約されたグラフを構築
    //     for (int i = 1; i <= N; i++){
    //         for (auto v : Edge[i]){
    //             if (groups[i] != groups[v]){
    //                 small_graph[groups[i]].append(groups[v]);
    //             }
    //         }
    //     }

    //     list<ll> into_count(temp,0);//入ってくる辺の数を管理
    //     priority_queue<ll,list<ll>,greater<ll>>
    //     pQ;//入ってくる辺がないような頂点を管理 list<ll> topological_sort;

    //     for (auto &vec : small_graph){
    //         sort(vall(vec));
    //         vec.erase(unique(vall(vec)),vec.end());
    //         for (auto v : vec){
    //             into_count[v]++;
    //         }
    //     }

    //     for (int i = 1; i < temp; i++){
    //         if (into_count[i] == 0){
    //             pQ.push(i);
    //         }
    //     }

    //     while (!pQ.empty()){
    //         ll now = pQ.top();
    //         pQ.pop();
    //         topological_sort.append(now);
    //         for (auto v : small_graph[now]){
    //             into_count[v]--;
    //             if (into_count[v] == 0){
    //                 pQ.push(v);
    //             }
    //         }
    //     }

    //     list<ll> inv_topological_sort(temp);
    //     for (int i = 1; i < temp; i++){
    //         inv_topological_sort[topological_sort[i-1]] = i;
    //     }
    //     list<list<ll>> ans(temp-1);
    //     for (int i = 1; i <= N; i++){
    //         ans[inv_topological_sort[groups[i]]-1].append(i);
    //     }

    //     return ans;
    // }
};


template<typename costT>
struct MaxFlowGraph {
    int N;
    list<>
};


struct WeightedGraph {
    ll N;
    list<list<pll>> Edge;
    list<ll> D;  // 直近のDijkstraで求めた距離リスト

    /// @brief 頂点,辺数を指定して自動で入力を受け取ってグラフを構築する
    /// @param vertex_num
    /// @param edge_num
    /// @param directional
    WeightedGraph(ll vertex_num, ll edge_num, bool directional = false) {
        N = vertex_num;
        Edge = list<list<pll>>(N + 1);
        for (int i = 0; i < edge_num; i++) {
            ll u, v, w;
            cin >> u >> v >> w;
            Edge[u].append({v, w});
            if (!directional) {
                Edge[v].append({u, w});
            }
        }
    }
    /// @brief 頂点数を指定して辺のないグラフを構築
    /// @param vertex_num
    WeightedGraph(ll vertex_num) {
        N = vertex_num;
        Edge = list<list<pll>>(N + 1);
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築
    /// @param E
    WeightedGraph(const list<list<pll>>& E) {
        N = E.size() - 1;
        Edge = E;
    }

    void add_edge(ll u, ll v, ll w, bool directional = false) {
        Edge[u].append({v, w});
        if (!directional) {
            Edge[v].append({v, w});
        }
    }

    list<ll> Djikstra(const list<ll>& startpoints) {
        list<bool> visited(N + 1, 0);
        D = list<ll>(N + 1, 9223372036854775807);

        priority_deque<pll> pQ;
        for (auto v : startpoints) {
            pQ.push({0, v});
        }

        while (!pQ.empty()) {
            auto [dist, now] = pQ.pop_min();
            if (visited[now]) {
                continue;
            }
            visited[now] = 1;
            D[now] = dist;
            for (auto v : Edge[now]) {
                if (visited[v[0]]) {
                    continue;
                }
                pQ.push({dist + v[1], v[0]});
            }
        }
        return D;
    }
};

#endif /* GRAPH__HPP_ */