# パラメトリック整数計画問題(PIP)ソルバー 仕様書

## 概要
- 入力されたPIPをislpyなどのライブラリを利用して解く。
- 結果はC++とpythonのコードに変換され、次のような関数が出力される
```python
# 関数の例
def solve(p1, p2, p3):
    if p1 <= 3 and p2 <= 3 and p3 <= 3:
        return p1+p2+p3
    else:
        return p1+2*p2+p3
```

## 処理の流れ
次のような流れでPIPを解く関数を生成する。
```
自分のPIP記述
       ↓
isl Map / Set
       ↓
lexmax_pw_multi_aff()
       ↓
PwMultiAff / PwAff
       ↓
自作コードジェネレータ
       ↓
C++ / Python
```

## インターフェースについて

問題は、パラメーター $p$ と、係数行列 $A,B,c,d$ を用いて次のように表せる。

$f(p)=\max\{c^\top x:Ax\le Bp+d,\ x\in\mathbb Z^n, x \ge 0, p\ge 0\}$

この係数行列 $A,B,c,d$ を指定することで、関数が生成されるようにしたい。
このソルバーでは、pythonを用いてこの機能を実装する。

次のように係数を指定し、特定の関数を呼び出すことで結果が指定したファイルに書き出されるようになっている。
```python
A = [[1,2,3,4],[5,6,7,8]]
B = [[2,3,4],[5,6,7]]
c = [-1,-3,0,0]
d = [0,-5]

PIP_generate_py(A,B,c,d)
PIP_generate_cpp(A,B,c,d)

```