#include <vector>
#include <string>

using namespace std;
#define ll long long

vector<ll> create_partial_match_table(const string &t){
    vector<ll> table(t.size(),0);
    table[0] = -1;
    ll j = -1;
    for (ll i = 0; i < t.size()-1; i++){
        while (j >= 0 && t[i] != t[j]){
            j = table[j];
        }
        table[i+1] = j+1;
        j++;
    }
    return table;
}


ll kmp_search(const string &s, const string &t){
    auto table = create_partial_match_table(t);
    ll i = 0, j = 0;
    while (i+j < s.size()){
        if (s[i+j] == t[j]){
            j++;
            if (j == t.size()){
                return i;
            }
        }
        else{
            i = i+j-table[j];
            if (j > 0){
                j = table[j];
            }
        }
    }
    return -1;
}