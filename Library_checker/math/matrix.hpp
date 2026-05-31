#ifndef MATRIX__HPP_
#define MATRIX__HPP_

#include <vector>
#include <string>
#include <cmath>
using namespace std;
using ll = long long;



/// @brief 行列
/// @attention 行,列共に0-indexed
/// @attention コンストラクタにHとWを渡すと、[0][0]~[H-1][W-1]までできる。
/// @attention コンストラクタ1 matrix(N)...N次単位行列
/// @attention コンストラクタ2 matrix(h,w,v)...全成分がvの行列
/// @attention コンストラクタ3 matrix(vecvec)...2次元vectorで初期化
/// @tparam T 数を表す型
template<typename T>
struct matrix{
    vector<vector<T>> M;
    int H,W;

    /// @brief N次単位行列を生成
    /// @param N 
    matrix(size_t N){
        H = N;W = N;
        M = vector<vector<T>>(N,vector<T>(N,0));
        for (int i = 0; i < N; i++){
            M[i][i] = 1;
        }
    }
    /// @brief h×w型の、全要素がvの行列を生成
    /// @param h 
    /// @param w 
    /// @param v 
    matrix(int h, int w, T v){
        H = h;
        W = w;
        M = vector<vector<T>>(H,vector<T>(W,v));
    }
    /// @brief 2次元配列を用いて行列を生成
    /// @param A 
    matrix(const vector<vector<T>> &A){
        M = A;
        H = A.size();
        W = A[0].size();
    }
    
    matrix operator+(const matrix &A)const{
        if (H != A.H  || W != A.W){
            cerr << "size error(operator+)" << endl;
            assert(false);
        }
        matrix ans(M);
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                ans.M[i][j] += A.M[i][j];
            }
        }
        return ans;
    }
    void operator+=(const matrix &A){
        if (H != A.H  || W != A.W){
            cerr << "size error(operator+=)" << endl;
            assert(false);
        }
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] += A.M[i][j];
            }
        }
    }
    matrix operator-(const matrix &A)const{
        if (H != A.H  || W != A.W){
            cerr << "size error(operator-)" << endl;
            assert(false);
        }
        matrix ans(M);
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                ans.M[i][j] -= A.M[i][j];
            }
        }
        return ans;
    }
    void operator-=(const matrix &A){
        if (H != A.H  || W != A.W){
            cerr << "size error(operator-=)" << endl;
            assert(false);
        }
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] -= A.M[i][j];
            }
        }
    }
    matrix operator*(const matrix &A)const{
        if (W != A.H){
            cerr << "size error(operator*)" << endl;
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
        return ans;
    }
    void operator*=(const matrix &A){
        if (W != A.H){
            cerr << "size error(operator*=)" << endl;
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
    }
    matrix operator*(const T c)const{
        matrix ans(H,W,0);
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                ans.M[i][j] = M[i][j] * c;
            }
        }
        return ans;
    }
    void operator*=(const T c){
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                M[i][j] *= c;
            }
        }
    }

    
    /// @brief i行目を1/c倍
    /// @param i 
    /// @param c 
    void row_transformation_division(size_t i, T c){
        for (int j = 0; j < W; j++){
            M[i][j] /= c;
        }
    }
    /// @brief i行目のc倍を、j行目から減算
    /// @param i 
    /// @param j 
    /// @param c 
    void row_transformation_sub_row(size_t i, T c, size_t j){
        for (int k = 0; k < W; k++){
            M[j][k] -= M[i][k]*c;
        }
    }

    /// @brief 行の数を返す
    size_t size()const{return H;}
    bool empty()const{return H == 0;}
    inline vector<T>& operator[](const int row){
        return M[row];
    }
    inline const vector<T>& operator[](const int row)const{
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
    /// @brief r1<=行番号<=r2, c1<=列番号<=c2を満たすような部分の総和を求める。
    /// @param r1 
    /// @param r2 
    /// @param c1 
    /// @param c2 
    /// @return 和
    T rectangle_sum(int r1, int r2, int c1, int c2){
        if (r1 > r2 || c1 > c2){return 0;}
        return sum_from_origin(r2, c2) - sum_from_origin(r2, c1-1) - sum_from_origin(r1-1, c2) + sum_from_origin(r1-1, c1-1);
    }
};

/// @brief M^Nを返す。
template<typename T>
matrix<T> matrixpow(matrix<T> M, ll N){
    matrix<T> R(M.H);
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
template<typename T>
matrix<T> row_simplification(matrix<T> M){
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
                        M.row_transformation_sub_row(i, M[l][non_zero_column]/M[i][non_zero_column], l);
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
template<typename T>
pair<bool, matrix<T>> inverse_matrix(matrix<T> M){
    if (M.H != M.W){
        cerr << "This is not a square matrix" << endl;
        assert(false);
    }
    for (int i = 0; i < M.H; i++){
        for (int j = 0; j < M.W; j++){
            M[i].push_back(0);
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
template<typename T>
vector<vector<T>> solve_linear_equations(const matrix<T> &A){
    int M = A.H;
    int N = A.W-1;
    auto B = row_simplification(A);
    int r = 0;//解空間の次元
    vector<int> c;//各行の先頭項の列のindex
    vector<int> d(N, -1);//先頭項を含まない列が何番目にあるか
    int temp = 0, pre = 0;
    for (int i = 0; i < M; i++){
        bool all_zero = true;
        for (int j = pre; j < N; j++){
            if (!(B[i][j] == (T)0)){
                c.push_back(j);
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
        if (!(B[i][N] == (T)0)){return {};}
    }

    vector<vector<T>> ans(N-r+1, vector<T>(N,0));
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




#endif /* MATRIX__HPP_ */