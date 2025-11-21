#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#define INF 2251799813685248

using namespace std;
#define ll long long
#define ld long double
#define vall(A) A.begin(),A.end()


struct NoWeightGraph{
    ll N;//頂点数
    vector<vector<ll>> Edge;//隣接頂点リスト(重みなし)
    vector<ll> D;


    /// @brief 頂点,辺数を指定して自動で入力を受け取ってグラフを構築する
    /// @param vertex_num 
    /// @param edge_num 
    /// @param directional 
    NoWeightGraph(ll vertex_num, ll edge_num, bool directional = false){
        N = vertex_num;
        Edge = vector<vector<ll>>(N+1);
        for (ll i = 0; i < edge_num; i++){
            ll u,v;
            cin >> u >> v;
            Edge[u].push_back(v);
            if (!directional){
                Edge[v].push_back(u);
            }
        }
    }
    /// @brief 頂点数を指定して辺のないグラフを構築
    /// @param vertex_num 
    NoWeightGraph(ll vertex_num){
        N = vertex_num;
        Edge = vector<vector<ll>>(N+1);
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築
    /// @param E 
    NoWeightGraph(const vector<vector<ll>> &E){
        N = E.size()-1;
        Edge = E;
    }

    void add_edge(ll u, ll v, bool directional = false){
        Edge[u].push_back(v);
        if (!directional){
            Edge[v].push_back(v);
        }
    }

    void BFS(const vector<ll> &startpoints){
        queue<ll> Q;
        D = vector<ll>(N+1,INF);

        for (auto v : startpoints){
            D[v] = 0;
            Q.push(v);
        }

        while (!Q.empty()){
            ll now = Q.front();
            Q.pop();
            for (auto v : Edge[now]){
                if (D[v] == INF){
                    D[v] = D[now] + 1;
                    Q.push(v);
                }
            }
        }
    }

    void dfs(vector<vector<ll>> &E, ll now, vector<bool> &used, ll &temp, vector<pair<ll,ll>> &order){
        for (auto v : E[now]){
            if (used[v]){continue;}
            used[v] = true;
            dfs(E,v,used,temp,order);
        }
        order[now].second = temp;
        temp++;
    }

    vector<vector<ll>> SCC(){
        vector<vector<ll>> Edge_inverse(N+1);//辺が逆のグラフ
        for (ll i = 1; i <= N; i++){
            for (auto v : Edge[i]){
                Edge_inverse[v].push_back(i);
            }
        }


        ll temp = 1;
        vector<bool> used(N+1,0);
        vector<pair<ll,ll>> order(N+1);

        for (ll i = 1; i <= N; i++){
            order[i].first = i;
        }
        for (ll i = 1; i <= N; i++){
            if (!used[i]){
                used[i] = true;
                dfs(Edge,i,used,temp,order);//一回目のdfsで、帰った順番を記録
            }
        }

        sort(vall(order), [](const pair<ll,ll> &a, const pair<ll,ll> &b){return a.second > b.second;});
        order.pop_back();

        temp = 1;
        vector<ll> groups(N+1,-1);
        for (ll i = 0; i < N; i++){
            if (groups[order[i].first] == -1){//2回目のBFSで連結成分ごとに分解して、番号をつける
                queue<ll> Q;
                Q.push(order[i].first);
                groups[order[i].first] = temp;
                while (!Q.empty()){
                    ll now = Q.front();
                    Q.pop();
                    for (auto v : Edge_inverse[now]){
                        if (groups[v] == -1){
                            groups[v] = temp;
                            Q.push(v);
                        }
                    }
                }
                temp++;
            }
        }
        
        vector<vector<ll>> small_graph(temp);//縮約されたグラフを構築
        for (ll i = 1; i <= N; i++){
            for (auto v : Edge[i]){
                if (groups[i] != groups[v]){
                    small_graph[groups[i]].push_back(groups[v]);
                }
            }
        }

        vector<ll> into_count(temp,0);//入ってくる辺の数を管理
        priority_queue<ll,vector<ll>,greater<ll>> pQ;//入ってくる辺がないような頂点を管理
        vector<ll> topological_sort;

        for (auto &vec : small_graph){
            sort(vall(vec));
            vec.erase(unique(vall(vec)),vec.end());
            for (auto v : vec){
                into_count[v]++;
            }
        }

        for (ll i = 1; i < temp; i++){
            if (into_count[i] == 0){
                pQ.push(i);
            }
        }

        while (!pQ.empty()){
            ll now = pQ.top();
            pQ.pop();
            topological_sort.push_back(now);
            for (auto v : small_graph[now]){
                into_count[v]--;
                if (into_count[v] == 0){
                    pQ.push(v);
                }
            }
        }

        vector<ll> inv_topological_sort(temp);
        for (ll i = 1; i < temp; i++){
            inv_topological_sort[topological_sort[i-1]] = i;
        }
        vector<vector<ll>> ans(temp-1);
        for (ll i = 1; i <= N; i++){
            ans[inv_topological_sort[groups[i]]-1].push_back(i);
        }

        return ans;
    }

};

struct WeightedGraph{
    ll N;
    vector<vector<pair<ll,ll>>> Edge;
    vector<ll> D;
        /// @brief 頂点,辺数を指定して自動で入力を受け取ってグラフを構築する
    /// @param vertex_num 
    /// @param edge_num 
    /// @param directional 
    WeightedGraph(ll vertex_num, ll edge_num, bool directional = false){
        N = vertex_num;
        Edge = vector<vector<pair<ll,ll>>>(N+1);
        for (ll i = 0; i < edge_num; i++){
            ll u,v,w;
            cin >> u >> v >> w;
            Edge[u].push_back({v,w});
            if (!directional){
                Edge[v].push_back({u,w});
            }
        }
    }
    /// @brief 頂点数を指定して辺のないグラフを構築
    /// @param vertex_num 
    WeightedGraph(ll vertex_num){
        N = vertex_num;
        Edge = vector<vector<pair<ll,ll>>>(N+1);
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築
    /// @param E 
    WeightedGraph(const vector<vector<pair<ll,ll>>> &E){
        N = E.size()-1;
        Edge = E;
    }

    void add_edge(ll u, ll v, ll w, bool directional = false){
        Edge[u].push_back({v,w});
        if (!directional){
            Edge[v].push_back({v,w});
        }
    }

    void Djikstra(const vector<ll> &startpoints){
        vector<bool> visited(N+1,0);
        D = vector<ll>(N+1,INF);

        priority_queue<pair<ll,ll>,vector<pair<ll,ll>>,greater<pair<ll,ll>>> pQ;
        for (auto v : startpoints){
            pQ.push({0,v});
        }

        while (!pQ.empty()){
            ll dist = pQ.top().first;
            ll now = pQ.top().second;
            pQ.pop();
            if (visited[now]){continue;}
            visited[now] = 1;
            D[now] = dist;
            for (auto v : Edge[now]){
                if (visited[v.first]){continue;}
                pQ.push({dist+v.second, v.first});
            }
        }
    }
};