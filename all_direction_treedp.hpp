#define ll long long
#include <vector>
using namespace std;

//全方位木DP用の構造体
struct treedpinfo{
    ll farthest_vertex = -1;
    ll dist = -1;
};

//いくつかの部分木の情報をまとめる
treedpinfo merge_operation(const treedpinfo &a, const treedpinfo &b){
    if (a.dist == b.dist){
        if (a.farthest_vertex > b.farthest_vertex){
            return a;
        }
        return b;
    }
    else{
        if (a.dist > b.dist){
            return a;
        }
        return b;
    }
}
//一個下の部分木の情報をもとに次の頂点に更新する
treedpinfo update_operation(treedpinfo a){
    a.dist++;
    return a;
}

/// @brief 木DP
/// @param E 
/// @param dp 
/// @param used 
/// @param now 
/// @param eee 
void dfs_dp(vector<vector<ll>> &E, vector<treedpinfo> &dp, vector<bool> &used, ll now, treedpinfo &eee){
    used[now] = 1;
    if (E[now].empty() || (E[now].size() == 1 && now != 1)){
        //葉の状態は自分で書く。
        dp[now].farthest_vertex = now;
        dp[now].dist = 0;
        return;
    }

    treedpinfo r = eee;
    for (auto v : E[now]){
        if (used[v]){continue;}
        dfs_dp(E,dp,used,v,eee);
        r = merge_operation(r,dp[v]);
    }
    dp[now] = update_operation(r);
}
/// @brief 全方位木DP
/// @param E 
/// @param dp 
/// @param used 
/// @param now 
/// @param from_parent
void dfs_rerooting(vector<vector<ll>> &E, vector<treedpinfo> &dp, vector<bool> &used, ll now, treedpinfo from_parent){
    used[now] = 1;
    if (E[now].empty() || (E[now].size() == 1 && now != 1)){return;}


    ll child_num = 0;
    vector<ll> child_index;
    for (auto v : E[now]){
        if (used[v]){continue;}
        child_index.push_back(v);
        child_num++;
    }
    if (child_num == 1){
        dp[child_index[0]] = merge_operation(dp[child_index[0]],update_operation(from_parent));
        dfs_rerooting(E,dp,used,child_index[0],update_operation(from_parent));
        return;
    }
    vector<treedpinfo> left_ruiseki(child_num);
    left_ruiseki.front() = dp[child_index.front()];
    vector<treedpinfo> right_ruiseki(child_num);
    right_ruiseki.back() = dp[child_index.back()];
    for (ll i = 1; i < child_num; i++){
        left_ruiseki[i] = merge_operation(left_ruiseki[i-1], dp[child_index[i]]);
        right_ruiseki[child_num-i-1] = merge_operation(right_ruiseki[child_num-i], dp[child_index[child_num-i-1]]);
    }

    for (ll i = 0; i < child_num; i++){
        if (i == 0){
            auto temp = update_operation(merge_operation(update_operation(right_ruiseki[i+1]),from_parent));
            dp[child_index[i]] = merge_operation(dp[child_index[i]],temp);
            dfs_rerooting(E,dp,used,child_index[i],temp);
        }
        else if (i == child_num-1){
            auto temp = update_operation(merge_operation(update_operation(left_ruiseki[i-1]),from_parent));
            dp[child_index[i]] = merge_operation(dp[child_index[i]],temp);
            dfs_rerooting(E,dp,used,child_index[i],temp);
        }
        else{
            auto temp = update_operation(merge_operation(update_operation(merge_operation(left_ruiseki[i-1],right_ruiseki[i+1])),from_parent));
            dp[child_index[i]] = merge_operation(dp[child_index[i]],temp);
            dfs_rerooting(E,dp,used,child_index[i],temp);
        }
    }

}

/* sample usage 428E Farthest Vertex

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    ll N;
    cin >> N;
    vector<vector<ll>> E(N+1);
    for (ll i = 1; i < N; i++){
        ll s,t;
        cin >> s >> t;
        E[s].push_back(t);
        E[t].push_back(s);
    }

    vector<treedpinfo> alldirectiondp(N+1);
    vector<bool> used(N+1,0);

    treedpinfo eee;//単位元(作用させても変わらないもの)


    dfs_dp(E,alldirectiondp,used,1,eee);
    used = vector<bool>(N+1,0);
    treedpinfo only_parent;//親のみの部分木の時の答えを記録しておく。
    only_parent.dist = 0;
    only_parent.farthest_vertex = 1;
    dfs_rerooting(E,alldirectiondp,used,1,only_parent);


    for (ll i = 1; i <= N; i++){
        cout << alldirectiondp[i].farthest_vertex << "\n";
    }

}
*/