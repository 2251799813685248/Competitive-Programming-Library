# Competitive Programming Library made by $2^{51}$
2251799813685248が作った競技プログラミング用ライブラリ

## description

### all_direction_treedp
- 抽象化全方位木DPを使う。詳細はファイル内にあるABC428E. Farthest Vertexの解法を参照。

### convolution
- mod $998244353$ , mod $1224736769$ 上での畳み込みを配列長を $N $として計算量 $O(N\log N)$ で行う。

### kmp_search
- kmp法による文字列検索を行う。
- 文字列Sの中に文字列Tがあるかを調べる。ある場合はその最初の位置を、なければ-1を返す。
- ```kmp_search(S,T)```で実行できる。

### lll
- 多倍長整数

### math
- #### ```p_fact(ll x)```
  - 単一の正の整数を素因数分解する。
- #### ```Eratosthenes```,```Eratosthenes.p_fact(ll N)```
  - エラトステネスの篩を用いて、1以上N以下の自然数を $O(N\log N)$ で素因数分解をする。

### AVLset/map...AVL木による重複なし集合/連想配列
- 基本的な機能は```std::set```,```std::map```と同じ。
- index(0-indexed)によるアクセス```[k]```に対応。(計算量は $O(\log N)$ )
- あるイテレーターが指す要素のインデックスも $O(\log N)$ で取得可能。

### AVLmultiset...AVL木による多重集合
- 基本的な機能は```std::multiset```と同じ。
- index(0-indexed)によるアクセス```[k]```に対応。(計算量は $O(\log N)$ )

### FenwickTree...フェニックス木、Binary Indexed Tree
- ```range_sum(l,r)```,```update(idx,value)```,```update(idx,a,b)```に対応。

### Graph構造体
- #### NoWeightGraph
  - 始点をいくつか指定してBFS, 強連結成分分解ができる。
- #### WeightedGraph
  - 始点をいくつか指定してダイクストラ法を実行できる。
 

