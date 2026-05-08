#ifndef FLOOR_OF_LINEAR_HPP_
#define FLOOR_OF_LINEAR_HPP_

#include <cassert>
#include <math_functions.hpp>
using ll = long long;



ll internal_floor_sum(ll A, ll B, ll C){
    if (C < 0){return 0;}
    if (A > B){swap(A,B);}
    if (B%A == 0){
        return (1+floor2(C,A))*(1+floor2(C,B)) - (B/A)*floor2(C,B)*(floor2(C,B)+1)/2;
    }
    ll k = floor2(C-B*floor2(C,B),A);
    return (1+k)*(1+floor2(C,B)) + floor2(B,A)*floor2(C,B)*(floor2(C,B)+1)/2 + internal_floor_sum(A, B%A, C-A*(floor2(B,A)*floor2(C,B)+k+1));
}
/// @brief `\sum_{i=0}^{N} \lfloor\frac{Ci+D}{B}\rfloor`を求める。
ll floor_sum(ll N, ll B, ll C, ll D){
    if (N < 0){
        return 0;
    }
    if (B < 0){//Bを負にする。
        B *= -1;
        C *= -1;
        D *= -1;
    }
    if (C > 0){//Cを負にするが、Cを-Cに置き換えてC>0として扱う。
        D += N*C;
    }
    else{
        C *= -1;
    }
    if (C == 0){
        return (N+1)*floor2(D,B);
    }
    ll k = floor2(D-C*N,B);
    return (N+1)*k + internal_floor_sum(B,C,D-B*(k+1));
}

//ll generalized_floor_sum_1_1(ll N, ll B, ll C, ll D){
//    return 0;
//}
//ll generalized_floor_sum_0_2(ll N, ll B, ll C, ll D){
//    return 0;
//}


ll internal_floor_max(ll A, ll B, ll C, ll D, ll E, ll F){
    if (D < 0){return -1000000000000000000;}
    if (C <= 0){return -1000000000000000000;}
    if (E > 0){
        if (B > C){swap(A,E);swap(B,C);}
        ll M = floor2(D-C*floor2(D, C), B);
        ll tempans = max(A*M+E*floor2(D, C), A*floor2(D, B)) + F;
        return max(tempans, A*(1+M)+internal_floor_max(A, B, C%B, D-B*(1+M+floor2(C, B)*floor2(D, C)), E-A*floor2(C, B), F+A*floor2(C, B)*floor2(D, C)));
    }
    else return A*floor2(D, B) + F;
}
/// @brief `0<=x<=N`の下で、`A*floor2(C*x+D, B)+E*x+F`の最大値を求める。もし何かがおかしいなら`-10^18`が返される。
/// @param N 
/// @param A 
/// @param B 
/// @param C 
/// @param D 
/// @param E 
/// @param F 
/// @return `max`
ll floor_max(ll N, ll A, ll B, ll C, ll D, ll E, ll F){
    if (N < 0){
        return -1000000000000000000;
    }
    assert(B != 0);
    //マイナスを処理
    if (B < 0){
        B *= -1;
        C *= -1;
        D *= -1;
    }
    if (A < 0){
        A *= -1;
        C *= -1;
        D *= -1;
        D += B-1;
    }
    //自明なケース
    if (C == 0 or A == 0){
        return A*floor2(D, B) + F + max(0LL, E*N);
    }
    if (E == 0){
        return A*floor2(max(0LL, C*N)+D, B) + F;
    }
    //Cの係数を調整
    if (C > 0){
        F += E*N;
        E *= -1;
        D += C*N;
    }
    else{
        //`A*floor2(D-C*x, B)+E*x+F`, `A,B,C>0`にする
        C *= -1;
    }
    //自明なケースを処理
    if (E < 0){
        return A*floor2(D, B) + F;
    }
    ll x_offset = floor2(D-C*N, B)+1;
    D -= B*x_offset;
    return A*x_offset + max(internal_floor_max(A,B,C,D,E,F), -A+E*N+F);
}




#endif /* FLOOR_OF_LINEAR_HPP_ */