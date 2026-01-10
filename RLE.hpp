/// @brief AをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
template<typename T>
vector<pair<T,ll>> RLEvec(const vector<T> &A){
    vector<pair<T,ll>> R;
    T previous = A[0];
    ll combo = 0;
    for (ll i = 0;;){
        if (i == A.size()){
            R.push_back({previous,combo});
            break;
        }
        if (A[i] == previous){
            combo++;
            i++;
        }
        else{
            R.push_back({previous, combo});
            previous = A[i];
            combo = 0;
        }
    }
    return R;
}

/// @brief SをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
vector<pair<char,ll>> RLEstr(const string &S){
    vector<pair<char,ll>> R;
    char previous = S[0];
    ll combo = 0;
    for (ll i = 0;;){
        if (i == S.size()){
            R.push_back({previous,combo});
            break;
        }
        if (S[i] == previous){
            combo++;
            i++;
        }
        else{
            R.push_back({previous, combo});
            previous = S[i];
            combo = 0;
        }
    }
    return R;
}
