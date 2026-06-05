#ifndef GEOMRTRY__HPP_
#define GEOMRTRY__HPP_

#include <vector>
#include <queue>
#include <array>
#include <algorithm>
using namespace std;
using ll = long long;
using pll = array<ll,2>;


pll operator+(const pll& a, const pll& b){
    return {a[0]+b[0], a[1]+b[1]};
}
pll operator-(const pll& a, const pll& b){
    return {a[0]-b[0], a[1]-b[1]};
}
ll dot(const pll& a, const pll& b){return a[0]*b[0]+a[1]*b[1];}
ll cross_z(const pll& a, const pll& b){return a[0]*b[1]-a[1]*b[0];}


/// @brief {x,y}の辞書順に点をソートした時、最小と最大を通るような下側凸包を求める。 
vector<pll> static_convex_hull_below(vector<pll> A){
    sort(A.begin(), A.end());
    deque<pll> res;
    for (auto& R : A){
        if (res.empty()){res.push_back(R);continue;}
        if (R == res.back()){continue;}
        if (res.size() == 1ull){res.push_back(R);}
        while (res.size() >= 2ull){
            auto Q = res.back();
            res.pop_back();
            auto P = res.back();
            if (cross_z(Q-P, R-P) > 0){
                res.push_back(Q);
                break;
            }
        }
        res.push_back(R);
    }
    return vector<pll>(res.begin(), res.end());
}
/// @brief {x,y}の辞書順に点をソートした時、最小と最大を通るような上側凸包を求める。 
vector<pll> static_convex_hull_above(vector<pll> A){
    sort(A.begin(), A.end());
    deque<pll> res;
    for (auto& R : A){
        if (res.empty()){res.push_back(R);continue;}
        if (R == res.back()){continue;}
        if (res.size() == 1ull){res.push_back(R);}
        while (res.size() >= 2){
            auto Q = res.back();
            res.pop_back();
            auto P = res.back();
            if (cross_z(R-P, Q-P) > 0){
                res.push_back(Q);
                break;
            }
        }
        res.push_back(R);
    }
    return vector<pll>(res.begin(), res.end());
}

/// @brief {x,y}の辞書順に点をソートした時、最小の点から始めて反時計回りに回るように凸包の頂点をすべて列挙する。
vector<pll> static_convex_hull(const vector<pll>& A){
    auto B = static_convex_hull_below(A);
    auto C = static_convex_hull_above(A);
    for (int i = (int)C.size()-2; i > 0; i--){
        B.push_back(move(C[i]));
    }
    return B;
}


/// @brief (同一でもよい)3点`a`,`b`,`c`が同一直線上に並んでいるかどうかを判定 
inline bool colinear(const pll &a, const pll &b, const pll &c){
    return cross_z(b-a,c-a) == 0;
}
/// @brief (同一でもよい)2点`a`,`b`を結ぶ線分上に点`c`があるかを判定する。線分上にないなら`0`, 線分上であって端点と重ならないなら`1`, 線分上であってどちらかの端点上なら`2`
int on_segment(const pll &a, const pll &b, const pll &c){
    if (a == c || b == c){return 2;}
    if (cross_z(a-c,b-c) == 0){
        if (dot(b-a,c-a) > 0 && dot(a-b,c-b) > 0){return 1;}
        else{return 0;}
    }
    return 0;
}
/// @brief 異なる2点`a`,`b`を通る直線と、それ上にない点`c`が与えられたとき、点`d`が直線で区切られた領域に関して`c`と同じ領域に属するかどうかを判定する。同じ領域なら`1`, 異なる領域なら`-1`, 直線上なら`0`
int same_side(const pll& a, const pll& b, const pll& c, const pll& d){
    assert(a != b && !colinear(a,b,c));
    if (colinear(a,b,d)){return 0;}
    ll e = cross_z(b-a,c-a);
    ll f = cross_z(b-a,d-a);
    if ((e > 0 && f > 0) || (e < 0 && f < 0)){return 1;}
    return -1;
}
/// @brief 三点`a`,`b`,`c`が作る凸包の内部に点`d`があれば2, 境界上にあれば1, 外部にあれば0を返す。 
//int in_triangle(const pll& a, const pll& b, const pll& c, const pll& d){
//    if (colinear(a,b,c)){
//        
//    }
//}

/// @brief 偏角は-πより大きく、π以下であるとする。原点の偏角は0とする。このとき、arg(a) < arg(b)かどうかを判定する 
bool compare_argument(const pll& a, const pll& b){
    if (a[0] == 0 && a[1] == 0){
        return (b[1] == 0 && b[0] < 0) || b[1] > 0;
    }
    if (b[0] == 0 && b[1] == 0){
        return a[1] < 0;
    }
    if ((a[1] == 0 && a[0] < 0) || a[1] > 0){
        if ((b[1] == 0 && b[0] < 0) || b[1] > 0){
            return cross_z(a,b) > 0;
        }
        else{
            return false;
        }
    }
    else{
        if ((b[1] == 0 && b[0] < 0) || b[1] > 0){
            return true;
        }
        else{
            return cross_z(a,b) > 0;
        }
    }
}


#endif /* GEOMRTRY__HPP_ */