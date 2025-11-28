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

vector<ll> powroot998244353{1LL, 998244352LL, 911660635LL, 372528824LL, 69212480LL, 381091786LL, 515872972LL, 273395035LL, 469477650LL, 503794470LL, 464513504LL, 558899782LL, 504969456LL, 840897051LL, 539927980LL, 417009993LL, 725461291LL, 456548895LL, 712494065LL, 542639453LL, 768214923LL, 303507821LL, 438914733LL, 761881641};
vector<ll> powrootinv998244353{1LL, 998244352LL, 86583718LL, 509520358LL, 661054123LL, 863921598LL, 323451984LL, 689146517LL, 379690232LL, 240519260LL, 899368279LL, 920065956LL, 588792270LL, 118574449LL, 847593593LL, 858760106LL, 987418793LL, 177938570LL, 753608159LL, 786906984LL, 331540181LL, 655706071LL, 268754442LL, 191076546};
vector<ll> powroot1224736769{1LL, 1224736768LL, 24506215LL, 992888437LL, 853017291LL, 235319402LL, 269744380LL, 157861287LL, 894223137LL, 600648668LL, 1091208103LL, 382541006LL, 12818149LL, 149218560LL, 746299392LL, 405692663LL, 633223136LL, 672327338LL, 992917013LL, 758198491LL, 1079610480LL, 1056667043LL, 1039331205LL, 1026803890LL, 449603200};
vector<ll> powrootinv1224736769{1LL, 1224736768LL, 1200230554LL, 961581489LL, 796105727LL, 1023008969LL, 406386483LL, 251411652LL, 655108271LL, 1145368249LL, 780593535LL, 38041180LL, 816166160LL, 659160240LL, 1200430513LL, 392462252LL, 15561184LL, 893027826LL, 928760284LL, 497993173LL, 529117122LL, 637457654LL, 931394937LL, 823596420LL, 55047034};


vector<ll> DFT998244353(vector<ll> X, ll K, bool inverse = false){
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
    for (ll i = 0; i < (1LL<<K); i++){
        if (i%2){
            X[i] = odd[i/2];
        }
        else{
            X[i] = even[i/2];
        }
    }
    return X;
}
vector<ll> DFT1224736769(vector<ll> X, ll K, bool inverse = false){
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
    for (ll i = 0; i < (1LL<<K); i++){
        if (i%2){
            X[i] = odd[i/2];
        }
        else{
            X[i] = even[i/2];
        }
    }
    return X;
}



vector<ll> convolution998244353(vector<ll> A, vector<ll> B){

    if (min(A.size(), B.size()) <= 60) {
        if (A.size() < B.size()) {
            swap(A, B);
        }
        std::vector<ll> ans(A.size() + B.size() - 1);
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < B.size(); j++) {
                ans[i + j] += A[i] * B[j];
            }
        }
        for (auto &v : ans){
            v %= 998244353;
        }
        return ans;
    }

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
    ll invpow2log2N = inverse_mod((1LL<<log2N),998244353);
    for (ll i = 0; i < (1LL<<log2N); i++){
        C[i] = (C[i]*invpow2log2N)%998244353;
    }
    return C;
}
vector<ll> convolution1224736769(vector<ll> A, vector<ll> B){

    if (min(A.size(), B.size()) <= 60) {
        if (A.size() < B.size()) {
            swap(A, B);
        }
        std::vector<ll> ans(A.size() + B.size() - 1);
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < B.size(); j++) {
                ans[i + j] += A[i] * B[j];
            }
        }
        for (auto &v : ans){
            v %= 1224736769;
        }
        return ans;
    }

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
    ll invpow2log2N = inverse_mod((1LL<<log2N),1224736769);
    for (ll i = 0; i < 1048576; i++){
        C[i] = (C[i]*invpow2log2N)%1224736769;
    }
    return C;
}