# PIPソルバー

`max { cᵀx : Ax ≤ Bp+d, x ∈ ℤⁿ, x ≥ 0, p ∈ ℤᵏ, p ≥ 0 }` の
**最適目的値**を返すPython/C++関数を生成します。解ベクトルではなく目的値を最大化します。

## 準備・実行例

このディレクトリで実行します。

```sh
python3 -m pip install -r requirements.txt
python3 example.py
```

```python
from pip_solver import PIP_generate_py, PIP_generate_cpp

# max 3*x, subject to 2*x <= p1 and x <= p2
A = [[2], [1]]
B = [[1, 0], [0, 1]]
c = [3]
d = [0, 0]

PIP_generate_py(A, B, c, d, "solution.py")
PIP_generate_cpp(A, B, c, d, "solution.cpp")

from solution import solve
assert solve(9, 3) == 9
assert solve(9, 10) == 12
```

出力先を省略すると、カレントディレクトリの `pip_generated.py` /
`pip_generated.cpp` に保存します。既存の出力先ファイルは上書きされ、返り値は
生成されたソースコード（`str`）です。親ディレクトリはあらかじめ作成してください。
第6引数または `function_name="answer"` で関数名を変更できます。

両言語を生成する場合、最適化結果を再利用できます。

```python
from pip_solver import PIPSolver

solver = PIPSolver(A, B, c, d)
solver.generate_py("solution.py")
solver.generate_cpp("solution.cpp")
print(solver.optimum)  # isl.PwAff
# ファイルに保存しない場合は solver.python_source() / solver.cpp_source()
```

## 入力と境界条件

- `A`: m行n列、`B`: m行k列、`c`: n要素、`d`: m要素。全係数は整数です。
  次元不整合、浮動小数点数、boolは `ValueError` になります。
  仕様書 `a.md` の例はAが4列、cが3要素なので、利用時に次元を揃えてください。
- 生成関数の引数は `p1, ..., pk`。Pythonでは整数として扱える値を受け付けます。
- 実行不能、または負のパラメーターにはPythonで `None`、C++で `std::nullopt` を返します。
  目的値が0の場合は正常な最適解です。
- 目的値が非有界で、実行可能なパラメーターが存在する場合は、**生成時**に
  `PIPUnboundedError` が発生します。A,cは固定なので、改善可能な非負の方向があれば
  全ての実行可能なパラメーターで目的値が非有界になります。
  変数自体が非有界でも、目的値が有限なら生成できます。
- パラメーターなし（Bの各行が空）、変数なし（cが空）も扱えます。
  制約がない `A=B=d=[]` の場合、パラメーター数は0と解釈します。

## 生成したC++の利用

C++17で利用できます。生成時の `integer_type` で引数・返り値・中間計算の整数型を選択します。

```python
PIP_generate_cpp(A, B, c, d, "solution.cpp", integer_type="cpp_int")  # 既定値
PIP_generate_cpp(A, B, c, d, "solution128.cpp", integer_type="__int128")
PIP_generate_cpp(A, B, c, d, "solution64.cpp", integer_type="long long")

# PIPSolverのメソッドでも同じ引数を指定できます。
solver.generate_cpp("solution64.cpp", "answer", integer_type="long long")
source = solver.cpp_source(function_name="answer", integer_type="__int128")
```

| integer_type | 生成する整数型 | 必要な環境 |
| --- | --- | --- |
| `"cpp_int"`（既定値） | `boost::multiprecision::cpp_int`（任意精度） | Boostヘッダー |
| `"__int128"` | 符号付き128ビット整数 | GCC/Clangなどの対応コンパイラー |
| `"long long"` | 符号付き64ビット整数 | 64ビットの `long long` を持つ環境 |

`"boost::multiprecision::cpp_int"` も `"cpp_int"` と同じ意味で指定できます。
固定長の2種類ではBoostをインクルードしません。生成コード内の定数が選択した型の範囲を
超える場合は、生成時に `OverflowError` が発生します。実行時のオーバーフロー検査は
行わないため、入力・返り値・途中の計算結果が全て選択した型の範囲に収まる必要があります。

isl/islpyへの実行時依存はありません。生成したPythonも標準ライブラリだけで動作します。

```cpp
#include "solution.cpp"
#include <iostream>

int main() {
    auto value = solve(9, 3);
    if (value.has_value()) std::cout << *value << '\n';
}
```

```sh
g++ -std=c++17 main.cpp -o main
./main
```

`__int128` を選ぶ場合、`std::cout` で表示するには別途文字列への変換が必要です。

## 処理と検証

係数から補助変数 `v=cᵀx` を持つisl Setを作り、xを存在量化します。
`lexmax_pw_multi_aff()` でvの最大値を求め、得られたPwAffの各領域と式を
isl AST経由でPython/C++に変換します。負数の床除算と0方向への除算を区別します。
[islpyのAST API](https://documen.tician.de/islpy/ref_ast.html) を利用しています。

```sh
python3 -m unittest -v test_pip_solver
```

小規模なランダム問題の全探索比較、整数除算、合同条件、実行不能、非有界、
空の次元、巨大整数、各整数型の最大値・最小値、3種類の整数型で生成したC++の
コンパイルとPythonとの比較を含みます。C++テストでは未定義動作の検査も有効にしています。
パラメトリック整数計画の計算量や生成される場合分けの数は、問題により大きくなります。
