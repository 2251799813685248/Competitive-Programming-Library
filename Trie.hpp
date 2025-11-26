#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#define ll long long
using namespace std;


/// @brief ただのTrie木
struct Trie{
    struct TrieNode{
        char c;//このノードの文字
        bool forbidden = false;//禁止されたかどうか
        ll stringnum = 0;//何文字分このノードで終わっているか。
        unordered_map<char,TrieNode*> child;//子ノードへの参照

        TrieNode(char d, ll n){
            c = d;
            stringnum = n;
        }

        TrieNode(){}
    };

    TrieNode root;//trie木の根。文字を持たない('.'を持っている)

    Trie(){
        root = TrieNode('.', 0);
    }

    /// @brief 文字列を追加する。追加しようとしたけどダメだったらfalseが返ってくる。
    /// @param S 
    /// @return 追加できたか。
    bool addstring(const string &S){
        TrieNode *N = &root;
        for (ll i = 0; i < S.size(); i++){
            if (N->child.count(S[i])){
                if (N->child[S[i]]->forbidden){
                    return false;
                }
            }
            else{
                N->child[S[i]] = new TrieNode(S[i],0);
            }

            if (i == S.size()-1){
                N->child[S[i]]->stringnum++;
            }
            N = N->child[S[i]];
        }
        return true;
    }

    /// @brief 接頭辞Sを禁止する。
    /// @param S 
    /// @return 禁止によっていくつの文字が消えたか。
    ll forbid(const string &S){
        TrieNode *N = &root;
        for (ll i = 0; i < S.size(); i++){
            if (N->child.count(S[i])){
                if (N->child[S[i]]->forbidden){
                    return 0;
                }
            }
            else{
                N->child[S[i]] = new TrieNode(S[i],0);
            }

            if (i == S.size()-1){
                N->child[S[i]]->forbidden = true;
            }
            N = N->child[S[i]];
        }

        ll forbidden_string = 0;//DFSでこれより下を全部禁止する
        deque<TrieNode*> Q;
        Q.push_back(N);
        while (!Q.empty()){
            TrieNode *node = Q.back();
            Q.pop_back();
            forbidden_string += node->stringnum;
            for (auto v : node->child){
                Q.push_back(v.second);
            }
        }
        N->child.clear();
        N->stringnum = 0;
        return forbidden_string;
    }
    
};