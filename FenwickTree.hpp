#include <vector>
#include <algorithm>
#define ll long long
using namespace std;

struct FenwickTree{
    private:
    vector<ll> pow2ll{1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296};

    public:
    vector<ll> tree;
    vector<ll> B;
    ll log2N = 0;

    void fill_tree(vector<ll> &ruisekiwaA, ll L, ll R, ll index){
        if (index >= pow2ll[log2N]){return;}
        if (index == 0){
            tree[index] = ruisekiwaA.back();
            fill_tree(ruisekiwaA,L,(L+R)/2,1);
            return;
        }
        tree[index] = ruisekiwaA[R] - (L == 0 ? 0 : ruisekiwaA[L-1]);
        fill_tree(ruisekiwaA,L,(L+R)/2,2*index);
        fill_tree(ruisekiwaA,R+1,(3*R-L+1)/2,2*index+1);
    }


    FenwickTree(ll howmany, ll value){
        while (pow2ll[log2N] < howmany){
            log2N++;
        }
        tree = vector<ll>(pow2ll[log2N], value);
        B = vector<ll>(pow2ll[log2N], value);
        for (ll i = pow2ll[log2N-1]-1; i >= 1; i--){
            tree[i] = tree[2*i]*2;
        }
        tree[0] = 2*tree[1];
    }

    FenwickTree(ll howmany, vector<ll> &A){
        while (pow2ll[log2N] < howmany){
            log2N++;
        }
        vector<ll> ruisekiwaA(pow2ll[log2N]);
        ruisekiwaA[0] = A[0];
        for (ll i = 1; i < pow2ll[log2N]; i++){
            if (i < howmany){
                ruisekiwaA[i] = A[i] + ruisekiwaA[i-1];
            }
            else{
                ruisekiwaA[i] = ruisekiwaA[i-1];
            }
        }
        tree = vector<ll>(pow2ll[log2N],0);
        B = A;
        while (B.size() < pow2ll[log2N]){
            B.push_back(0);
        }
        fill_tree(ruisekiwaA,0,pow2ll[log2N]-1,0);
    }

    ll sum_from_origin(ll index){
        index++;
        if (index == pow2ll[log2N]){
            return tree[0];
        }
        vector<ll> temp(log2N+1);
        for (ll i = 0; i <= log2N; i++){
            temp[i] = index%2;
            index /= 2;
        }
        reverse(temp.begin(), temp.end());
        vector<ll> temp2;
        temp2.push_back(0);
        temp2.push_back(1);
        for (ll i = 2; i <= log2N; i++){
            temp2.push_back(0);
            temp2[i] = temp2[i-1]*2 + temp[i-1];
        }
        ll sum = 0;
        for (ll i = 0; i <= log2N; i++){
            sum += temp[i] * tree[temp2[i]];
        }
        return sum;
    }

    /// @brief [l,r]の総和を返す。
    /// @param l 
    /// @param r 
    /// @return 総和
    ll range_sum(ll l, ll r){
        if (l == 0){
            return sum_from_origin(r);
        }
        return sum_from_origin(r)-sum_from_origin(l-1);
    }


    void add(ll index, ll value, ll L = 0, ll d = 0, ll index_on_tree = 0){
        if (index_on_tree == 0){
            tree[index_on_tree] += value;
            add(index,value,0,1,1);
            return;
        }
        if (d == log2N+1){
            return;
        }
        ll R = L + pow2ll[log2N-d]-1;
        if (L <= index && index <= R){
            tree[index_on_tree] += value;
            add(index,value,L,d+1,2*index_on_tree);
        }
        else{
            add(index,value,R+1,d+1,2*index_on_tree+1);
        }
    }

    /// @brief 一か所変える。
    /// @param index 
    void update(ll index, ll value){
        add(index,value-B[index]);
        B[index] = value;
    }
    /// @brief 一か所をa倍してbを足す。
    /// @param index 
    /// @param a 
    /// @param b 
    void update(ll index, ll a, ll b){
        add(index,(a-1)*B[index]+b);
        B[index] = B[index]*a + b;
    }
};