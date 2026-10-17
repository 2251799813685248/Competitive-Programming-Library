#ifndef INVERSIONS_
#define INVERSIONS_

#include <SegTree.hpp>
#include <algorithm>

using ll = long long;

/// @brief 転倒数と、座標圧縮後の配列を返す。
/// 転倒数とは、i<jかつA[i]>A[j]を満たす非負整数i,j(0<=i,j<A.size())の組の個数である。
template<typename T> inline constexpr pair<ll, list<int>> inversions(const list<T>& A){
    list<T> B = A;
    sort(vall(B));
    list<int> C(A.size());
    for (int i = 0, sz = A.size(); i < sz; i++){
        C[i] = lower_bound(vall(B), A[i]) - B.begin();
    }
    ll inv = 0;
    SegTree<int> ST(A.size(), 0, 0, [](int a, int b){return a+b;});
    for (int i = A.size()-1; i >= 0; i--){
        inv += ST.range_get(0, C[i]);
        ST.pointwise_update(C[i], ST.get(C[i])+1);
    }
    return make_pair(inv, C);
}


#endif /*INVERSIONS*/