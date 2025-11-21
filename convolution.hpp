#include <vector>
#include <algorithm>
#define ll long long
using namespace std;


/// @brief 一次不定方程式ax+by=1の解を1つ見つける
/// @param a 
/// @param b 
/// @return {x,y}
pair<ll,ll> axby1(ll a, ll b){
    if (a == 1 or b == 1){
        if (a == 1){
            return {1-b,1};
        }
        else{
            return {1,1-a};
        }
    }
    else if (a>b){
        auto p = axby1(a%b, b);
        return {p.first, p.second - p.first*(a/b)};
    }
    else{
        swap(a,b);
        auto p = axby1(a%b, b);
        return {p.second - p.first*(a/b), p.first};
    }
}

/// @brief 1/a mod Mを求める
/// @param a 
/// @param M 
/// @return 1/a mod M
ll inverse_mod(ll a, ll M){
    if (__gcd(a,M) != 1){
        return -1;
    }
    return (M+(axby1(a,M).first)%M)%M;
}

vector<ll> powroot998244353{1, 998244352, 911660635, 372528824, 69212480, 381091786, 515872972, 273395035, 469477650, 503794470, 464513504, 558899782, 504969456, 840897051, 539927980, 417009993, 725461291, 456548895, 712494065, 542639453, 768214923, 303507821, 438914733, 761881641};
vector<ll> powrootinv998244353{1, 998244352, 86583718, 509520358, 661054123, 863921598, 323451984, 689146517, 379690232, 240519260, 899368279, 920065956, 588792270, 118574449, 847593593, 858760106, 987418793, 177938570, 753608159, 786906984, 331540181, 655706071, 268754442, 191076546};
vector<ll> powroot1224736769{1, 1224736768, 24506215, 992888437, 853017291, 235319402, 269744380, 157861287, 894223137, 600648668, 1091208103, 382541006, 12818149, 149218560, 746299392, 405692663, 633223136, 672327338, 992917013, 758198491, 1079610480, 1056667043, 1039331205, 1026803890, 449603200};
vector<ll> powrootinv1224736769{1, 1224736768, 1200230554, 961581489, 796105727, 1023008969, 406386483, 251411652, 655108271, 1145368249, 780593535, 38041180, 816166160, 659160240, 1200430513, 392462252, 15561184, 893027826, 928760284, 497993173, 529117122, 637457654, 931394937, 823596420, 55047034};


vector<ll> DFT998244353(const vector<ll> &X, ll K, bool inverse = false){
    if (K == 1){
        return vector<ll>{(X[0]+X[1])%998244353, (998244353+X[0]-X[1])%998244353};
    }

    vector<ll> even(1LL<<(K-1));
    vector<ll> odd(1LL<<(K-1));
    for (ll i = 0; i < (1LL<<(K-1)); i++){
        even[i] = (X[i] + X[(1LL<<(K-1))+i])%998244353;
    }
    ll temp = 1;
    if (inverse){
        for (ll i = 0; i < (1LL<<(K-1)); i++){
            odd[i] = (temp*(998244353 + X[i] - X[(1LL<<(K-1))+i]))%998244353;
            temp = (temp*powrootinv998244353[K])%998244353;
        }
    }
    else{
        for (ll i = 0; i < (1LL<<(K-1)); i++){
            odd[i] = (temp*(998244353 + X[i] - X[(1LL<<(K-1))+i]))%998244353;
            temp = (temp*powroot998244353[K])%998244353;
        }
    }

    even = DFT998244353(even,K-1,inverse);
    odd = DFT998244353(odd,K-1,inverse);
    vector<ll> Y;
    for (ll i = 0; i < (1LL<<K); i++){
        if (i%2){
            Y.push_back(odd[i/2]);
        }
        else{
            Y.push_back(even[i/2]);
        }
    }
    return Y;
}
vector<ll> DFT1224736769(const vector<ll> &X, ll K, bool inverse = false){
    if (K == 1){
        return vector<ll>{(X[0]+X[1])%1224736769, (1224736769+X[0]-X[1])%1224736769};
    }

    vector<ll> even(1LL<<(K-1));
    vector<ll> odd(1LL<<(K-1));
    for (ll i = 0; i < (1LL<<(K-1)); i++){
        even[i] = (X[i] + X[(1LL<<(K-1))+i])%1224736769;
    }
    ll temp = 1;
    if (inverse){
        for (ll i = 0; i < (1LL<<(K-1)); i++){
            odd[i] = (temp*(1224736769 + X[i] - X[(1LL<<(K-1))+i]))%1224736769;
            temp = (temp*powrootinv1224736769[K])%1224736769;
        }
    }
    else{
        for (ll i = 0; i < (1LL<<(K-1)); i++){
            odd[i] = (temp*(1224736769 + X[i] - X[(1LL<<(K-1))+i]))%1224736769;
            temp = (temp*powroot1224736769[K])%1224736769;
        }
    }

    even = DFT1224736769(even,K-1,inverse);
    odd = DFT1224736769(odd,K-1,inverse);
    vector<ll> Y;
    for (ll i = 0; i < (1LL<<K); i++){
        if (i%2){
            Y.push_back(odd[i/2]);
        }
        else{
            Y.push_back(even[i/2]);
        }
    }
    return Y;
}



vector<ll> convolution998244353(vector<ll> A, vector<ll> B){
    ll N = A.size()+B.size()-1;
    ll log2N = 0;
    while ((1LL<<log2N) < N){
        log2N++;
    }

    while (A.size() < (1LL<<log2N)){
        A.push_back(0);
    }
    while (B.size() < (1LL<<log2N)){
        B.push_back(0);
    }

    A = DFT998244353(A,log2N);
    B = DFT998244353(B,log2N);
    vector<ll> C((1LL<<log2N),0);
    for (ll i = 0; i < (1LL<<log2N); i++){
        C[i] = (A[i]*B[i])%998244353;
    }
    C = DFT998244353(C,log2N,1);
    for (ll i = 0; i < (1LL<<log2N); i++){
        C[i] = (C[i]*inverse_mod((1LL<<log2N),998244353))%998244353;
    }
    return C;
}
vector<ll> convolution1224736769(vector<ll> A, vector<ll> B){
    ll N = A.size()+B.size()-1;
    ll log2N = 0;
    while ((1LL<<log2N) < N){
        log2N++;
    }

    while (A.size() < (1LL<<log2N)){
        A.push_back(0);
    }
    while (B.size() < (1LL<<log2N)){
        B.push_back(0);
    }

    A = DFT1224736769(A,log2N);
    B = DFT1224736769(B,log2N);
    vector<ll> C((1LL<<log2N),0);
    for (ll i = 0; i < (1LL<<log2N); i++){
        C[i] = (A[i]*B[i])%1224736769;
    }
    C = DFT1224736769(C,(1LL<<log2N),1);
    for (ll i = 0; i < 1048576; i++){
        C[i] = (C[i]*inverse_mod((1LL<<log2N),1224736769))%1224736769;
    }
    return C;
}