# Competitive Programming Library made by $2^{51}$
2251799813685248が作った競技プログラミング用ライブラリ

## description
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
