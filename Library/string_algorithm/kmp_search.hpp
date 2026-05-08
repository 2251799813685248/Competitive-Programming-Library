#ifndef KMP_SEARCH_HPP_
#define KMP_SEARCH_HPP_

#include <string>
#include <vector>
using namespace std;



/// @brief kmp_searchに使うスキップテーブルを作成
vector<int> create_partial_match_table(const string &t){
    vector<int> table(t.size(),0);
    table[0] = -1;
    int j = -1;
    for (int i = 0; i < (int)t.size()-1; i++){
        while (j >= 0 && t[i] != t[j]){
            j = table[j];
        }
        table[i+1] = j+1;
        j++;
    }
    return table;
}

/// @brief sの中にtが含まれているかを判定し、あるなら最初に現れる位置を求める
/// @param s 
/// @param t 
/// @return 
int kmp_search(const string &s, const string &t){
    auto table = create_partial_match_table(t);
    int i = 0, j = 0;
    while (i+j < (int)s.size()){
        if (s[i+j] == t[j]){
            j++;
            if (j == (int)t.size()){
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




#endif /* KMP_SEARCH_HPP_ */