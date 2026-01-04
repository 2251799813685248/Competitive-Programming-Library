#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <functional>
#include <random>
#include <bitset>


using namespace std;
#define ll long long





void dfs(vector<vector<ll>> &E, vector<bool> &used, vector<ll> &dist, vector<pair<ll,ll>> &new_number, ll now, ll &timeee){
    if (used[now]){return;}
    used[now] = 1;
    new_number[now].first = timeee;
    timeee++;
    for (auto &v : E[now]){
        if (used[v]){continue;}
        dist[v] = dist[now]+1;
        dfs(E,used,dist,new_number,v,timeee);
    }
    new_number[now].second = timeee-1;
}


int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    ll N;
    cin >> N;
    vector<vector<ll>> E(N+1);
    vector<pair<ll,ll>> new_number(N+1);
    vector<bool> used(N+1,0);
    vector<ll> dist(N+1,0);
    vector<pair<ll,ll>> Edge(N);
    for (ll i = 1; i < N; i++){
        ll u,v;
        cin >> u >> v;
        E[u].push_back(v);
        E[v].push_back(u);
        Edge[i] = {u,v};
    }

    ll timeee = 1;
    dfs(E,used,dist,new_number,1,timeee);


}