#ifndef MATRIX__HPP_
#define MATRIX__HPP_

#include <list.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <cassert>
#include <modint.hpp>
#include <prime_and_divisors.hpp>
using namespace std;
using ll = long long;



/// @brief 可変サイズの行列
/// @attention 行,列共に0-indexed
/// @attention コンストラクタにHとWを渡すと、[0][0]~[H-1][W-1]までできる。
/// @attention コンストラクタ1 matrix(N)...N次単位行列
/// @attention コンストラクタ2 matrix(h,w,v)...全成分がvの行列
/// @attention コンストラクタ3 matrix(vecvec)...2次元listで初期化
/// @tparam T 数を表す型
template<typename T>
class matrix{
    public:
    
    list<list<T>> M;
    int H,W;
    using value_type = T;


    /// @brief デフォルトコンストラクタ すべて0で初期化される
    constexpr matrix(){H = 0; W = 0; M = list<list<T>>();}
    /// @brief N次単位行列を生成
    /// @param N 
    constexpr matrix(int N){
        H = N;W = N;
        M = list<list<T>>(N,list<T>(N,0));
        for (int i = 0; i < N; i++){
            M[i][i] = 1;
        }
    }
    /// @brief h×w型の、全要素がvの行列を生成
    /// @param h 
    /// @param w 
    /// @param v 
    constexpr matrix(int h, int w, T v = (T)0){
        H = h;
        W = w;
        M = list<list<T>>(H,list<T>(W,v));
    }
    /// @brief 2次元配列を用いて行列を生成
    /// @param A 
    constexpr matrix(const list<list<T>> &A){
        M = A;
        H = A.size();
        W = A.empty() ? 0 : A[0].size();
    }
    constexpr matrix(const list<T>& A){
        H = A.size();
        W = 1;
        M.resize(H);
        for (int i = 0; i < H; i++){M[i] = list<T>{A[i]};}
    }
    constexpr matrix(const matrix& A){
        H = A.H;
        W = A.W;
        M = A.M;
    }
    constexpr matrix(matrix&& A) noexcept {
        H = A.H;
        W = A.W;
        M = std::move(A.M);
    }



    matrix& operator=(const matrix& A){
        H = A.H;
        W = A.W;
        M = A.M;
        return *this;
    }
    matrix& operator=(matrix&& A) noexcept {
        H = A.H;
        W = A.W;
        M = std::move(A.M);
        return *this;
    }

    matrix operator+() const {return *this;}
    matrix operator-() const {return  *this * (T)(-1);}
    

    matrix& operator+=(const matrix &A){
        if (H != A.H  || W != A.W){
            cerr << "SizeError: addition of matrix" << endl;
            assert(false);
        }
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] += A.M[i][j];
            }
        }
        return *this;
    }
    matrix& operator-=(const matrix &A){
        if (H != A.H  || W != A.W){
            cerr << "SizeError: subtraction of matrix" << endl;
            assert(false);
        }
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] -= A.M[i][j];
            }
        }
        return *this;
    }
    matrix& operator*=(const matrix &A){
        if (W != A.H){
            cerr << "SizeError: multiplication of matrix" << endl;
            assert(false);
        }
        matrix ans(H,A.W,0);
        for (int i = 0; i < H; i++){
            for (int k = 0; k < W; k++){
                T constval = M[i][k];
                for (int j = 0; j < A.W; j++){
                    ans.M[i][j] += constval*A.M[k][j];
                }
            }
        }
        W = A.W;
        M = ans.M;
        return *this;
    }
    matrix& operator*=(const T c){
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] *= c;
            }
        }
        return *this;
    }
    
    friend matrix operator+(const matrix& lhs, const matrix& rhs) {return matrix(lhs) += rhs;}
    friend matrix operator-(const matrix& lhs, const matrix& rhs) {return matrix(lhs) -= rhs;}
    friend matrix operator*(const matrix& lhs, const matrix& rhs) {return matrix(lhs) *= rhs;}
    friend matrix operator*(const matrix& lhs, const T& rhs) {return matrix(lhs) *= rhs;}
    
    friend bool operator==(const matrix& lhs, const matrix& rhs) {return lhs.M == rhs.M;}
    friend bool operator!=(const matrix& lhs, const matrix& rhs) {return lhs.M != rhs.M;}


    
    /// @brief i行目を1/c倍
    /// @param i 
    /// @param c 
    inline constexpr void row_transformation_division(int i, T c){
        for (int j = 0; j < W; j++){
            M[i][j] /= c;
        }
    }
    /// @brief i行目のc倍を、j行目から減算
    /// @param i 
    /// @param j 
    /// @param c 
    inline constexpr void row_transformation_sub_row(int i, T c, int j){
        for (int k = 0; k < W; k++){
            M[j][k] -= M[i][k]*c;
        }
    }

    inline constexpr void transpose(){
        swap(H,W);
        list<list<T>> temp(H, list<T>(W));
        for (int i = 0; i < W; i++){
            for (int j = 0; j < H; j++){
                temp[j][i] = std::move(M[i][j]);
            }
        }
        M = std::move(temp);
    }

    inline constexpr ll size() const {return H;}
    inline constexpr ll rowsize() const {return H;}
    inline constexpr ll columsize() const {return W;}
    inline constexpr bool empty() const {return min(H,W) == 0;}
    inline constexpr list<T>& operator[](const int row){
        return M[row];
    }
    inline constexpr const list<T>& operator[](const int row)const{
        return M[row];
    }

    /// @brief 行列を2次元累積和テーブルに変換する。
    void cumulative2d(){
        for (int i = 0; i < H; i++){
            for (int j = 1; j < W; j++){
                M[i][j] += M[i][j-1];
            }
        }
        for (int i = 1; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] += M[i-1][j];
            }
        }
    }
    T sum_from_origin(int r, int c){
        if (r < 0 || c < 0){return 0;}
        return M[r][c];
    }
    /// @brief r1 <= 行番号 < r2, c1 <= 列番号 < c2 を満たすような部分の総和を求める。
    /// @param r1 
    /// @param r2 
    /// @param c1 
    /// @param c2 
    /// @return 和
    T rectangle_sum(int r1, int c1, int r2, int c2){
        if (r1 > r2 || c1 > c2){return 0;}
        return sum_from_origin(r2-1, c2-1) - sum_from_origin(r2-1, c1-1) - sum_from_origin(r1-1, c2-1) + sum_from_origin(r1-1, c1-1);
    }
};



/// @brief M^Nを返す。
template<typename matrixT>
matrixT matrixpow(matrixT M, ull N){
    matrixT R(M.H);
    if (N){
        while (N){
            if (N%2){
                R *= M;
            }
            M *= M;
            N /= 2;
        }
        return R;
    }
    else{
        return R;
    }
}
/// @brief 与えられた行列を行簡約化する
template<typename matrixT>
matrixT row_simplification(matrixT M){
    int non_zero_column = 0;
    for (int i = 0; i < M.H; i++){
        bool finished = false;
        while (!finished){
            if (non_zero_column == M.W){
                return M;
            }
            for (int k = i; k < M.H; k++){
                if (M[k][non_zero_column] != 0){
                    swap(M[i],M[k]);
                    M.row_transformation_division(i, M[i][non_zero_column]);
                    for (int l = 0; l < M.H; l++){
                        if (l == i){continue;}
                        M.row_transformation_sub_row(i, M[l][non_zero_column], l);
                    }
                    finished = true;
                    break;
                }
            }
            non_zero_column++;
        }
    }
    return M;
}
/// @brief 正方行列の逆行列を求める。第一引数がfalseなら正則行列でない。
template<typename matrixT>
pair<bool, matrixT> inverse_matrix(matrixT M){
    if (M.H != M.W){
        cerr << "SizeError: not a square matrix" << endl;
        assert(false);
    }
    for (int i = 0; i < M.H; i++){
        for (int j = 0; j < M.W; j++){
            M[i].append(0);
        }
        M[i][M.W+i] = 1;
    }
    M.W *= 2;
    M = row_simplification(M);
    M.W /= 2;
    bool regular = true;
    for (ll i = 0; i < M.H; i++) if (M[i][i] == 0) regular = false;
    for (int i = 0; i < M.H; i++){
        M[i].erase(M[i].begin(), M[i].begin()+M.W);
    }
    return make_pair(regular, M);
}

/// @brief 拡大係数行列Aで表される連立方程式を解く
/// @attention 返り値は解空間の次元をrとして {{解の1つ}, {v1},{v2},...,{vr}} の形式である。解がなければ空となる。
template<typename matrixT>
list<list<typename matrixT::value_type>> solve_linear_equations(const matrixT &A){
    int M = A.H;
    int N = A.W-1;
    auto B = row_simplification(A);
    int r = 0;//解空間の次元
    list<int> c;//各行の先頭項の列のindex
    list<int> d(N, -1);//先頭項を含まない列が何番目にあるか
    int temp = 0, pre = 0;
    for (int i = 0; i < M; i++){
        bool all_zero = true;
        for (int j = pre; j < N; j++){
            if (!(B[i][j] == typename matrixT::value_type(0))){
                c.append(j);
                pre = j+1;
                all_zero = false;
                break;
            }
            else{
                d[j] = temp;
                temp++;
                pre++;
            }
        }
        if (all_zero){
            break;
        }
        r++;
    }
    for (int j = pre; j < N; j++){
        d[j] = temp;
        temp++;
    }

    for (ll i = r; i < M; i++){
        if (!(B[i][N] == typename matrixT::value_type(0))){return {};}
    }

    list<list<typename matrixT::value_type>> ans(N-r+1, list<typename matrixT::value_type>(N,0));
    for (int i = 0; i < r; i++){
        ans[0][c[i]] += B[i][N];
        for (int j = c[i]+1; j < N; j++){
            if (d[j] >= 0){
                ans[d[j]+1][c[i]] -= B[i][j];
            }
        }
    }
    for (int j = 0; j < N; j++){
        if (d[j] >= 0){
            ans[d[j]+1][j] = 1;
        }
    }
    return ans;
}

/// @brief 素数mod限定の行列式
template<typename matrixT> constexpr matrixT::value_type det_mod_P(matrixT A){
    if (A.H != A.W){
        cerr << "SizeError: not a square matrix" << endl;
        assert(false);
    }
    if (!MillerRabin<ull>(matrixT::value_type::mod_value)){
        cerr << "MOD is not a prime number" << endl;
        assert(false);
    }
    int non_zero_column = 0;
    int swapcnt = 0;
    typename matrixT::value_type det = 1;
    for (int i = 0; i < A.H; i++){
        bool finished = false;
        while (!finished){
            if (non_zero_column == A.W){break;}
            for (int k = i; k < A.H; k++){
                if (A[k][non_zero_column] != 0){
                    swap(A[i],A[k]);
                    if (i != k){swapcnt++;}
                    det *= A[i][non_zero_column];
                    A.row_transformation_division(i, A[i][non_zero_column]);
                    for (int l = 0; l < A.H; l++){
                        if (l == i){continue;}
                        A.row_transformation_sub_row(i, A[l][non_zero_column], l);
                    }
                    finished = true;
                    break;
                }
            }
            non_zero_column++;
        }
    }
    for (int i = 0; i < A.H; i++){det *= A[i][i];}
    return (swapcnt&1) ? (-1)*det : det;
}




#endif /* MATRIX__HPP_ */