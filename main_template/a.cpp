#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <functional>
#include <random>
#include <bitset>


using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = array<int,2>;
using pll = array<ll,2>;

#define vall(A) A.begin(), A.end()
template<typename T> inline void vin(vector<T>& A){for (int i = 0, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void vout(const vector<T>& A){for (int i = 0, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void vout2d(const vector<vector<T>>& A){for (int i = 0, H = A.size(); i < H; i++){vout(A[i]);}}
template<typename T> inline void adjvin(vector<T>& A){for (int i = 1, sz = A.size(); i < sz; i++){cin >> A[i];}}
template<typename T> inline void adjvout(const vector<T>& A){for (int i = 1, sz = A.size(); i < sz; i++){cout << A[i] << " \n"[i == sz-1];}}
template<typename T> inline void adjvout2d(const vector<vector<T>>& A){for (int i = 1, H = A.size(); i < H; i++){adjvout(A[i]);}}
template<typename T> inline bool btest(T K, int i){return K&(1ull<<i);}
template<typename T> void print(T object){cout << (object) << "\n";}
template<typename T, typename U> void print(T object1, U object2){cout << (object1) << " " << (object2) << "\n";}
template<typename T, typename U, typename V> void print(T object1, U object2, V object3){cout << (object1) << " " << (object2) << " " << (object3) << "\n";}
template<typename T, typename U, typename V, typename W> void print(T object1, U object2, V object3, W object4){cout << (object1) << " " << (object2) << " " << (object3) << " " << (object4) << "\n";}
const vector<ull> pow2ll{1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904, 9223372036854775808ull};
const vector<ull> pow10ll{1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000,100000000000000,1000000000000000,10000000000000000,100000000000000000,1000000000000000000, 10000000000000000000ull};
const vector<ll> di{0,1,0,-1};
const vector<ll> di8{0,1,1,1,0,-1,-1,-1};
const vector<ll> dj{1,0,-1,0};
const vector<ll> dj8{1,1,0,-1,-1,-1,0,1};



namespace a1073741824{

namespace AVL{

//AVLset.hpp
/// @brief AVL木による重複なし集合
/// @tparam T 木に載せる要素の型
template <typename T>
struct AVLset {
    private:
    /// @brief AVLset用ノード
    /// @tparam TTT
    template<typename TTT>
    struct AVLsetNode{
        TTT key; //値
        AVLsetNode* left;
        AVLsetNode* right;
        AVLsetNode* parent; //親へのポインタ
        int size; //この部分木に含まれるノードの総数
        int height; //部分木から各ノードへの経路長の最大値

        AVLsetNode(TTT k, AVLsetNode* p = nullptr) : key(k), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
    };

    int treesize = 0;//全体の要素数

    public:
    AVLsetNode<T>* root;

    AVLset() : root(nullptr) {}


    ///双方向イテレーターの実装
    class iterator {
    public:
        AVLsetNode<T>* current;
        const AVLset<T>* tree; // 所属する木へのポインタ

        // コンストラクタで木への参照を受け取る
        iterator(AVLsetNode<T>* node, const AVLset<T>* t) : current(node), tree(t) {}

        T& operator*() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return current->key;
        }
        T* operator->() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return &current->key;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }

        // --- 前方移動 (++) ---
        iterator& operator++() {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLsetNode<T>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        // --- 前方移動 (++(int)) ---
        void operator++(int) {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLsetNode<T>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
        }

        // --- 後方移動 (--) ---
        iterator& operator--() {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return *this;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLsetNode<T>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
            return *this;
        }

        // --- 後方移動 (--(int)) ---
        void operator--(int) {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLsetNode<T>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
        }
    };


    /// @brief 最小値を指すイテレーターを取得する。
    iterator begin() const {
        return iterator(getMinNode(root), this);
    }
    /// @brief 最大値を指すイテレーターを取得する。 
    iterator prevend() const{
        return iterator(getMaxNode(root),this);
    }
    /// @brief 最大値の次を指すイテレーターを取得する。 
    iterator end() const {
        return iterator(nullptr, this);
    }

    // --- ヘルパー関数（公開・内部兼用） ---
    AVLsetNode<T>* getMinNode(AVLsetNode<T>* n) const {
        if (!n) return nullptr;
        while (n->left) n = n->left;
        return n;
    }

    AVLsetNode<T>* getMaxNode(AVLsetNode<T>* n) const {
        if (!n) return nullptr;
        while (n->right) n = n->right;
        return n;
    }


    /// @brief 指定したkeyを挿入する。すでにあれば何も起きない
    /// @param key 
    void insert(const T& key) {
        root = insertNode(root, nullptr, key);
    }

    /// @brief 指定した要素を指すイテレーターを返す。なければend()が返される。
    /// @param key 
    /// @return 
    iterator find(const T& key) {
        AVLsetNode<T>* curr = root;
        while (curr) {
            if (key == curr->key) return iterator(curr, this);
            else if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return end();
    }


    // イテレータが指す要素を1つだけ削除し、次の要素へのイテレータを返す
    iterator eraseone(iterator pos) {
        if (pos == end()) return end();

        AVLsetNode<T>* targetNode = pos.current; // イテレータからノードを取得

        // ケースA: 子が2つある場合
        // この場合、deleteNodeDirect内部で「後継ノード(nextItが指す先)」が物理的に削除され、
        // その値が targetNode にコピーされます。
        // つまり、targetNode はメモリ上には残りますが、中身が「次の要素」に入れ替わります。
        if (targetNode->left != nullptr && targetNode->right != nullptr) {
            
            // 削除操作を実行
            // (内部で targetNode->key が書き換わり、後継ノードが delete されます)
            deleteNodeDirect(targetNode);

            // targetNode の場所には「削除した値の次の値」が入っているため、
            // 更新された targetNode を指すイテレータを返せば正解です。
            // (pos は内部的に targetNode を指しているので、そのまま pos を返せばOKですが、
            //  念のため再構築して返します)
            return pos;
            //return iterator(targetNode, this);
        } 
        
        // ケースB: 子が0個 または 1個の場合
        // この場合、targetNode そのものが物理的に delete されます。
        // そのため、削除前に「次のイテレータ」を確保しておく従来の方法が有効です。
        else {
            iterator nextIt = pos;
            ++nextIt; // 次へ進めておく

            deleteNodeDirect(targetNode);
            
            return nextIt; // 退避しておいたイテレータを返す
        }
    }

    // 削除
    iterator eraseone(const T& key) {
        return eraseone(find(key));
    }

    // 範囲 [first, last) の要素をすべて削除する
    // 戻り値: 削除された範囲の直後のイテレータ（つまり last）
    iterator range_erase(iterator first, iterator last) {
        while (first != last) {
            AVLsetNode<T>* node = first.current; // Iteratorのcurrentにアクセスできる前提

            // 【バグ修正】
            // firstが2つの子を持つ場合、内部実装では「後継ノード(右の最小)」が物理的に削除されます。
            // もし、その「後継ノード」が「last」そのものであった場合、
            // eraseを実行すると last イテレーターが無効化(メモリ解放)されてしまいます。
            if (node->left != nullptr && node->right != nullptr) {
                
                // 物理的に削除される予定の後継ノードを探す
                AVLsetNode<T>* successor = node->right;
                while (successor->left != nullptr) {
                    successor = successor->left;
                }

                // 「終了地点(last)」が「物理削除されるノード」と一致してしまった場合
                if (last.current == successor) {
                    // この場合、範囲 [first, last) には first 1つしか要素がありません。
                    // (successorはBSTにおいて直後の要素であるため)
                    
                    // 1. 削除実行
                    //    値のSwapが行われ、successor(元のlast)がdeleteされます。
                    //    この時点で引数の last は無効になります。
                    eraseone(first);

                    // 2. ループ終了
                    //    元の first ノードには、コピーされた successor(last) の値が入っています。
                    //    つまり、現在の node こそが、新しい「last」相当の位置です。
                    //    無効になった last と比較する前に、正しいイテレータを返して終了します。
                    return iterator(node, this);
                }
            }

            // 通常ケース: last が巻き添えになることはないので安全に進める
            first = eraseone(first);
        }
        return last;
    }

    /// @brief 指定した要素keyがあるかを検索する。
    /// @param key 
    /// @return keyがあればtrue なければfalse
    int count(const T& key) {
        AVLsetNode<T>* current = root;
        while (current != nullptr) {
            if (key == current->key) return 1;
            else if (key < current->key) current = current->left;
            else current = current->right;
        }
        return 0;
    }

    /// @brief 要素数を取得
    /// @return 要素数
    int size(){
        return treesize;
    }

    /// @brief 空であるか判定
    /// @return treesize==0
    bool empty(){
        return treesize == 0;
    }

    /// @brief 指定したkey以上の最小要素を指すイテレーターを取得する。なければend()が返される。
    /// @param key 
    /// @return iterator
    iterator lower_bound(const T& key){
        AVLsetNode<T>* curr = root;
        AVLsetNode<T>* ok_node = nullptr;
        while (curr != nullptr){
            if (curr->key < key){
                curr = curr->right; 
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }
    
    /// @brief 指定したkeyより大きい最小要素を指すイテレーターを取得する。なければend()が返される。
    /// @param key 
    /// @return iterator
    iterator upper_bound(const T& key){
        AVLsetNode<T>* curr = root;
        AVLsetNode<T>* ok_node = nullptr;
        while (curr != nullptr){
            if (curr->key <= key){
                curr = curr->right;
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }

    /// @brief 全要素を消す。
    void clear(){
        deque<AVLsetNode<T>*> Q;
        Q.push_back(root);
        while (!Q.empty()){
            if (Q.front()->left != nullptr){
                Q.push_back(Q.front()->left);
            }
            if (Q.front()->right != nullptr){
                Q.push_back(Q.front()->right);
            }
            delete Q.front();
            Q.pop_front();
        }
        root = nullptr;//すべて初期化。
        treesize = 0;
    }

    /// @brief index(0-indexed)でk番目の要素にアクセスする。
    /// @param k 
    /// @return 
    iterator getkth(const int &k){
        if (k < 0){
            cerr << "Index out of bounds(index must be non-negative)" << endl;
            assert(false);
        }
        if (k >= treesize){
            return end();
            //cerr << "Index out of bounds(right out)" << endl;
            //assert(false);
        }
        return iterator(findKthNode(root, k), this);
    }

    iterator operator[](const int &k){
        return getkth(k);
    }

    /// @brief イテレーターが指す場所のindex(0-indexed)を返す。
    /// @param pos 
    /// @return index
    int getindex(const iterator& it){
        //end()のindexはtreesize
        if (it == end()) return treesize;

        AVLsetNode<T>* node = it.current; 
        
        // --- Step 1: 自分の左部分木のサイズをカウント ---
        int idx = getSize(node->left);

        // --- Step 2: 根に向かって遡る ---
        AVLsetNode<T>* curr = node;
        while (curr->parent != nullptr) {
            // 自分が親の「右の子」である場合
            if (curr == curr->parent->right) {
                // 親(1つ) + 親の左部分木(getSize) を加算
                idx += 1 + getSize(curr->parent->left);
            }
            
            // 上へ移動
            curr = curr->parent;
        }

        return idx;
    }


    /// @brief 全要素を小さい順に表示
    void display() {
        std::cout << "\n\n";
        inOrder(root);
        std::cout << "\n\n";
    }




private:
    // --- ヘルパー関数 ---

    //部分木サイズを取得
    int getSize(AVLsetNode<T>* n) {
        if (n == nullptr) return 0;
        return n->size;
    }

    // 高さとサイズを同時に更新する関数
    void updateNode(AVLsetNode<T>* n) {
        if (n != nullptr) {
            n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
            n->size = 1 + getSize(n->left) + getSize(n->right); // ★サイズ計算
        }
    }

    int getHeight(AVLsetNode<T>* n) {
        if (n == nullptr) return 0;
        return n->height;
    }

    int getBalance(AVLsetNode<T>* n) {
        if (n == nullptr) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    // 右回転 (親ポインタ更新付き)
    AVLsetNode<T>* rightRotate(AVLsetNode<T>* y) {
        AVLsetNode<T>* x = y->left;
        AVLsetNode<T>* T2 = x->right;

        // 回転
        x->right = y;
        y->left = T2;

        // 親の更新
        if (T2) T2->parent = y;
        x->parent = y->parent; // xはyの元の親を継承
        y->parent = x;         // yの親はxになる

        // 高さ、部分木の要素数更新
        updateNode(y);
        updateNode(x);

        return x;
    }

    // 左回転 (親ポインタ更新付き)
    AVLsetNode<T>* leftRotate(AVLsetNode<T>* x) {
        AVLsetNode<T>* y = x->right;
        AVLsetNode<T>* T2 = y->left;

        // 回転
        y->left = x;
        x->right = T2;

        // 親の更新
        if (T2) T2->parent = x;
        y->parent = x->parent;
        x->parent = y;

        // 高さ、部分木の要素数更新
        updateNode(x);
        updateNode(y);

        return y;
    }

    // 挿入ロジック (親ポインタの設定を追加)
    AVLsetNode<T>* insertNode(AVLsetNode<T>* node, AVLsetNode<T>* parent, const T& key) {
        // 1. 通常のBST挿入
        if (node == nullptr){// 親をセット
            treesize++;
            return new AVLsetNode<T>(key, parent);
        }
        
        if (key < node->key)
            node->left = insertNode(node->left, node, key);
        else if (key > node->key)
            node->right = insertNode(node->right, node, key);
        else
            return node;

        // 2. 高さ更新
        updateNode(node);

        // 3. バランスチェック & 回転
        int balance = getBalance(node);

        // LL
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);
        // RR
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);
        // LR
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // RL
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    
    // 特定のノードを物理的に削除し、親を遡ってバランス調整する
    void deleteNodeDirect(AVLsetNode<T>* node) {
        if (node == nullptr) return;

        treesize--;//削除されるので一個減らす

        // ---------------------------------------------------
        // 1. 子が2つある場合
        //    後継者（右の最小）と値を入れ替え、削除対象を後継者にする
        // ---------------------------------------------------
        if (node->left != nullptr && node->right != nullptr) {
            AVLsetNode<T>* successor = getMinNode(node->right);
            
            // 値だけコピー（本来はstd::moveなどが好ましい）
            node->key = successor->key;
            
            // 削除対象を successor に変更して、以下の「子0or1」の処理に流す
            node = successor;
        }

        // ---------------------------------------------------
        // 2. 子が0個 または 1個の場合（nodeは削除されるノード）
        // ---------------------------------------------------
        AVLsetNode<T>* child = (node->left != nullptr) ? node->left : node->right;
        AVLsetNode<T>* parent = node->parent;

        // 子がいる場合、子の親ポインタを更新
        if (child != nullptr) {
            child->parent = parent;
        }

        // 親のリンクを更新
        if (parent == nullptr) {
            // 根を削除する場合
            root = child;
        } else {
            if (node == parent->left) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        }

        // ノード自体のメモリ解放
        delete node;

        // ---------------------------------------------------
        // 3. 削除した場所の親からルートまで遡ってバランス調整
        // ---------------------------------------------------
        AVLsetNode<T>* curr = parent;
        while (curr != nullptr) {
            // 高さ、サイズの更新
            updateNode(curr);

            // バランスチェック
            int balance = getBalance(curr);
            
            // 親へのポインタを繋ぎ変えるため、回転後の新しい部分木の根を受け取る
            AVLsetNode<T>* newSubRoot = curr;

            // LL Case
            if (balance > 1 && getBalance(curr->left) >= 0) {
                newSubRoot = rightRotate(curr);
            }
            // LR Case
            else if (balance > 1 && getBalance(curr->left) < 0) {
                curr->left = leftRotate(curr->left);
                newSubRoot = rightRotate(curr);
            }
            // RR Case
            else if (balance < -1 && getBalance(curr->right) <= 0) {
                newSubRoot = leftRotate(curr);
            }
            // RL Case
            else if (balance < -1 && getBalance(curr->right) > 0) {
                curr->right = rightRotate(curr->right);
                newSubRoot = leftRotate(curr);
            }

            // 回転が行われた場合、親から見たリンクも修正が必要
            // (rightRotate等の内部でparentは更新されるが、
            //  「親のleft/rightのどちらか」という情報はここで更新する)
            AVLsetNode<T>* p = newSubRoot->parent;
            if (p != nullptr) {
                if (curr == p->left) p->left = newSubRoot;
                else p->right = newSubRoot;
            } else {
                root = newSubRoot;
            }

            // 次の親へ（回転した場合は新しい親の上へ）
            curr = newSubRoot->parent;
        }
    }

    // 再帰探索ロジック
    AVLsetNode<T>* findKthNode(AVLsetNode<T>* node, int k) {
        if (node == nullptr){
            cerr << "There are fatal bugs in this AVL tree" << endl;
            assert(false);
        }
        // 左部分木のサイズを取得
        int leftSize = getSize(node->left);

        if (k == leftSize) {
            // 左側の数がちょうど k 個なら、現在のノードが k 番目 (0-indexed)
            return node;
        } else if (k < leftSize) {
            // k が左のサイズより小さいなら、左部分木の中に答えがある
            return findKthNode(node->left, k);
        } else {
            // k が左より大きいなら、右部分木へ。
            // 探すべき順位は、(k - 左の数 - 自分1つ分) になる
            return findKthNode(node->right, k - leftSize - 1);
        }
    }


    void inOrder(AVLsetNode<T>* roott) {
        if (roott != nullptr) {
            inOrder(roott->left);
            // T型を cout で出力できる必要がある
            std::cout << roott->key << " "; 
            inOrder(roott->right);
        }
    }
};

//AVLmap.hpp
/// @brief AVL木による連想配列
/// @tparam T key
/// @tparam U value
template <typename T, typename U>
struct AVLmap {
    private:
    /// @brief AVLmap用ノード
    /// @tparam TTT UUU
    template <typename TTT, typename UUU>
    struct AVLmapNode {
        TTT key; //連想配列のキー
        UUU value; //連想配列に入れる値
        AVLmapNode* left;
        AVLmapNode* right;
        AVLmapNode* parent; //親へのポインタ
        int size; //この部分木に含まれるノードの総数
        int height; //部分木から各ノードへの経路長の最大値

        AVLmapNode(TTT k, UUU v, AVLmapNode* p = nullptr) : key(k), value(v), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
    };

    int treesize = 0;//全体の要素数
    
    public:
    AVLmapNode<T,U>* root;

    AVLmap() : root(nullptr) {}


    ///双方向イテレーターの実装
    class iterator {
    public:
        AVLmapNode<T,U>* current;
        const T &key = current->key;
        U &value = current->value;
        const AVLmap<T,U>* tree; // 所属する木へのポインタ

        // コンストラクタで木への参照を受け取る
        iterator(AVLmapNode<T,U>* node, const AVLmap<T,U>* t) : current(node), tree(t) {}

        pair<T,U> operator*(){
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return make_pair(current->key, current->value);
        }
        pair<T,U>* operator->(){
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return &current->key;
        }

        

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }

        // --- 前方移動 (++) ---
        iterator& operator++() {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLmapNode<T,U>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        // --- 前方移動 (++(int)) ---
        void operator++(int) {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLmapNode<T,U>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
        }

        // --- 後方移動 (--) ---
        iterator& operator--() {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return *this;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLmapNode<T,U>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
            return *this;
        }

        // --- 後方移動 (--(int)) ---
        void operator--(int) {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLmapNode<T,U>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
        }
    };


    /// @brief 最小値を指すイテレーターを取得する。
    iterator begin() const {
        return iterator(getMinNode(root), this);
    }
    /// @brief 最大値を指すイテレーターを取得する。 
    iterator prevend() const{
        return iterator(getMaxNode(root),this);
    }
    /// @brief 最大値の次を指すイテレーターを取得する。 
    iterator end() const {
        return iterator(nullptr, this);
    }

    // --- ヘルパー関数（公開・内部兼用） ---
    AVLmapNode<T,U>* getMinNode(AVLmapNode<T,U>* n) const {
        if (!n) return nullptr;
        while (n->left) n = n->left;
        return n;
    }

    AVLmapNode<T,U>* getMaxNode(AVLmapNode<T,U>* n) const {
        if (!n) return nullptr;
        while (n->right) n = n->right;
        return n;
    }


    /// @brief 指定した{key,value}を挿入する。すでにあれば何も起きない
    /// @param key 
    /// @param value
    void insert(const T& key, const U& value) {
        root = insertNode(root, nullptr, key, value);
    }

    /// @brief 指定した要素を指すイテレーターを返す。なければend()が返される。
    /// @param key 
    /// @return 
    iterator find(const T& key) {
        AVLmapNode<T,U>* curr = root;
        while (curr) {
            if (key == curr->key) return iterator(curr, this);
            else if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return end();
    }

    U& operator[](const T& key){
        bool succeed_to_insert = false;
        U* returned_pointer;
        root = insertNode_and_ret_velue(root, nullptr, key, succeed_to_insert, returned_pointer);
        return *returned_pointer;
    }


    // イテレータが指す要素を1つだけ削除し、次の要素へのイテレータを返す
    iterator eraseone(iterator pos) {
        if (pos == end()) return end();

        AVLmapNode<T,U>* targetNode = pos.current; // イテレータからノードを取得

        // ケースA: 子が2つある場合
        // この場合、deleteNodeDirect内部で「後継ノード(nextItが指す先)」が物理的に削除され、
        // その値が targetNode にコピーされます。
        // つまり、targetNode はメモリ上には残りますが、中身が「次の要素」に入れ替わります。
        if (targetNode->left != nullptr && targetNode->right != nullptr) {
            
            // 削除操作を実行
            // (内部で targetNode->key が書き換わり、後継ノードが delete されます)
            deleteNodeDirect(targetNode);

            // targetNode の場所には「削除した値の次の値」が入っているため、
            // 更新された targetNode を指すイテレータを返せば正解です。
            // (pos は内部的に targetNode を指しているので、そのまま pos を返せばOKですが、
            //  念のため再構築して返します)
            return pos;
            //return iterator(targetNode, this);
        } 
        
        // ケースB: 子が0個 または 1個の場合
        // この場合、targetNode そのものが物理的に delete されます。
        // そのため、削除前に「次のイテレータ」を確保しておく従来の方法が有効です。
        else {
            iterator nextIt = pos;
            ++nextIt; // 次へ進めておく

            deleteNodeDirect(targetNode);
            
            return nextIt; // 退避しておいたイテレータを返す
        }
    }

    // 削除
    iterator eraseone(const T& key) {
        return eraseone(find(key));
    }

    /// @brief 範囲 [first, last) の要素をすべて削除する
    /// @param first
    /// @param last
    /// @return last
    iterator range_erase(iterator first, iterator last) {
        while (first != last) {
            AVLmapNode<T,U>* node = first.current; // Iteratorのcurrentにアクセスできる前提

            // 【バグ修正】
            // firstが2つの子を持つ場合、内部実装では「後継ノード(右の最小)」が物理的に削除されます。
            // もし、その「後継ノード」が「last」そのものであった場合、
            // eraseを実行すると last イテレーターが無効化(メモリ解放)されてしまいます。
            if (node->left != nullptr && node->right != nullptr) {
                
                // 物理的に削除される予定の後継ノードを探す
                AVLmapNode<T,U>* successor = node->right;
                while (successor->left != nullptr) {
                    successor = successor->left;
                }

                // 「終了地点(last)」が「物理削除されるノード」と一致してしまった場合
                if (last.current == successor) {
                    // この場合、範囲 [first, last) には first 1つしか要素がありません。
                    // (successorはBSTにおいて直後の要素であるため)
                    
                    // 1. 削除実行
                    //    値のSwapが行われ、successor(元のlast)がdeleteされます。
                    //    この時点で引数の last は無効になります。
                    eraseone(first);

                    // 2. ループ終了
                    //    元の first ノードには、コピーされた successor(last) の値が入っています。
                    //    つまり、現在の node こそが、新しい「last」相当の位置です。
                    //    無効になった last と比較する前に、正しいイテレータを返して終了します。
                    return iterator(node, this);
                }
            }

            // 通常ケース: last が巻き添えになることはないので安全に進める
            first = eraseone(first);
        }
        return last;
    }

    /// @brief 指定した要素keyがあるかを検索する。
    /// @param key 
    /// @return keyがあればtrue なければfalse
    bool count(const T& key) {
        AVLmapNode<T,U>* current = root;
        while (current != nullptr) {
            if (key == current->key) return true;
            else if (key < current->key) current = current->left;
            else current = current->right;
        }
        return false;
    }

    /// @brief 要素数を取得
    /// @return 要素数
    int size(){
        return treesize;
    }

    /// @brief 空であるか判定
    /// @return treesize==0
    bool empty(){
        return treesize == 0;
    }

    /// @brief 指定したkey以上の最小要素を指すイテレーターを取得する。なければend()が返される。
    /// @param key 
    /// @return iterator
    iterator lower_bound(const T& key){
        AVLmapNode<T,U>* curr = root;
        AVLmapNode<T,U>* ok_node = nullptr;
        while (curr != nullptr){
            if (curr->key < key){
                curr = curr->right; 
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }
    
    /// @brief 指定したkeyより大きい最小要素を指すイテレーターを取得する。なければend()が返される。
    /// @param key 
    /// @return iterator
    iterator upper_bound(const T& key){
        AVLmapNode<T,U>* curr = root;
        AVLmapNode<T,U>* ok_node = nullptr;
        while (curr != nullptr){
            if (curr->key <= key){
                curr = curr->right;
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }

    /// @brief 全要素を消す。
    void clear(){
        deque<AVLmapNode<T,U>*> Q;
        Q.push_back(root);
        while (!Q.empty()){
            if (Q.front()->left != nullptr){
                Q.push_back(Q.front()->left);
            }
            if (Q.front()->right != nullptr){
                Q.push_back(Q.front()->right);
            }
            delete Q.front();
            Q.pop_front();
        }
        root = nullptr;//すべて初期化。
        treesize = 0;
    }

    /// @brief index(0-indexed)でk番目の要素にアクセスする。
    /// @param k 
    /// @return 
    iterator getkth(int k){
        if (k < 0){
            cerr << "Index out of bounds(index must be non-negative)" << endl;
            assert(false);
        }
        if (k >= treesize){
            return end();
            //cerr << "Index out of bounds(right out)" << endl;
            //assert(false);
        }
        return iterator(findKthNode(root, k), this);
    }

    /// @brief イテレーターが指す場所のindex(0-indexed)を返す。
    /// @param pos 
    /// @return index
    int getindex(const iterator& it){
        //end()のindexはtreesize
        if (it == end()) return treesize;

        AVLmapNode<T,U>* node = it.current; 
        
        // --- Step 1: 自分の左部分木のサイズをカウント ---
        int idx = getSize(node->left);

        // --- Step 2: 根に向かって遡る ---
        AVLmapNode<T,U>* curr = node;
        while (curr->parent != nullptr) {
            // 自分が親の「右の子」である場合
            if (curr == curr->parent->right) {
                // 親(1つ) + 親の左部分木(getSize) を加算
                idx += 1 + getSize(curr->parent->left);
            }
            
            // 上へ移動
            curr = curr->parent;
        }

        return idx;
    }


    /// @brief 全要素を小さい順に表示
    void display() {
        inOrder(root);
        std::cout << "\n";
    }




private:
    // --- ヘルパー関数 ---

    //部分木サイズを取得
    int getSize(AVLmapNode<T,U>* n) {
        if (n == nullptr) return 0;
        return n->size;
    }

    // 高さとサイズを同時に更新する関数
    void updateNode(AVLmapNode<T,U>* n) {
        if (n != nullptr) {
            n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
            n->size = 1 + getSize(n->left) + getSize(n->right); // ★サイズ計算
        }
    }

    int getHeight(AVLmapNode<T,U>* n) {
        if (n == nullptr) return 0;
        return n->height;
    }

    int getBalance(AVLmapNode<T,U>* n) {
        if (n == nullptr) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    // 右回転 (親ポインタ更新付き)
    AVLmapNode<T,U>* rightRotate(AVLmapNode<T,U>* y) {
        AVLmapNode<T,U>* x = y->left;
        AVLmapNode<T,U>* T2 = x->right;

        // 回転
        x->right = y;
        y->left = T2;

        // 親の更新
        if (T2) T2->parent = y;
        x->parent = y->parent; // xはyの元の親を継承
        y->parent = x;         // yの親はxになる

        // 高さ、部分木の要素数更新
        updateNode(y);
        updateNode(x);

        return x;
    }

    // 左回転 (親ポインタ更新付き)
    AVLmapNode<T,U>* leftRotate(AVLmapNode<T,U>* x) {
        AVLmapNode<T,U>* y = x->right;
        AVLmapNode<T,U>* T2 = y->left;

        // 回転
        y->left = x;
        x->right = T2;

        // 親の更新
        if (T2) T2->parent = x;
        y->parent = x->parent;
        x->parent = y;

        // 高さ、部分木の要素数更新
        updateNode(x);
        updateNode(y);

        return y;
    }

    // 挿入ロジック (親ポインタの設定を追加)
    AVLmapNode<T,U>* insertNode(AVLmapNode<T,U>* node, AVLmapNode<T,U>* parent, const T& key, const U& value){
        // 1. 通常のBST挿入
        if (node == nullptr){// 親をセット
            treesize++;
            return new AVLmapNode<T,U>(key, value, parent);
        }
        
        if (key < node->key)
            node->left = insertNode(node->left, node, key, value);
        else if (key > node->key)
            node->right = insertNode(node->right, node, key, value);
        else
            return node;

        // 2. 高さ更新
        updateNode(node);

        // 3. バランスチェック & 回転
        int balance = getBalance(node);

        // LL
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);
        // RR
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);
        // LR
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // RL
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    AVLmapNode<T,U>* insertNode_and_ret_velue(AVLmapNode<T,U>* node, AVLmapNode<T,U>* parent, const T& key, bool &succeed_to_insert, U*& returned_pointer){
         // 1. 通常のBST挿入
        if (node == nullptr){// 親をセット
            succeed_to_insert = true;
            treesize++;
            AVLmapNode<T,U>* tempnode = new AVLmapNode<T,U>(key, *(new U), parent);
            returned_pointer = &(tempnode->value);
            return tempnode;
        }
        
        pair<AVLmapNode<T,U>*, U*> temp;

        if (key < node->key){
            node->left = insertNode_and_ret_velue(node->left, node, key, succeed_to_insert, returned_pointer);
        }
        else if (key > node->key){
            node->right = insertNode_and_ret_velue(node->right, node, key, succeed_to_insert, returned_pointer);
        }
        else{
            returned_pointer = &(node->value);
            return node;
        }

        if (!succeed_to_insert){return node;}//もし新しく挿入してないなら木を回転させる必要はない

        // 2. 高さ更新
        updateNode(node);

        // 3. バランスチェック & 回転
        int balance = getBalance(node);

        // LL
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);
        // RR
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);
        // LR
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // RL
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }
    
    // 特定のノードを物理的に削除し、親を遡ってバランス調整する
    void deleteNodeDirect(AVLmapNode<T,U>* node) {
        if (node == nullptr) return;

        treesize--;//削除されるので一個減らす

        // ---------------------------------------------------
        // 1. 子が2つある場合
        //    後継者（右の最小）と値を入れ替え、削除対象を後継者にする
        // ---------------------------------------------------
        if (node->left != nullptr && node->right != nullptr) {
            AVLmapNode<T,U>* successor = getMinNode(node->right);
            
            // 値だけコピー（本来はstd::moveなどが好ましい）
            node->key = successor->key;
            
            // 削除対象を successor に変更して、以下の「子0or1」の処理に流す
            node = successor;
        }

        // ---------------------------------------------------
        // 2. 子が0個 または 1個の場合（nodeは削除されるノード）
        // ---------------------------------------------------
        AVLmapNode<T,U>* child = (node->left != nullptr) ? node->left : node->right;
        AVLmapNode<T,U>* parent = node->parent;

        // 子がいる場合、子の親ポインタを更新
        if (child != nullptr) {
            child->parent = parent;
        }

        // 親のリンクを更新
        if (parent == nullptr) {
            // 根を削除する場合
            root = child;
        } else {
            if (node == parent->left) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        }

        // ノード自体のメモリ解放
        delete node;

        // ---------------------------------------------------
        // 3. 削除した場所の親からルートまで遡ってバランス調整
        // ---------------------------------------------------
        AVLmapNode<T,U>* curr = parent;
        while (curr != nullptr) {
            // 高さ、サイズの更新
            updateNode(curr);

            // バランスチェック
            int balance = getBalance(curr);
            
            // 親へのポインタを繋ぎ変えるため、回転後の新しい部分木の根を受け取る
            AVLmapNode<T,U>* newSubRoot = curr;

            // LL Case
            if (balance > 1 && getBalance(curr->left) >= 0) {
                newSubRoot = rightRotate(curr);
            }
            // LR Case
            else if (balance > 1 && getBalance(curr->left) < 0) {
                curr->left = leftRotate(curr->left);
                newSubRoot = rightRotate(curr);
            }
            // RR Case
            else if (balance < -1 && getBalance(curr->right) <= 0) {
                newSubRoot = leftRotate(curr);
            }
            // RL Case
            else if (balance < -1 && getBalance(curr->right) > 0) {
                curr->right = rightRotate(curr->right);
                newSubRoot = leftRotate(curr);
            }

            // 回転が行われた場合、親から見たリンクも修正が必要
            // (rightRotate等の内部でparentは更新されるが、
            //  「親のleft/rightのどちらか」という情報はここで更新する)
            AVLmapNode<T,U>* p = newSubRoot->parent;
            if (p != nullptr) {
                if (curr == p->left) p->left = newSubRoot;
                else p->right = newSubRoot;
            } else {
                root = newSubRoot;
            }

            // 次の親へ（回転した場合は新しい親の上へ）
            curr = newSubRoot->parent;
        }
    }

    // 再帰探索ロジック
    AVLmapNode<T,U>* findKthNode(AVLmapNode<T,U>* node, int k) {
        if (node == nullptr){
            cerr << "There are fatal bugs in this AVL tree" << endl;
            assert(false);
        }
        // 左部分木のサイズを取得
        int leftSize = getSize(node->left);

        if (k == leftSize) {
            // 左側の数がちょうど k 個なら、現在のノードが k 番目 (0-indexed)
            return node;
        } else if (k < leftSize) {
            // k が左のサイズより小さいなら、左部分木の中に答えがある
            return findKthNode(node->left, k);
        } else {
            // k が左より大きいなら、右部分木へ。
            // 探すべき順位は、(k - 左の数 - 自分1つ分) になる
            return findKthNode(node->right, k - leftSize - 1);
        }
    }


    void inOrder(AVLmapNode<T,U>* roott) {
        if (roott != nullptr) {
            inOrder(roott->left);
            // T型を cout で出力できる必要がある
            std::cout << roott->key << ":" << roott->value << " "; 
            inOrder(roott->right);
        }
    }
};

//AVLmultiset.hpp
/// @brief AVL木による多重集合
/// @tparam T 
template <typename T>
struct AVLmultiset {
    public:
    template <typename TTT>
    struct AVLmultisetNode {
        TTT key; // intではなくT型にする
        AVLmultisetNode* left;
        AVLmultisetNode* right;
        AVLmultisetNode* parent; //親へのポインタ
        int size; //この部分木に含まれるノードの総数
        int height;

        AVLmultisetNode(TTT k, AVLmultisetNode* p = nullptr) : key(k), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
    };

    private: int treesize = 0;//全体の要素数
    public:
    AVLmultisetNode<T>* root;

    AVLmultiset() : root(nullptr) {}

    // ==========================================
    //  高機能イテレータの実装
    // ==========================================
    class iterator {
    public:
        AVLmultisetNode<T>* current;
        const AVLmultiset<T>* tree; // 所属する木へのポインタ

        // コンストラクタで木への参照を受け取る
        iterator(AVLmultisetNode<T>* node, const AVLmultiset<T>* t) : current(node), tree(t) {}

        T& operator*() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return current->key;
        }
        T* operator->() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return &current->key;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }

        // --- 前方移動 (++) ---
        iterator& operator++() {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLmultisetNode<T>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        // --- 前方移動 (++(int)) ---
        void operator++(int) {
            // 1. end() からの移動（不正操作）
            if (current == nullptr) {
                cerr << "Error: Incrementing end() iterator." << endl;
                assert(false);
            }

            // 2. 通常の移動ロジック
            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                AVLmultisetNode<T>* p = current->parent;
                while (p != nullptr && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
        }


        // --- 後方移動 (--) ---
        iterator& operator--() {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return *this;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLmultisetNode<T>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
            return *this;
        }

        // --- 後方移動 (--(int)) ---
        void operator--(int) {
            // 1. end() からの復帰（特殊処理）
            if (current == nullptr) {
                // 木が空の場合は戻れない
                if (tree->root == nullptr) {
                    cerr << "Error: Decrementing iterator of empty tree." << endl;
                    assert(false);
                }
                // 木の最大値（一番右）へ移動
                current = tree->getMaxNode(tree->root);
                return;
            }

            // 2. 通常の移動ロジック
            if (current->left != nullptr) {
                current = current->left;
                while (current->right != nullptr)
                    current = current->right;
            } else {
                AVLmultisetNode<T>* p = current->parent;
                while (p != nullptr && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                
                // 3. begin() の手前に戻ろうとした場合（不正操作）
                if (p == nullptr) {
                    // 移動先がない（＝自分が最小値だった）
                    cerr << "Error: Decrementing begin() iterator." << endl;
                    assert(false);
                }
                current = p;
            }
        }
    };

    // --- イテレータ取得 ---
    iterator begin() const{
        return iterator(getMinNode(root), this);
    }

    iterator prevend() const{
        return iterator(getMaxNode(root), this);
    }

    iterator end() const{
        return iterator(nullptr, this);
    }

    // --- ヘルパー関数（公開・内部兼用） ---
    AVLmultisetNode<T>* getMinNode(AVLmultisetNode<T>* n) const {
        if (!n) return nullptr;
        while (n->left) n = n->left;
        return n;
    }

    AVLmultisetNode<T>* getMaxNode(AVLmultisetNode<T>* n) const {
        if (!n) return nullptr;
        while (n->right) n = n->right;
        return n;
    }


    // 挿入 (const T& で参照渡しにしてコピーコスト削減)
    void insert(const T& key) {
        root = insertNode(root, nullptr, key);
    }

    /// @brief 指定した要素を指すイテレーターを返す。なければend()が返される。
    /// @param key 
    /// @return 
    iterator find(const T& key) {
        AVLmultisetNode<T>* curr = root;
        while (curr) {
            if (key == curr->key) return iterator(curr, this);
            else if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return end();
    }



    /// @brief イテレータが指す要素を1つだけ削除し、次の要素へのイテレータを返す
    iterator eraseone(iterator pos) {
        if (pos == end()) return end();

        AVLmultisetNode<T>* targetNode = pos.current; // イテレータからノードを取得

        // ケースA: 子が2つある場合
        // この場合、deleteNodeDirect内部で「後継ノード(nextItが指す先)」が物理的に削除され、
        // その値が targetNode にコピーされます。
        // つまり、targetNode はメモリ上には残りますが、中身が「次の要素」に入れ替わります。
        if (targetNode->left != nullptr && targetNode->right != nullptr) {
            
            // 削除操作を実行
            // (内部で targetNode->key が書き換わり、後継ノードが delete されます)
            deleteNodeDirect(targetNode);

            // targetNode の場所には「削除した値の次の値」が入っているため、
            // 更新された targetNode を指すイテレータを返せば正解です。
            // (pos は内部的に targetNode を指しているので、そのまま pos を返せばOKですが、
            //  念のため再構築して返します)
            return iterator(targetNode, this);
        } 
        
        // ケースB: 子が0個 または 1個の場合
        // この場合、targetNode そのものが物理的に delete されます。
        // そのため、削除前に「次のイテレータ」を確保しておく従来の方法が有効です。
        else {
            iterator nextIt = pos;
            ++nextIt; // 次へ進めておく

            deleteNodeDirect(targetNode);
            
            return nextIt; // 退避しておいたイテレータを返す
        }
    }

    // 値で検索して1つだけ削除
    iterator eraseone(const T& key) {
        return eraseone(find(key));
    }

    // 範囲 [first, last) の要素をすべて削除する
    // 戻り値: 削除された範囲の直後のイテレータ（つまり last）
    iterator range_erase(iterator first, iterator last) {
        while (first != last) {
            AVLmultisetNode<T>* node = first.current; // Iteratorのcurrentにアクセスできる前提

            // 【バグ修正】
            // firstが2つの子を持つ場合、内部実装では「後継ノード(右の最小)」が物理的に削除されます。
            // もし、その「後継ノード」が「last」そのものであった場合、
            // eraseを実行すると last イテレーターが無効化(メモリ解放)されてしまいます。
            if (node->left != nullptr && node->right != nullptr) {
                
                // 物理的に削除される予定の後継ノードを探す
                AVLmultisetNode<T>* successor = node->right;
                while (successor->left != nullptr) {
                    successor = successor->left;
                }

                // 「終了地点(last)」が「物理削除されるノード」と一致してしまった場合
                if (last.current == successor) {
                    // この場合、範囲 [first, last) には first 1つしか要素がありません。
                    // (successorはBSTにおいて直後の要素であるため)
                    
                    // 1. 削除実行
                    //    値のSwapが行われ、successor(元のlast)がdeleteされます。
                    //    この時点で引数の last は無効になります。
                    eraseone(first);

                    // 2. ループ終了
                    //    元の first ノードには、コピーされた successor(last) の値が入っています。
                    //    つまり、現在の node こそが、新しい「last」相当の位置です。
                    //    無効になった last と比較する前に、正しいイテレータを返して終了します。
                    return iterator(node, this);
                }
            }

            // 通常ケース: last が巻き添えになることはないので安全に進める
            first = eraseone(first);
        }
        return last;
    }

    // 検索
    bool count(const T& key) {
        AVLmultisetNode<T>* current = root;
        while (current != nullptr) {
            if (key == current->key) return true;
            else if (key < current->key) current = current->left;
            else current = current->right;
        }
        return false;
    }

    int size(){
        return treesize;
    }

    bool empty(){
        return treesize == 0;
    }

    iterator lower_bound(const T& key){
        AVLmultisetNode<T>* curr = root;
        AVLmultisetNode<T>* ok_node = nullptr;
        while (curr){
            if (curr->key < key){
                curr = curr->right; 
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }
    iterator upper_bound(const T& key){
        AVLmultisetNode<T>* curr = root;
        AVLmultisetNode<T>* ok_node = nullptr;
        while (curr){
            if (curr->key <= key){
                curr = curr->right;
            }
            else{
                ok_node = curr;
                curr = curr->left;
            }
        }
        return iterator(ok_node, this);
    }

    void clear(){
        deque<AVLmultisetNode<T>*> Q;
        Q.push_back(root);
        while (!Q.empty()){
            if (Q.front()->left != nullptr){
                Q.push_back(Q.front()->left);
            }
            if (Q.front()->right != nullptr){
                Q.push_back(Q.front()->right);
            }
            delete Q.front();
            Q.pop_front();
        }
        root = nullptr;
        treesize = 0;
    }

    /// @brief indexが指す要素にアクセスする
    /// @param k 
    /// @return 
    iterator getkth(int k){
        if (k < 0){
            cerr << "Index out of bounds" << endl;
            assert(false);
        }
        if (k >= treesize){
            return end();
        }
        return iterator(findKthNode(root, k), this);
    }

    /// @brief イテレーターが指す場所のindex(0-indexed)を返す。
    /// @param pos 
    /// @return index
    int getindex(const iterator& it){
        //end()のindexはtreesize
        if (it == end()) return treesize;

        AVLmultisetNode<T>* node = it.current; 
        
        // --- Step 1: 自分の左部分木のサイズをカウント ---
        int idx = getSize(node->left);

        // --- Step 2: 根に向かって遡る ---
        AVLmultisetNode<T>* curr = node;
        while (curr->parent != nullptr) {
            // 自分が親の「右の子」である場合
            if (curr == curr->parent->right) {
                // 親(1つ) + 親の左部分木(getSize) を加算
                idx += 1 + getSize(curr->parent->left);
            }
            
            // 上へ移動
            curr = curr->parent;
        }

        return idx;
    }

    

    // 表示
    void display() {
        PrintinOrder(root);
        std::cout << std::endl;
    }




private: //ここからprivateメンバ関数(内部実装などに使う。)

    //部分木サイズを取得
    int getSize(AVLmultisetNode<T>* n) {
        if (n == nullptr) return 0;
        return n->size;
    }

    // 高さとサイズを同時に更新する関数
    void updateNode(AVLmultisetNode<T>* n) {
        if (n != nullptr) {
            n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
            n->size = 1 + getSize(n->left) + getSize(n->right); // ★サイズ計算
        }
    }

    int getHeight(AVLmultisetNode<T>* n) {
        if (n == nullptr) return 0;
        return n->height;
    }

    int getBalance(AVLmultisetNode<T>* n) {
        if (n == nullptr) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    // 右回転 (親ポインタ更新付き)
    AVLmultisetNode<T>* rightRotate(AVLmultisetNode<T>* y) {
        AVLmultisetNode<T>* x = y->left;
        AVLmultisetNode<T>* T2 = x->right;

        // 回転
        x->right = y;
        y->left = T2;

        // 親の更新
        if (T2) T2->parent = y;
        x->parent = y->parent; // xはyの元の親を継承
        y->parent = x;         // yの親はxになる

        // 高さ、部分木の要素数更新
        updateNode(y);
        updateNode(x);

        return x;
    }

    // 左回転 (親ポインタ更新付き)
    AVLmultisetNode<T>* leftRotate(AVLmultisetNode<T>* x) {
        AVLmultisetNode<T>* y = x->right;
        AVLmultisetNode<T>* T2 = y->left;

        // 回転
        y->left = x;
        x->right = T2;

        // 親の更新
        if (T2) T2->parent = x;
        y->parent = x->parent;
        x->parent = y;

        // 高さ、部分木の要素数更新
        updateNode(x);
        updateNode(y);

        return y;
    }

    // 挿入ロジック (親ポインタの設定を追加)
    AVLmultisetNode<T>* insertNode(AVLmultisetNode<T>* node, AVLmultisetNode<T>* parent, const T& key) {
        // 1. 通常のBST挿入
        if (node == nullptr){// 親をセット
            treesize++;
            return new AVLmultisetNode<T>(key, parent);
        }
        
        if (key < node->key){
            node->left = insertNode(node->left, node, key);
        }
        else{
            node->right = insertNode(node->right, node, key);
        }

        // 2. 高さ更新
        updateNode(node);

        // 3. バランスチェック & 回転
        int balance = getBalance(node);

        // LL
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);
        // RR
        if (balance < -1 && key >= node->right->key)
            return leftRotate(node);
        // LR
        if (balance > 1 && key >= node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // RL
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    // 特定のノードを物理的に削除し、親を遡ってバランス調整する
    void deleteNodeDirect(AVLmultisetNode<T>* node) {
        if (node == nullptr) return;

        treesize--;//削除されるので一個減らす

        // ---------------------------------------------------
        // 1. 子が2つある場合
        //    後継者（右の最小）と値を入れ替え、削除対象を後継者にする
        // ---------------------------------------------------
        if (node->left != nullptr && node->right != nullptr){
            AVLmultisetNode<T>* successor = getMinNode(node->right);
            
            // 値だけコピー（本来はstd::moveなどが好ましい）
            node->key = move(successor->key);
            
            
            // 削除対象を successor に変更して、以下の「子0or1」の処理に流す
            node = successor;
        }

        // ---------------------------------------------------
        // 2. 子が0個 または 1個の場合（nodeは削除されるノード）
        // ---------------------------------------------------
        AVLmultisetNode<T>* child = (node->left != nullptr) ? node->left : node->right;
        AVLmultisetNode<T>* parent = node->parent;

        // 子がいる場合、子の親ポインタを更新
        if (child != nullptr) {
            child->parent = parent;
        }

        // 親のリンクを更新
        if (parent == nullptr) {
            // 根を削除する場合
            root = child;
        } else {
            if (node == parent->left) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        }

        // ノード自体のメモリ解放
        delete node;

        // ---------------------------------------------------
        // 3. 削除した場所の親からルートまで遡ってバランス調整
        // ---------------------------------------------------
        AVLmultisetNode<T>* curr = parent;
        while (curr != nullptr) {
            // 高さの更新
            updateNode(curr);

            // バランスチェック
            int balance = getBalance(curr);
            
            // 親へのポインタを繋ぎ変えるため、回転後の新しい部分木の根を受け取る
            AVLmultisetNode<T>* newSubRoot = curr;

            // LL Case
            if (balance > 1 && getBalance(curr->left) >= 0) {
                newSubRoot = rightRotate(curr);
            }
            // LR Case
            else if (balance > 1 && getBalance(curr->left) < 0) {
                curr->left = leftRotate(curr->left);
                newSubRoot = rightRotate(curr);
            }
            // RR Case
            else if (balance < -1 && getBalance(curr->right) <= 0) {
                newSubRoot = leftRotate(curr);
            }
            // RL Case
            else if (balance < -1 && getBalance(curr->right) > 0) {
                curr->right = rightRotate(curr->right);
                newSubRoot = leftRotate(curr);
            }

            // 回転が行われた場合、親から見たリンクも修正が必要
            // (rightRotate等の内部でparentは更新されるが、
            //  「親のleft/rightのどちらか」という情報はここで更新する)
            AVLmultisetNode<T>* p = newSubRoot->parent;
            if (p != nullptr) {
                if (curr == p->left) p->left = newSubRoot;
                else p->right = newSubRoot;
            } else {
                root = newSubRoot;
            }

            // 次の親へ（回転した場合は新しい親の上へ）
            curr = newSubRoot->parent;
        }
    }

    // 再帰探索ロジック
    AVLmultisetNode<T>* findKthNode(AVLmultisetNode<T>* node, int k) {
        // 左部分木のサイズを取得
        int leftSize = getSize(node->left);

        if (k == leftSize) {
            // 左側の数がちょうど k 個なら、現在のノードが k 番目 (0-indexed)
            return node;
        } else if (k < leftSize) {
            // k が左のサイズより小さいなら、左部分木の中に答えがある
            return findKthNode(node->left, k);
        } else {
            // k が左より大きいなら、右部分木へ。
            // 探すべき順位は、(k - 左の数 - 自分1つ分) になる
            return findKthNode(node->right, k - leftSize - 1);
        }
    }

    //全部の値を出力
    void PrintinOrder(AVLmultisetNode<T>* r) {
        if (r != nullptr) {
            PrintinOrder(r->left);
            // T型を cout で出力できる必要がある
            std::cout << r->key << " ";
            PrintinOrder(r->right);
        }
    }
};

template<typename T>
using set = AVLset<T>;
template<typename T>
using multiset = AVLmultiset<T>;
template<typename T, typename U>
using map = AVLmap<T,U>;

}
namespace math{

//math.hpp
/// @brief a^bをmで割った余りを返す。bに関して対数時間で計算できる。
/// @param a 
/// @param b 
/// @param m 
/// @return a^b%m
ll modpow(ll a, ll b, const ll m){
    ll t = a%m;
    ll ans = 1;
    while (b > 0){
        if (b%2){
            ans = (ans*t)%m;
        }
        b /= 2;
        t = (t*t)%m;
    }
    return ans;
}

/// @brief a^nを返す。bに関して線形時間で計算できる。
/// @param a 
/// @param n
/// @param m 
/// @return a^n
ll powll(ll a, ll n){
    ll r = 1;
    for (int i = 1; i <= n; i++){
        r *= a;
    }
    return r;
}

/// @brief floor(sqrt(N))を返す
ll isqrt(ll N){
    if (N){
        ll ok = 1;
        ll ng = min(N,2000000000LL);
        while (ng - ok >= 2){
            ll mid = (ok+ng)/2;
            if (mid*mid <= N){
                ok = mid;
            } 
            else{
                ng = mid;
            }
        }
        return ok;
    }
    else{return 0;}
}

/// @brief floor(log_a(L))を返す
/// @param a
/// @param L
ll ilog(ll a, ll L){
    __int128_t t = 1;
    ll ans = 0;
    while (t <= L){
        ans++;
        t *= a;
    }
    return ans-1;
}

/// @brief 正の整数Nを素因数分解する
/// @param N
/// @return vector<vector<ll> {{素因数1,個数}, {素因数2,個数}, {素因数3,個数}...}
vector<pll> p_fact(ll N){
    if (N == 1){
        return vector<pll> {{1,0}};
    }
    vector<pll> R;//戻り値用リスト

    const int M = isqrt(N);
    for (int i = 2; i <= M; i++){
        if (N % i == 0){
            ll divide_count = 0;
            while (N % i == 0){
                divide_count++;
                N /= i;
            }
            R.push_back({i,divide_count});
        }
    }
    if (N != 1){
        R.push_back({N,1});
    }
    return R;
}

/// @brief 素因数分解リストを受け取って約数関数の値を求める
template<typename T>
ll divisor_function(const vector<array<T,2>>& vv, ll K){
    if (vv[0][0] == 1){
        return 1;
    }
    ll R = 1;
    if (K == 0){
        for (auto x : vv){
            R *= x[1]+1;
        }
    }
    else{
        for (auto x : vv){
            ll r = powll(x[0],K);
            R *= (powll(r,x[1]+1) - 1)/(r - 1);
        }
    }
    return R;
}

/// @brief 素因数分解の結果pを受け取って、約数リストを生成する。
template<typename T>
vector<T> enumerate_divisor(const vector<array<T,2>>& p){
    vector<T> d{1};
    if (p[0][0] == 1){
        return d;
    }
    for (auto &v : p){
        int t = d.size();
        ll temp = 1;
        for (int w = 0; w < v[1]; w++){
            temp *= v[0];
            for (int i = 0; i < t; i++){
                d.push_back(d[i]*temp);
            }
        }
    }
    sort(vall(d));
    return d;
}

/// @brief 有理数のfloorを求める
/// @param y
/// @param x 
/// @return floor(y/x)
inline ll floor2(ll y, ll x){
    if ((x^y) > 0){
        x = abs(x);
        y = abs(y);
        return y/x;
    }
    else if ((x^y) < 0){
        x = abs(x);
        y = abs(y);
        return -((y+x-1)/x);
    }
    else{
        return y/x;
    }
}
/// @brief 有理数のceilを求める
/// @param y 
/// @param x 
/// @return 
inline ll ceil2(ll y, ll x){
    if ((x^y) > 0){
        x = abs(x);
        y = abs(y);
        return (y+x-1)/x;
    }
    else if ((x^y) < 0){
        x = abs(x);
        y = abs(y);
        return -(y/x);
    }
    else{
        return y/x;
    }
}

/// @brief 線形篩
/// @attention コンストラクタに整数Nを渡すことでN以下の整数を扱うことができる。
struct LinearSieve{
    vector<int> p_list;
    vector<int> lpf;
    //Nを渡すことで1以上N以下の整数を扱うことができる
    LinearSieve(int N): lpf(N+1,-1){
        lpf[1] = 1;
        int p_list_size = 0;
        for (int i = 2; i <= N; i++){
            if (lpf[i] < 0){
                p_list.push_back(i);
                p_list_size++;
                lpf[i] = i;
            }
            for (int j = 0; j < p_list_size && p_list[j] <= lpf[i] && p_list[j]*i <= N; j++){
                lpf[p_list[j]*i] = p_list[j];
            }
        }
    }
    vector<pii> p_fact(int x){
        if (x == 1){return {{1,0}};}
        vector<pii> r;
        do{
            if (r.empty() || lpf[x] != r.back()[0]){
                r.push_back({lpf[x], 1});
            }
            else{
                r.back()[1]++;
            }
            x /= lpf[x];
        }while(x > 1);
        return r;
    }
    vector<vector<pii>> p_fact_all(int N){
        vector<vector<pii>> r(N+1);
        r[1].push_back({1,0});
        for (int i = 2; i <= N; i++){
            r[i] = p_fact(i);
        }
        return r;
    }
};

/// @brief 一次不定方程式ax+by=1の解を1つ見つける
/// @param a `a>=0`である必要がある
/// @param b `b>=0`である必要がある
/// @return {x,y,gcd(x,y)}
template<typename T>
array<T,3> axby1(T a, T b){
    T x = 1, y = 0;
    T z = 0, w = 1;
    T tmp;
    while (b){
        T p = a/b, q = a%b;
        tmp = x - y * p; x = y; y = tmp;
        tmp = z - w * p; z = w; w = tmp;
        a = b; b = q;
    }
    return {x, z, a};
}

/// @brief 1/a mod Mを求める
/// @param a 
/// @param M 
/// @return 1/a mod M
template<typename T, typename U>
T inverse_mod(T a, U M){
    auto temp = axby1(a,(T)M);
    assert(temp[2] == 1);
    return (M+temp[0])%M;
}

/// @brief modint
template<const int M>
struct mint{
    int val = 0;
    mint(const ll x){
        val = x%M;
    }
    mint(const mint &m){
        val = m.val;
    }
    mint(){}

    mint inv(){
        val = math::inverse_mod(val, M);
        return *this;
    }
    mint pow(ll N){
        val = math::modpow(val, N, M);
        return *this;
    }
    

    // 入出力用オーバーロード
    friend std::ostream& operator<<(std::ostream& os, const mint m){
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, mint &m){
        ll a;
        is >> a;
        m = a;
        return is;
    }


    void operator=(const ll x){
        val = x%M;
    }
    void operator=(const mint &a){
        val = a.val;
    }
    
    bool operator==(const ll x) const{
        return val == x;
    }
    bool operator==(const mint &a) const{
        return val == a.val;
    }
    bool operator!=(const ll x) const{
        return val != x;
    }
    bool operator!=(const mint &a) const{
        return val != a.val;
    }

    mint operator+(){
        return *this;
    }
    mint operator-(){
        if (val != 0) val = M-val;
        return *this;
    }

    mint operator+(const mint &a) const{
        return mint(val+a.val);
    }
    void operator+=(const mint &a){
        val += a.val;
        if (val >= M) val -= M;
    }
    mint operator+(const ll b) const{
        return mint(val+b%M);
    }
    void operator+=(const ll b){
        val += b%M;
        if (val >= M) val -= M;
    }

    mint operator-(const mint &a) const{
        return mint(M+val-a.val);
    }
    void operator-=(const mint &a){
        val += M-a.val;
        if (val >= M) val -= M;
    }
    mint operator-(const ll b) const{
        return mint(val+M-b%M);
    }
    void operator-=(const ll b){
        val += M-b%M;
        if (val >= M) val -= M;
    }

    mint operator*(const mint &a) const{
        return mint((ll)val*a.val);
    }
    void operator*=(const mint &a){
        val = ((ll)val*a.val)%M;
    }
    mint operator*(const ll b) const{
        return mint(val*(b%M));
    }
    void operator*=(const ll b){
        val = (val*(b%M))%M;
    }

    mint operator/(const mint &a) const{
        return mint((ll)val*inverse_mod(a.val,M));
    }
    void operator/=(const mint &a){
        val = ((ll)val*inverse_mod(a.val,M))%M;
    }
    mint operator/(const ll b) const{
        return mint(val*inverse_mod(b%M,M));
    }
    void operator/=(const ll b){
        val = (val*inverse_mod(b%M,M))%M;
    }
};
struct dynamic_mint{
    int val = 0;
    int M = -1;
    dynamic_mint(const ll x){
        val = x;
    }
    dynamic_mint(const ll x, const ll MOD){
        M = MOD;
        val = x%M;
    }
    dynamic_mint(const dynamic_mint &m){
        update_and_check_mod(m.M);
        val = m.val;
    }
    dynamic_mint(){}

    void update_and_check_mod(const int new_MOD){
        if (M == -1 && new_MOD == -1){
            //cerr << "M is invalid" << endl;
            //assert(false);
        }
        M = max(M, new_MOD);
    }

    dynamic_mint inv(){
        val = math::inverse_mod(val, M);
        return *this;
    }
    dynamic_mint pow(ll N){
        val = math::modpow(val, N, M);
        return *this;
    }
    

    // 入出力用オーバーロード
    friend std::ostream& operator<<(std::ostream& os, const dynamic_mint m){
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, dynamic_mint &m){
        ll a;
        is >> a;
        m = a;
        return is;
    }


    void operator=(const ll x){
        val = x;
    }
    void operator=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = a.val;
    }
    
    bool operator==(const ll x) const{
        return val == x;
    }
    bool operator==(const dynamic_mint &a) const{
        return val == a.val;
    }
    bool operator!=(const ll x) const{
        return val != x;
    }
    bool operator!=(const dynamic_mint &a) const{
        return val != a.val;
    }

    dynamic_mint operator+(){
        return *this;
    }
    dynamic_mint operator-(){
        if (val != 0) val = M-val;
        return *this;
    }

    dynamic_mint operator+(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint(val+a.val, M);
    }
    void operator+=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val += a.val;
        if (val >= M) val -= M;
    }
    dynamic_mint operator+(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val+b%M, M);
    }
    void operator+=(const ll b){
        update_and_check_mod(-1);
        val += b%M;
        if (val >= M) val -= M;
    }

    dynamic_mint operator-(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint(M+val-a.val, M);
    }
    void operator-=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val += M-a.val;
        if (val >= M) val -= M;
    }
    dynamic_mint operator-(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val+M-b%M, M);
    }
    void operator-=(const ll b){
        update_and_check_mod(-1);
        val += M-b%M;
        if (val >= M) val -= M;
    }

    dynamic_mint operator*(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint((ll)val*a.val, M);
    }
    void operator*=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = ((ll)val*a.val)%M;
    }
    dynamic_mint operator*(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val*(b%M), M);
    }
    void operator*=(const ll b){
        update_and_check_mod(-1);
        val = (val*(b%M))%M;
    }

    dynamic_mint operator/(const dynamic_mint &a){
        update_and_check_mod(a.M);
        return dynamic_mint((ll)val*inverse_mod(a.val,M), M);
    }
    void operator/=(const dynamic_mint &a){
        update_and_check_mod(a.M);
        val = ((ll)val*inverse_mod(a.val,M))%M;
    }
    dynamic_mint operator/(const ll b){
        update_and_check_mod(-1);
        return dynamic_mint(val*inverse_mod(b%M,M), M);
    }
    void operator/=(const ll b){
        update_and_check_mod(-1);
        val = (val*inverse_mod(b%M,M))%M;
    }
};

//階乗前計算による二項係数mod998244353
struct factorialncr{
    vector<ll> factorialmod;
    vector<ll> factorialmodinv;
    ll N_MAX_N_MAX;
    ll MOD;
    factorialncr(const ll N_MAX, const ll M){
        N_MAX_N_MAX = max(1ll, N_MAX);
        MOD = M;
        factorialmod = vector<ll>(N_MAX+1);
        factorialmodinv = vector<ll>(N_MAX+1);
        factorialmod[0] = 1;
        factorialmod[1] = 1;
        factorialmodinv[0] = 1;
        factorialmodinv[1] = 1;
        for (int i = 2; i <= N_MAX; i++){
            factorialmod[i] = (i*factorialmod[i-1])%M;
            factorialmodinv[i] = (M-factorialmodinv[M%i]*(M/i)%M)%M;
        }
        for (int i = 1; i <= N_MAX; i++){
            factorialmodinv[i] = (factorialmodinv[i]*factorialmodinv[i-1])%M;
        }
    }

    ll nCr(ll n, ll r){
        if (r < 0 || n < r || n > N_MAX_N_MAX){
            return 0;
        }
        return factorialmod[n]*factorialmodinv[r]%MOD*factorialmodinv[n-r]%MOD;
    }
};

//表の前計算による二項係数modM
struct tablencr{
    vector<vector<ll>> ncrmodlist;
    ll N_MAX_N_MAX;
    public:
    tablencr(const ll N_MAX, const ll M){
        N_MAX_N_MAX = N_MAX;
        ncrmodlist = vector<vector<ll>> (N_MAX+1, vector<ll>(N_MAX+1,0));
        ncrmodlist[0][0] = 1;
        for (int i = 1; i <= N_MAX; i++){
            for (int j = 0; j <= i; j++){
                if (j == 0 || j == i){
                    ncrmodlist[i][j] = 1;
                }
                else{
                    ncrmodlist[i][j] = (ncrmodlist[i-1][j-1] + ncrmodlist[i-1][j])%M;
                }
            }
        }
    }
    ll nCr(ll n, ll r){
        if (r < 0 || n < r || n > N_MAX_N_MAX){
            return 0;
        }
        return ncrmodlist[n][r];
    }
};

//matrix.hpp
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
    size_t H,W;

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
    size_t size(){return H;}
    bool empty(){return H == 0;}
    vector<T>& operator[](const int row){
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
        return M[r2][c2] - M[r2][c1-1] - M[r1-1][c2] + M[r1-1][c1-1];
    }
};

/// @brief A^Nを返す。
/// @param A 
/// @param N 
/// @return A^N
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
/// @tparam T 
/// @param M 
/// @return 
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
/// @param N 
/// @param M 
/// @param A 
/// @param B 
/// @return 
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

ll internal_floor_max(ll A, ll B, ll C, ll D, ll E, ll F){
    if (D < 0){return -1000000000000000000;}
    if (B <= 0){return -1000000000000000000;}
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

//convolution.hpp
vector<ll> powroot998244353{1LL, 998244352LL, 911660635LL, 372528824LL, 69212480LL, 381091786LL, 515872972LL, 273395035LL, 469477650LL, 503794470LL, 464513504LL, 558899782LL, 504969456LL, 840897051LL, 539927980LL, 417009993LL, 725461291LL, 456548895LL, 712494065LL, 542639453LL, 768214923LL, 303507821LL, 438914733LL, 761881641};
vector<ll> powrootinv998244353{1LL, 998244352LL, 86583718LL, 509520358LL, 661054123LL, 863921598LL, 323451984LL, 689146517LL, 379690232LL, 240519260LL, 899368279LL, 920065956LL, 588792270LL, 118574449LL, 847593593LL, 858760106LL, 987418793LL, 177938570LL, 753608159LL, 786906984LL, 331540181LL, 655706071LL, 268754442LL, 191076546};
vector<ll> powroot1224736769{1LL, 1224736768LL, 24506215LL, 992888437LL, 853017291LL, 235319402LL, 269744380LL, 157861287LL, 894223137LL, 600648668LL, 1091208103LL, 382541006LL, 12818149LL, 149218560LL, 746299392LL, 405692663LL, 633223136LL, 672327338LL, 992917013LL, 758198491LL, 1079610480LL, 1056667043LL, 1039331205LL, 1026803890LL, 449603200};
vector<ll> powrootinv1224736769{1LL, 1224736768LL, 1200230554LL, 961581489LL, 796105727LL, 1023008969LL, 406386483LL, 251411652LL, 655108271LL, 1145368249LL, 780593535LL, 38041180LL, 816166160LL, 659160240LL, 1200430513LL, 392462252LL, 15561184LL, 893027826LL, 928760284LL, 497993173LL, 529117122LL, 637457654LL, 931394937LL, 823596420LL, 55047034};

vector<ll> DFT998244353(vector<ll> X, ll K, bool inverse = false){
    if (K == 1){
        return vector<ll>{(X[0]+X[1])%998244353, (998244353+X[0]-X[1])%998244353};
    }

    vector<ll> even(1<<(K-1));
    vector<ll> odd(1<<(K-1));
    for (int i = 0; i < (1<<(K-1)); i++){
        even[i] = (X[i] + X[(1<<(K-1))+i])%998244353;
    }
    ll temp = 1;
    if (inverse){
        for (int i = 0; i < (1<<(K-1)); i++){
            odd[i] = (temp*(998244353 + X[i] - X[(1<<(K-1))+i]))%998244353;
            temp = (temp*powrootinv998244353[K])%998244353;
        }
    }
    else{
        for (int i = 0; i < (1<<(K-1)); i++){
            odd[i] = (temp*(998244353 + X[i] - X[(1<<(K-1))+i]))%998244353;
            temp = (temp*powroot998244353[K])%998244353;
        }
    }

    even = DFT998244353(even,K-1,inverse);
    odd = DFT998244353(odd,K-1,inverse);
    for (int i = 0; i < (1<<K); i++){
        if (i%2){
            X[i] = odd[i/2];
        }
        else{
            X[i] = even[i/2];
        }
    }
    return X;
}
vector<ll> DFT1224736769(vector<ll> X, ll K, bool inverse = false){
    if (K == 1){
        return vector<ll>{(X[0]+X[1])%1224736769, (1224736769+X[0]-X[1])%1224736769};
    }

    vector<ll> even(1<<(K-1));
    vector<ll> odd(1<<(K-1));
    for (int i = 0; i < (1<<(K-1)); i++){
        even[i] = (X[i] + X[(1<<(K-1))+i])%1224736769;
    }
    ll temp = 1;
    if (inverse){
        for (int i = 0; i < (1<<(K-1)); i++){
            odd[i] = (temp*(1224736769 + X[i] - X[(1<<(K-1))+i]))%1224736769;
            temp = (temp*powrootinv1224736769[K])%1224736769;
        }
    }
    else{
        for (int i = 0; i < (1<<(K-1)); i++){
            odd[i] = (temp*(1224736769 + X[i] - X[(1<<(K-1))+i]))%1224736769;
            temp = (temp*powroot1224736769[K])%1224736769;
        }
    }

    even = DFT1224736769(even,K-1,inverse);
    odd = DFT1224736769(odd,K-1,inverse);
    for (int i = 0; i < (1<<K); i++){
        if (i%2){
            X[i] = odd[i/2];
        }
        else{
            X[i] = even[i/2];
        }
    }
    return X;
}

vector<ll> convolution998244353(vector<ll> A, vector<ll> B){

    if (min(A.size(), B.size()) <= 60) {
        if (A.size() < B.size()) {
            swap(A, B);
        }
        std::vector<ll> ans(A.size() + B.size() - 1);
        for (size_t i = 0; i < A.size(); i++) {
            for (size_t j = 0; j < B.size(); j++) {
                ans[i+j] += A[i] * B[j];
                ans[i+j] %= 998244353;
            }
        }
        return ans;
    }

    ll N = A.size()+B.size()-1;
    ll log2N = 0;
    while ((1LL<<log2N) < N){
        log2N++;
    }

    while (A.size() < (1ULL<<log2N)){
        A.push_back(0);
    }
    while (B.size() < (1ULL<<log2N)){
        B.push_back(0);
    }

    A = DFT998244353(A,log2N);
    B = DFT998244353(B,log2N);
    vector<ll> C((1LL<<log2N),0);
    for (int i = 0; i < (1<<log2N); i++){
        C[i] = (A[i]*B[i])%998244353;
    }
    C = DFT998244353(C,log2N,1);
    ll invpow2log2N = inverse_mod((1LL<<log2N),998244353);
    for (int i = 0; i < (1<<log2N); i++){
        C[i] = (C[i]*invpow2log2N)%998244353;
    }
    return C;
}
vector<ll> convolution1224736769(vector<ll> A, vector<ll> B){

    if (min(A.size(), B.size()) <= 60) {
        if (A.size() < B.size()) {
            swap(A, B);
        }
        std::vector<ll> ans(A.size() + B.size() - 1);
        for (size_t i = 0; i < A.size(); i++) {
            for (size_t j = 0; j < B.size(); j++) {
                ans[i+j] += A[i] * B[j];
                ans[i+j] %= 1224736769;
            }
        }
        for (auto &v : ans){
            v %= 1224736769;
        }
        return ans;
    }

    ll N = A.size()+B.size()-1;
    ll log2N = 0;
    while ((1LL<<log2N) < N){
        log2N++;
    }

    while (A.size() < (1ULL<<log2N)){
        A.push_back(0);
    }
    while (B.size() < (1ULL<<log2N)){
        B.push_back(0);
    }

    A = DFT1224736769(A,log2N);
    B = DFT1224736769(B,log2N);
    vector<ll> C((1LL<<log2N),0);
    for (int i = 0; i < (1<<log2N); i++){
        C[i] = (A[i]*B[i])%1224736769;
    }
    C = DFT1224736769(C,(1<<log2N),1);
    ll invpow2log2N = inverse_mod((1LL<<log2N),1224736769);
    for (int i = 0; i < (1<<log2N); i++){
        C[i] = (C[i]*invpow2log2N)%1224736769;
    }
    return C;
}

/// @brief [x^N](P(x)/Q(x))をmod998244353で求める。
/// @param N 
/// @param P 
/// @param Q 
/// @return 
ll Bostan_Mori998244353(const ll N, vector<ll> P, vector<ll> Q){
    assert(N >= 0);
    if (N == 0){
        return P[0]*math::inverse_mod(Q[0], 998244353);
    }
    vector<ll> Q_minus;
    const int maxloop = (N == 1 ? 1 : 65-__builtin_clzll(N-1));
    for (int _i_ = 0; _i_ < maxloop; _i_++){
        Q_minus.resize(Q.size());
        for (size_t i = 0; i < Q_minus.size(); i += 2){
            Q_minus[i] = Q[i];
        }
        for (size_t i = 1; i < Q_minus.size(); i += 2){
            Q_minus[i] = 998244352*Q[i]%998244353;
        }
        auto A = math::convolution998244353(P,Q_minus);
        auto B = math::convolution998244353(Q,Q_minus);
        Q.resize((B.size()+1)/2);
        for (size_t i = 0; i < B.size(); i += 2){
            Q[i/2] = B[i];
        }
        P.resize((A.size()+!btest(N,_i_))/2);
        for (size_t i = btest(N,_i_); i < A.size(); i += 2){
            P[i/2] = A[i];
        }
    }
    return P[0]*math::inverse_mod(Q[0], 998244353);
}


}
namespace array_datastructure{

//SegTree&LazySegTree.hpp
/// @brief 抽象化セグメントツリー
/// @attention コンストラクタ1 SegTree(A, e, op, mapping)
/// @attention コンストラクタ2 SegTree(N, I, e, op, mapping)
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
template<typename info>
struct SegTree{
    int log2N;//セグ木の高さ-1
    info e;///単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)

    vector<info> tree;//セグ木本体
    int max_capacity;

    /// @brief N個のIで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    SegTree(int N, info I, info eee, function<info(info,info)> op){
        //基本情報を登録
        e = eee;
        operation = op;
        max_capacity = N;

        //セグ木のサイズを決定
        if (N == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<info>(1<<(log2N+1));
        tree[0] = e;
        for (int i = 0; i < N; i++){
            tree[i+(1<<log2N)] = I;
        }
        for (int i = N; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = e;
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = operation(tree[2*i], tree[2*i+1]);
        }
    }
    /// @brief vector Aで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    SegTree(const vector<info> &A, info eee, function<info(info,info)> op){
        //基本情報を登録
        e = eee;
        operation = op;
        max_capacity = A.size();

        //セグ木のサイズを決定
        if (A.size() == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<info>(1<<(log2N+1));
        tree[0] = e;
        for (int i = 0; i < max_capacity; i++){
            tree[i+(1<<log2N)] = A[i];
        }
        for (int i = max_capacity; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = e;
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = operation(tree[2*i], tree[2*i+1]);
        }
    }

    int get_left(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(index_on_tree-(1<<(31-__builtin_clz(index_on_tree))));
    }
    int get_right(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(1+index_on_tree-(1<<(31-__builtin_clz(index_on_tree))))-1;
    }    


    /// @brief index閉区間[L,R]において、集計を行う。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @return [L,R]での集計結果
    info range_get(const int L, const int R){
        if (L > R || L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            return e;
        }
        info ret = e;
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            ret = operation(ret, tree[(left+(1<<log2N))>>log2interval]);
            left += 1<<log2interval;
        }
        return ret;
    }

    /// @brief index tが指す要素を取得する。変更はできない
    /// @param t 
    /// @return 要素
    info get(const int t) const{
        if (t >= max_capacity || t < 0){
            return e;
        }
        return tree[t+(1<<log2N)];
    }

    /// @brief index tが指す要素をvalに置換する。
    /// @param L 置換対象のindex
    /// @param val 置換後の値
    void pointwise_update(const int t, const info &val){
        if (t >= max_capacity || t < 0){
            return;
        }
        int start_index = t + (1<<log2N);
        tree[start_index] = val;
        start_index >>= 1;
        while (start_index){
            tree[start_index] = operation(tree[2*start_index],tree[2*start_index+1]);
            start_index >>= 1;
        }
    }

    /// @brief 左端をLに固定したとき、Gがtrueになる最小の右端indexを返す。もしなければINF(=2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(L, t)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最小が求まる。
    /// @return Gがtrueになる最小右端indexまたは2147483647
    int min_right(int L, const function<bool(info)> &G){

        info current_result = e;

        checkpoint:

        int ctz = L == 0 ? log2N : __builtin_ctz(L);
        if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz]))){
            if (get_right(((1<<log2N)+L)>>ctz)+1 == 1<<log2N){
                return 2147483647;
            }
            current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz]);
            L = get_right(((1<<log2N)+L)>>ctz)+1;
            goto checkpoint;
        }

        for (int i = ctz-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+L)>>i]))){
                current_result = operation(current_result, tree[((1<<log2N)+L)>>i]);
                L = get_right(((1<<log2N)+L)>>i)+1;
                goto checkpoint;
            }
        }
        return L;
    }
    /// @brief 右端をRに固定したとき、Gがtrueになる最大の左端indexを返す。もしなければ-INF-1(=-2147483648)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。
    /// @param R 右端 
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(t, R)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最大が求まる。
    /// @return Gがtrueになる最大左端index
    int max_left(int R, const function<bool(info)> &G){
        info current_result = e;

        checkpoint:

        int cto = __builtin_ctz(~R);//cto...count trailing one
        if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto]))){
            if (get_left(((1<<log2N)+R)>>cto) == 0){
                return -2147483648;
            }
            current_result = operation(current_result, tree[((1<<log2N)+R)>>cto]);
            R = get_left(((1<<log2N)+R)>>cto)-1;
            goto checkpoint;
        }

        for (int i = cto-1; i >= 0; i--){
            if (!G(operation(current_result, tree[((1<<log2N)+R)>>i]))){
                current_result = operation(current_result, tree[((1<<log2N)+R)>>i]);
                R = get_left(((1<<log2N)+R)>>i)-1;
                goto checkpoint;
            }
        }
        return R;
    }

    info operator[](const int t){
        return get(t);
    }
    int size(){
        return max_capacity;
    }
};
/// @brief 抽象化遅延セグメントツリー
/// @attention コンストラクタ1 LazySegTree(A, e, op, mapping, composition, id)
/// @attention コンストラクタ2 LazySegTree(N, I, e, op, mapping, composition , id)
/// @tparam info セグ木の各ノードに載せる情報をまとめた構造体の型
/// @tparam func 更新に使う変数をまとめた構造体の型(アフィン変換なら、aとbを持つ構造体など)
/// @param e 載せたものの単位元(sumなら0, maxなら-infなど)
/// @param operation 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
/// @param mapping infoに対してfuncを作用させた結果を返す関数(アフィン変換ならx -> ax+b)
/// @param composition func同士の合成結果を1つのfuncにする関数((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
/// @param id funcの恒等写像(アフィン変換ならx -> 1x+0)
template<typename info, typename func>
struct LazySegTree{
    int log2N;//セグ木の高さ-1
    info e;///セグ木に載っているものの単位元
    function<info(info,info)> operation;//各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    function<info(func,info)> mapping;//更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    function<func(func,func)> composition;//mを合成した結果(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    func id;//mappingの恒等写像版(アフィン変換ならx -> 1x+0など)

    vector<pair<info,func>> tree;//セグ木本体(info, delay)
    int max_capacity;

    /// @brief N個のIで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    /// @param c mを合成した結果((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    /// @param ididid mの恒等写像版(アフィン変換ならx -> 1x+0)
    LazySegTree(int N, info I, info eee, function<info(info,info)> op, function<info(func,info)> m, function<func(func,func)> c, func ididid){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;
        composition = c;
        id = ididid;
        max_capacity = N;

        //セグ木のサイズを決定
        if (N == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));

        //セグ木に実際に乗せるvectorを構築
        tree = vector<pair<info,func>>(1<<(log2N+1));
        tree[0] = make_pair(e,id);
        for (int i = 0; i < N; i++){
            tree[i+(1<<log2N)] = make_pair(I,id);
        }
        for (int i = N; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = make_pair(e,id);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = make_pair(operation(tree[2*i].first, tree[2*i+1].first),id);
        }
    }
    /// @brief vector Aで初期化
    /// @param I 載せたい構造体
    /// @param N 載せた個数
    /// @param eee 載せたものの単位元(sumなら0, maxなら-infなど)
    /// @param op 各ノードに載ってる構造体に対する二項演算をする関数(min,max,sumなど)
    /// @param m 更新を行うとどうなるか？(アフィン変換ならx -> ax+b)
    /// @param c mを合成した結果((u,v)でu(v())の結果を返すものとする)(ax+bのあとにcx+dを作用させると実質acx+bc+dになるなど)
    /// @param ididid mの恒等写像版(アフィン変換ならx -> 1x+0)
    LazySegTree(const vector<info> &A, info eee, function<info(info,info)> op, function<info(func,info)> m, function<func(func,func)> c, func ididid){
        //基本情報を登録
        e = eee;
        operation = op;
        mapping = m;
        composition = c;
        id = ididid;
        max_capacity = A.size();

        //セグ木のサイズを決定
        if (A.size() == 0){cerr << "N=0 is invalid" << endl; assert(false);}
        log2N = 64-(max_capacity == 1 ? 64 : __builtin_clzll(max_capacity-1));


        //セグ木に実際に乗せるvectorを構築
        tree = vector<pair<info,func>>(1<<(log2N+1));
        tree[0] = make_pair(e,id);
        for (int i = 0; i < max_capacity; i++){
            tree[i+(1<<log2N)] = make_pair(A[i],id);
        }
        for (int i = max_capacity; i < (1<<log2N); i++){
            tree[i+(1<<log2N)] = make_pair(e,id);
        }
        for (int i = (1<<log2N)-1; i >= 1; i--){
            tree[i] = make_pair(operation(tree[2*i].first, tree[2*i+1].first),id);
        }
    }

    int get_left(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(index_on_tree-(1<<(31-__builtin_clz(index_on_tree))));
    }
    int get_right(int index_on_tree){
        return (1<<(log2N-31+__builtin_clz(index_on_tree)))*(1+index_on_tree-(1<<(31-__builtin_clz(index_on_tree))))-1;
    }

    /// @brief 遅延情報の伝播を行う
    void tell_info(const int index_on_tree){
        if (index_on_tree >= (1<<log2N)){
            tree[index_on_tree].second = id;
            return;
        }
        //左の子に伝播
        tree[2*index_on_tree].first = mapping(tree[index_on_tree].second, tree[2*index_on_tree].first);
        tree[2*index_on_tree].second = composition(tree[index_on_tree].second, tree[2*index_on_tree].second);
        //右の子に伝播
        tree[2*index_on_tree+1].first = mapping(tree[index_on_tree].second,tree[2*index_on_tree+1].first);
        tree[2*index_on_tree+1].second = composition(tree[index_on_tree].second, tree[2*index_on_tree+1].second);

        tree[index_on_tree].second = id;
    }


    deque<int> lazy_node;//区間集計、区間更新に使う。
    deque<int> lazy_node_flipped;//区間更新で使う。


    /// @brief index閉区間[L,R]において、集計を行う。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @return [L,R]での集計結果
    info range_get(const int L, const int R){
        if (L > R || L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            return e;
        }
        int Lstart = L + (1<<log2N);
        int Rstart = R+1 + (1<<log2N);
        int lm = (Lstart / (Lstart & -Lstart)) >> 1;
        int rm = (Rstart / (Rstart & -Rstart)) >> 1;
        while (Lstart < Rstart){
            if (Rstart <= rm){
                lazy_node.push_back(Rstart);
            }
            if (Lstart <= lm){
                lazy_node.push_back(Lstart);
            }
            Lstart >>= 1;
            Rstart >>= 1;
        }
        while (Lstart){
            lazy_node.push_back(Lstart);
            Lstart >>=1;
        }


        while (!lazy_node.empty()){
            tell_info(lazy_node.back());
            lazy_node.pop_back();
        }
        info ret = e;
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            ret = operation(ret, tree[(left+(1<<log2N))>>log2interval].first);
            left += 1<<log2interval;
        }
        return ret;
    }

    /// @brief index閉区間[L,R]に対してFをmappingする。
    /// @param L 左端(左端を含む)
    /// @param R 右端(右端を含む)
    /// @param F 適用する写像(アフィン変換ならaとbを持った構造体など)
    void range_update(const int L, const int R, const func &F){
        if (L > R || L >= max_capacity || L < 0 || R >= max_capacity || R < 0){
            return;
        }
        int Lstart = L + (1<<log2N);
        int Rstart = R+1 + (1<<log2N);
        int lm = (Lstart / (Lstart & -Lstart)) >> 1;
        int rm = (Rstart / (Rstart & -Rstart)) >> 1;
        while (Lstart && Lstart < Rstart){
            if (Rstart <= rm){
                lazy_node.push_back(Rstart);
            }
            if (Lstart <= lm){
                lazy_node.push_back(Lstart);
            }
            Lstart >>= 1;
            Rstart >>= 1;
        }
        while (Lstart){
            lazy_node.push_back(Lstart);
            Lstart >>=1;
        }


        while (!lazy_node.empty()){
            tell_info(lazy_node.back());
            lazy_node_flipped.push_back(lazy_node.back());
            lazy_node.pop_back();
        }
        int left = L;
        while (left < R+1){
            int log2interval = min(left ? __builtin_ctz(left) : log2N, 31-__builtin_clz(R+1-left));
            tree[(left+(1<<log2N))>>log2interval].first = mapping(F,tree[(left+(1<<log2N))>>log2interval].first);
            tree[(left+(1<<log2N))>>log2interval].second = composition(F,tree[(left+(1<<log2N))>>log2interval].second);
            left += 1<<log2interval;
        }
        while (!lazy_node_flipped.empty()){
            if (lazy_node_flipped.back()>>log2N){lazy_node_flipped.pop_back();continue;}
            tree[lazy_node_flipped.back()].first = operation(tree[2*lazy_node_flipped.back()].first, tree[2*lazy_node_flipped.back()+1].first);
            lazy_node_flipped.pop_back();
        }
    }

    /// @brief 左端をLに固定したとき、Gがtrueになる最小の右端indexを返す。もしなければINF(=2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(L, t)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最小が求まる。
    /// @return Gがtrueになる最小右端indexまたは2147483647
    int min_right(int L, const function<bool(info)> &G){
        info current_result = e;

        int ctz_init = L == 0 ? log2N : __builtin_ctz(L);
        for (int i = log2N; i > ctz_init; i--){
            tell_info(((1<<log2N)+L)>>i);
        }

        checkpoint:

        int ctz = L == 0 ? log2N : __builtin_ctz(L);
        tell_info(((1<<log2N)+L)>>ctz);
        if (!G(operation(current_result, tree[((1<<log2N)+L)>>ctz].first))){
            if (get_right(((1<<log2N)+L)>>ctz)+1 == 1<<log2N){
                return 2147483647;
            }
            current_result = operation(current_result, tree[((1<<log2N)+L)>>ctz].first);
            L = get_right(((1<<log2N)+L)>>ctz)+1;
            goto checkpoint;
        }

        for (int i = ctz-1; i >= 0; i--){
            tell_info(((1<<log2N)+L)>>i);
            if (!G(operation(current_result, tree[((1<<log2N)+L)>>i].first))){
                current_result = operation(current_result, tree[((1<<log2N)+L)>>i].first);
                L = get_right(((1<<log2N)+L)>>i)+1;
                goto checkpoint;
            }
        }
        return L;
    }
    /// @brief 右端をRに固定したとき、Gがtrueになる最大の左端indexを返す。もしなければ-INF-1(=-2147483647)が返ってくる。
    /// @attention 判定関数Gは、区間を広げていったときにfalse,false,false,...false,true,true,true...のように、falseが続いた後にtrueが続くものでなければならない。 
    /// @param L 左端
    /// @param G 判定関数...boolを返す。引数としてinfoを受け取るが、これはT.range_get(t, R)が入り、これに関する条件式を自分で関数内に記述することで、このようなtの最小が求まる。
    /// @return Gがtrueになる最大左端indexまたは-2147483648
    int max_left(int R, const function<bool(info)> &G){
        info current_result = e;

        int cto_init = __builtin_ctz(~R);
        for (int i = log2N; i > cto_init; i--){
            tell_info(((1<<log2N)+R)>>i);
        }

        checkpoint:

        int cto = __builtin_ctz(~R);//cto...count trailing one
        tell_info(((1<<log2N)+R)>>cto);
        if (!G(operation(current_result, tree[((1<<log2N)+R)>>cto].first))){
            if (get_left(((1<<log2N)+R)>>cto) == 0){
                return -2147483648;
            }
            current_result = operation(current_result, tree[((1<<log2N)+R)>>cto].first);
            R = get_left(((1<<log2N)+R)>>cto)-1;
            goto checkpoint;
        }

        for (int i = cto-1; i >= 0; i--){
            tell_info(((1<<log2N)+R)>>i);
            if (!G(operation(current_result, tree[((1<<log2N)+R)>>i].first))){
                current_result = operation(current_result, tree[((1<<log2N)+R)>>i].first);
                R = get_left(((1<<log2N)+R)>>i)-1;
                goto checkpoint;
            }
        }
        return R;
    }

    info operator[](const int t){
        return range_get(t,t);
    }
    int size(){
        return max_capacity;
    }
};

/// @brief 両端優先度付きキュー
/// @tparam T 
template<typename T>
struct priority_deque{
    private:
    vector<T> A;
    void adjust_max_heap_from_bottom(int idx){
        while (idx > 0){
            if (A[((idx-1)>>1)&-2] < A[idx]){
                swap(A[((idx-1)>>1)&-2], A[idx]);
            }
            else{return;}
            idx--;
            idx>>=1;
            idx&=-2;
        }
    }
    void adjust_min_heap_from_bottom(int idx){
        while (idx > 1){
            if (A[idx] < A[((idx-2)>>1)|1]){
                swap(A[idx], A[((idx-2)>>1)|1]);
            }
            else{return;}
            idx-=2;
            idx>>=1;
            idx|=1;
        }
    }
    void build_heap(){
        for (int i = A.size()-1; i >= 0; i--){
            if ((i&1) && A[i-1] < A[i]){swap(A[i-1], A[i]);}
            
            int n = A.size();
            int t = i;
	        if (t&1){
	            while (2*t+1 < n){
	        	int c = 2*t+3;
	        	if (n <= c || A[c-2] < A[c]) c -= 2;
	        	if (c < n && A[c] < A[t]){swap(A[t], A[c]); t = c;}
	        	else break;
	            }
	        } else {
	            while (2*t+2 < n){
	        	int c = 2*t+4;
	        	if (n <= c || A[c] < A[c-2]) c -= 2;
	        	if (c < n && A[t] < A[c]){swap(A[t], A[c]); t = c;}
	        	else break;
	            }
	        }
            if ((t|1) < (int)A.size() && A[t&-2] < A[t|1]){
	            swap(A[t&-2], A[t|1]);
	            t ^= 1;
	        }
        
	        int p;
	        while (i < t && A[p=(((t>>1)-1)&-2)] < A[t]){
	            swap(A[p], A[t]);
	            t = p;
	        }
	        while (i < t && A[t] < A[p=(((t>>1)-1)|1)]){
	            swap(A[p], A[t]);
	            t = p;
	        }
        }
    }
    public:
    void make_heap(const vector<T>& B){
        A = B;
        build_heap();
    }
    void push(const T& value){
        if (A.size()&1){
            if (A.back() < value){
                A.insert(prev(A.end()), value);
                adjust_max_heap_from_bottom(A.size()-2);
            }
            else{
                A.push_back(value);
                adjust_min_heap_from_bottom(A.size()-1);
            }
        }
        else{
            A.push_back(value);
            if (A.size() == 1){return;}
            if (A[((A.size()-2)>>1)&-2] < A.back()){
                adjust_max_heap_from_bottom(A.size()-1);
            }
            else if (A.back() < A[((A.size()-2)>>1)|1]){
                swap(A.back(), A[((A.size()-2)>>1)|1]);
                adjust_min_heap_from_bottom(((A.size()-2)>>1)|1);
            }
        }
    }
    T pop_min(){
        if (A.size() == 1){T temp = move(A[0]); A.pop_back(); return temp;}
        T ret = move(A[1]);
        A[1] = A.back();
        A.pop_back();
        int idx = 1;
        while ((idx<<1)+1 < (int)A.size()){
            if ((idx<<1)+3 < (int)A.size()){
                if (A[idx] < A[(idx<<1)+1] && A[idx] < A[(idx<<1)+3]){
                    return ret;
                }
                if (A[(idx<<1)+1] < A[(idx<<1)+3]){
                    swap(A[idx], A[(idx<<1)+1]);
                    idx<<=1;
                    idx+=1;
                }
                else{
                    swap(A[idx], A[(idx<<1)+3]);
                    idx<<=1;
                    idx+=3;
                }
            }
            else{
                if (A[(idx<<1)+1] < A[idx]){
                    swap(A[idx], A[(idx<<1)+1]);
                }
                idx<<=1;
                idx+=1;
                break;
            }
        }
        if (A.size() < 2){return ret;}
        if (A[idx-1] < A[idx]){
            swap(A[idx], A[idx-1]);
            adjust_max_heap_from_bottom(idx-1);
        }
        return ret;
    }
    T pop_max(){
        T ret = move(A[0]);
        A[0] = A.back();
        A.pop_back();
        int idx = 0;
        while ((idx<<1)+2 < (int)A.size()){
            if ((idx<<1)+4 < (int)A.size()){
                if (A[(idx<<1)+2] < A[idx] && A[(idx<<1)+4] < A[idx]){
                    return ret;
                }
                if (A[(idx<<1)+2] < A[(idx<<1)+4]){
                    swap(A[idx], A[(idx<<1)+4]);
                    idx<<=1;
                    idx+=4;
                }
                else{
                    swap(A[idx], A[(idx<<1)+2]);
                    idx<<=1;
                    idx+=2;
                }
            }
            else{
                if (A[idx] < A[(idx<<1)+2]){
                    swap(A[idx], A[(idx<<1)+2]);
                }
                idx<<=1;
                idx+=2;
                break;
            }
        }
        if (A.size() < 2){return ret;}
        if (idx+1 == (int)A.size()){
            if (A.back() < A[((idx-1)>>1)|1]){
                swap(A.back(), A[((idx-1)>>1)|1]);
                adjust_min_heap_from_bottom(((idx-1)>>1)|1);
            }
        }
        else{
            if (A[idx] < A[idx+1]){
                swap(A[idx], A[idx+1]);
                adjust_min_heap_from_bottom(idx+1);
            }
        }
        return ret;
    }
    T get_min(){return A.size() > 1 ? A[1] : A[0];}
    T get_max(){return A[0];}
    bool empty(){return A.empty();}
    size_t size(){return A.size();}
};

}
namespace graph{

//Graph.hpp
struct NoWeightGraph{
    ll N;//頂点数
    vector<vector<ll>> Edge;//隣接頂点リスト(重みなし)
    vector<vector<ll>> D;//直近のBFSで求めた距離リスト。距離のほかに、これを踏む前にどこにいたかも記録してある。{距離, 一個前}


    /// @brief 頂点,辺数を指定して自動で入力を受け取ってグラフを構築する
    /// @param vertex_num 
    /// @param edge_num 
    /// @param directional 
    NoWeightGraph(ll vertex_num, ll edge_num, bool directional = false){
        N = vertex_num;
        Edge = vector<vector<ll>>(N+1);
        for (int i = 0; i < edge_num; i++){
            ll u,v;
            cin >> u >> v;
            Edge[u].push_back(v);
            if (!directional){
                Edge[v].push_back(u);
            }
        }
    }
    /// @brief 頂点数を指定して辺のないグラフを構築
    /// @param vertex_num 
    NoWeightGraph(ll vertex_num){
        N = vertex_num;
        Edge = vector<vector<ll>>(N+1);
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築
    /// @attention 隣接頂点リストは1-indexedで作る必要がある。
    /// @param E 
    NoWeightGraph(const vector<vector<ll>> &E){
        N = E.size()-1;
        Edge = E;
    }

    void add_edge(ll u, ll v, bool directional = false){
        Edge[u].push_back(v);
        if (!directional){
            Edge[v].push_back(u);
        }
    }

    /// @brief 始点を指定し、そこからの距離と、それを実現するためのパス復元用の情報を持ったリストを作成する。
    /// @param startpoints 
    /// @return 
    vector<vector<ll>> BFS(const vector<ll> &startpoints){
        queue<ll> Q;
        D = vector<vector<ll>>(N+1,{9223372036854775807, -2});//-2...まだ到達してない,  -1...始点,  それ以外...一個前の頂点

        for (auto v : startpoints){
            D[v] = {0, -1};
            Q.push(v);
        }

        while (!Q.empty()){
            ll now = Q.front();
            Q.pop();
            for (auto v : Edge[now]){
                if (D[v][1] == -2){
                    D[v][0] = D[now][0] + 1;
                    D[v][1] = now;
                    Q.push(v);
                }
            }
        }
        return D;
    }

    /// @brief 頂点gにたどり着くためのパスを求める。ないなら空のリストが返ってくる
    /// @param g 
    /// @return 
    vector<ll> path(ll g){
        vector<ll> R;
        if (D[g][1] == -2){return R;}
        while (D[g][1] != -1){
            R.push_back(g);
            g = D[g][1];
        }
        R.push_back(g);
        reverse(vall(R));
        return R;
    }

    void scc_dfs(vector<vector<ll>> &E, ll now, vector<bool> &used, ll &temp, vector<pair<ll,ll>> &order){
        for (auto v : E[now]){
            if (used[v]){continue;}
            used[v] = true;
            scc_dfs(E,v,used,temp,order);
        }
        order[now].second = temp;
        temp++;
    }

    vector<vector<ll>> SCC(){
        vector<vector<ll>> Edge_inverse(N+1);//辺が逆のグラフ
        for (int i = 1; i <= N; i++){
            for (auto v : Edge[i]){
                Edge_inverse[v].push_back(i);
            }
        }


        ll temp = 1;
        vector<bool> used(N+1,0);
        vector<pair<ll,ll>> order(N+1);

        for (int i = 1; i <= N; i++){
            order[i].first = i;
        }
        for (int i = 1; i <= N; i++){
            if (!used[i]){
                used[i] = true;
                scc_dfs(Edge,i,used,temp,order);//一回目のdfsで、帰った順番を記録
            }
        }

        sort(vall(order), [](const pair<ll,ll> &a, const pair<ll,ll> &b){return a.second > b.second;});
        order.pop_back();

        temp = 1;
        vector<ll> groups(N+1,-1);
        for (int i = 0; i < N; i++){
            if (groups[order[i].first] == -1){//2回目のBFSで連結成分ごとに分解して、番号をつける
                queue<ll> Q;
                Q.push(order[i].first);
                groups[order[i].first] = temp;
                while (!Q.empty()){
                    ll now = Q.front();
                    Q.pop();
                    for (auto v : Edge_inverse[now]){
                        if (groups[v] == -1){
                            groups[v] = temp;
                            Q.push(v);
                        }
                    }
                }
                temp++;
            }
        }
        
        vector<vector<ll>> small_graph(temp);//縮約されたグラフを構築
        for (int i = 1; i <= N; i++){
            for (auto v : Edge[i]){
                if (groups[i] != groups[v]){
                    small_graph[groups[i]].push_back(groups[v]);
                }
            }
        }

        vector<ll> into_count(temp,0);//入ってくる辺の数を管理
        priority_queue<ll,vector<ll>,greater<ll>> pQ;//入ってくる辺がないような頂点を管理
        vector<ll> topological_sort;

        for (auto &vec : small_graph){
            sort(vall(vec));
            vec.erase(unique(vall(vec)),vec.end());
            for (auto v : vec){
                into_count[v]++;
            }
        }

        for (int i = 1; i < temp; i++){
            if (into_count[i] == 0){
                pQ.push(i);
            }
        }

        while (!pQ.empty()){
            ll now = pQ.top();
            pQ.pop();
            topological_sort.push_back(now);
            for (auto v : small_graph[now]){
                into_count[v]--;
                if (into_count[v] == 0){
                    pQ.push(v);
                }
            }
        }

        vector<ll> inv_topological_sort(temp);
        for (int i = 1; i < temp; i++){
            inv_topological_sort[topological_sort[i-1]] = i;
        }
        vector<vector<ll>> ans(temp-1);
        for (int i = 1; i <= N; i++){
            ans[inv_topological_sort[groups[i]]-1].push_back(i);
        }

        return ans;
    }

};

struct WeightedGraph{
    ll N;
    vector<vector<vector<ll>>> Edge;
    vector<ll> D;//直近のDijkstraで求めた距離リスト

    /// @brief 頂点,辺数を指定して自動で入力を受け取ってグラフを構築する
    /// @param vertex_num 
    /// @param edge_num 
    /// @param directional 
    WeightedGraph(ll vertex_num, ll edge_num, bool directional = false){
        N = vertex_num;
        Edge = vector<vector<vector<ll>>>(N+1);
        for (int i = 0; i < edge_num; i++){
            ll u,v,w;
            cin >> u >> v >> w;
            Edge[u].push_back({v,w});
            if (!directional){
                Edge[v].push_back({u,w});
            }
        }
    }
    /// @brief 頂点数を指定して辺のないグラフを構築
    /// @param vertex_num 
    WeightedGraph(ll vertex_num){
        N = vertex_num;
        Edge = vector<vector<vector<ll>>>(N+1);
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築
    /// @param E 
    WeightedGraph(const vector<vector<vector<ll>>> &E){
        N = E.size()-1;
        Edge = E;
    }

    void add_edge(ll u, ll v, ll w, bool directional = false){
        Edge[u].push_back({v,w});
        if (!directional){
            Edge[v].push_back({v,w});
        }
    }

    vector<ll> Djikstra(const vector<ll> &startpoints){
        vector<bool> visited(N+1,0);
        D = vector<ll>(N+1,9223372036854775807);

        priority_queue<vector<ll>,vector<vector<ll>>,greater<vector<ll>>> pQ;
        for (auto v : startpoints){
            pQ.push({0,v});
        }

        while (!pQ.empty()){
            ll dist = pQ.top()[0];
            ll now = pQ.top()[1];
            pQ.pop();
            if (visited[now]){continue;}
            visited[now] = 1;
            D[now] = dist;
            for (auto v : Edge[now]){
                if (visited[v[0]]){continue;}
                pQ.push({dist+v[1], v[0]});
            }
        }
        return D;
    }
};

struct Tree{
    ll N;//頂点数
    vector<vector<pll>> Edge;//隣接頂点リスト
    vector<pll> D;//直近のBFSで求めた距離リスト。距離のほかに、これを踏む前にどこにいたかも記録してある。{距離, 一個前}

    vector<vector<pll>> lca_doubling_table;//lcaを求めるためのダブリング表。頂点1を根とする。{どこに行ったか, そこに行くのにかかったコスト}
    vector<ll> lca_depth;//lcaを求めるための各頂点の深さ表。



    /// @brief 頂点数,重みありかなしかを指定して自動で入力を受け取ってグラフを構築する。
    /// @attention 頂点数をNにすると、ちょてん番号は1,2,3...Nになる。
    /// @param vertex_num 
    /// @param weighted
    Tree(ll vertex_num, bool weighted = false){
        N = vertex_num;
        Edge = vector<vector<pll>>(N+1);
        for (int i = 0; i < N-1; i++){
            ll u,v,w;
            if (weighted){
                cin >> u >> v >> w;
            }
            else{
                cin >> u >> v;
                w = 1;
            }
            Edge[u].push_back({v,w});
            Edge[v].push_back({u,w});
        }
    }

    /// @brief 事前に作った隣接頂点リストでグラフを構築。重みも入力する必要がある。{行先, 重み}
    /// @attention 1-indexedであり、辺はちょうど(頂点数-1)でなければならない
    /// @param E 
    Tree(const vector<vector<pll>> &E){
        N = E.size()-1;
        Edge = E;
    }

    /// @brief 始点を指定し、そこからの距離と、それを実現するためのパス復元用の情報を持ったリストを作成する
    /// @param startpoints 
    /// @return 
    vector<pll> BFS(const vector<ll> &startpoints){
        queue<ll> Q;
        D = vector<pll>(N+1,{9223372036854775807, -2});//-2...まだ到達してない,  -1...始点,  それ以外...一個前の頂点
        for (auto v : startpoints){
            D[v] = {0, -1};
            Q.push(v);
        }

        while (!Q.empty()){
            ll now = Q.front();
            Q.pop();
            for (auto v : Edge[now]){
                if (D[v[0]][1] == -2){
                    D[v[0]][0] = D[now][0] + v[1];
                    D[v[0]][1] = now;
                    Q.push(v[0]);
                }
            }
        }
        return D;
    }

    /// @brief 頂点gにたどり着くためのパスを求める。ないなら空のリストが返ってくる
    /// @param g 
    /// @return 
    vector<int> path(ll g){
        vector<int> R;
        if (D[g][1] == -2){return R;}
        while (D[g][1] != -1){
            R.emplace_back(g);
            g = D[g][1];
        }
        R.push_back(g);
        reverse(vall(R));
        return R;
    }

    /// @brief 木の直径とそのパスを求める
    /// @return 木の直径の両端を結ぶパスを求める
    pair<ll,vector<int>> diameter(){
        BFS({1});
        ll farthest1 = -1;
        ll dist1 = -1;
        for (int i = 1; i <= N; i++){
            if (dist1 <= D[i][0]){
                farthest1 = i;
                dist1 = D[i][0];
            }
        }
        BFS({farthest1});
        ll farthest2 = -1;
        ll dist2 = -1;
        for (int i = 1; i <= N; i++){
            if (dist2 <= D[i][0]){
                farthest2 = i;
                dist2 = D[i][0];
            }
        }
        return make_pair(dist2, path(farthest2));
    }

    /// @brief ダブリングテーブルを初期化する
    void doubling_init(){
        ll M = 1+log2l(N);
        lca_doubling_table = vector<vector<pll>> (N+1, vector<pll>(M+1,pll{-1,-1}));
        lca_depth = vector<ll>(N+1,0);

        queue<ll> Q;
        vector<bool> used(N+1,0);
        Q.push(1);
        used[1] = 1;
        lca_depth[1] = 0;
        while (!Q.empty()){
            ll now = Q.front();
            Q.pop();
            for (auto &v : Edge[now]){
                if (used[v[0]]){continue;}
                used[v[0]] = 1;
                lca_depth[v[0]] = lca_depth[now]+1;
                lca_doubling_table[v[0]][0] = {now, v[1]};
                Q.push(v[0]);
            }
        }

        for (int b = 1; b <= M; b++){
            for (int i = 1; i <= N; i++){
                if (lca_doubling_table[i][b-1][0] == -1){//2^(b-1)個上がない
                    lca_doubling_table[i][b] = {-1,-1};
                }
                else if (lca_doubling_table[lca_doubling_table[i][b-1][0]][b-1][0] == -1){//2^(b-1)個上はあるが、そのさらに2^(b-1)個上がない
                    lca_doubling_table[i][b] = {-1,-1};
                }
                else{
                    lca_doubling_table[i][b][0] = lca_doubling_table[lca_doubling_table[i][b-1][0]][b-1][0];
                    lca_doubling_table[i][b][1] = lca_doubling_table[i][b-1][1] + lca_doubling_table[lca_doubling_table[i][b-1][0]][b-1][1];
                }
            }
        }
    }


    ll path_length(ll u, ll v){
        ll length = 0;
        if (lca_depth[u] < lca_depth[v]) swap(u,v);
        ll s = lca_depth[u] - lca_depth[v];
        ll t = 0;
        while (s){
            if (s%2){
                length += lca_doubling_table[u][t][1];
                u = lca_doubling_table[u][t][0];
            }
            t++;
            s >>= 1;
        }

        ll r = lca_doubling_table[0].size()-1;
        while (u != v){
            if (r > 0 && lca_doubling_table[u][r][0] == lca_doubling_table[v][r][0]){
                r--;
            }
            else{
                length += lca_doubling_table[u][r][1] + lca_doubling_table[v][r][1];
                u = lca_doubling_table[u][r][0];
                v = lca_doubling_table[v][r][0];
            }
        }
        return length;
    }

    ll lca(ll u, ll v){
        if (lca_depth[u] < lca_depth[v]) swap(u,v);
        ll s = lca_depth[u] - lca_depth[v];
        ll t = 0;
        while (s){
            if (s%2){
                u = lca_doubling_table[u][t][0];
            }
            t++;
            s >>= 1;
        }

        ll r = lca_doubling_table[0].size()-1;
        while (u != v){
            if (r > 0 && lca_doubling_table[u][r][0] == lca_doubling_table[v][r][0]){
                r--;
            }
            else{
                u = lca_doubling_table[u][r][0];
                v = lca_doubling_table[v][r][0];
            }
        }
        return u;
    }

    //vector<ll> pathbetween(ll u, ll v){
    //    return {};
    //}

};

//UnionFind.hpp
/// @brief UnionFind木
/// @tparam nodeinfo 
template<typename nodeinfo>
struct UnionFind{
    vector<int> A;//根でないとき、どう辿れば根になるか(すでに根なら-1×(要素数))
    int groups;//連結成分数
    
    vector<nodeinfo> B;//各根に載っている状態を保存する。
    function<void(nodeinfo&, nodeinfo, int, int)> merge_info;//異なる連結成分をマージするときの関数{マージ先, マージ元, マージ先代表元, マージ元代表元}
    function<void(nodeinfo&, int, int)> modify_info;//同一連結成分内に対する操作を行う関数{変更される要素, それの代表元2つ}


    /// @brief 頂点番号が(0,)1,2...NのUnionFind木を構築する。全部同一の状態で初期化される
    /// @param N 頂点数の上限
    /// @param e 
    /// @param one_indexed 1-indexedかどうか
    UnionFind(const int &N, const nodeinfo &init, function<void(nodeinfo&, nodeinfo, int, int)> mergefunc, function<void(nodeinfo&, int, int)> modifyfunc, bool one_indexed = true): A(N+1,-1), groups(one_indexed ? N : N+1), B(N+1, init), merge_info(mergefunc), modify_info(modifyfunc){}

    /// @brief nodeの親を見つける
    /// @param node 
    /// @return root
    int findroot(int node){
        while (A[node] >= 0){
            node = A[node];
        }
        return node;
    }
    /// @brief node以上のノードをすべてrootに直接接続する
    /// @param node 
    /// @param root 
    void compress_path(int node, const int &root){
        int temp = node;
        while (A[temp] >= 0){
            temp = A[temp];
            A[node] = root;
            node = temp;
        }
    }

    /// @brief  二つのノードが同じグループであるかを返す
    /// @param node1 
    /// @param node2 
    /// @return true/false
    bool same_group(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);
        
        compress_path(node1, root1);
        compress_path(node2, root2);

        return root1 == root2;//判定
    }

    /// @brief node1を含むグループの根が持っている情報を返す。
    /// @param node1 
    /// @return 
    nodeinfo getinfo(int node1){
        int root1 = findroot(node1);
        compress_path(node1, root1);
        return B[root1];
    }

    /// @brief node1とnode2を含む2つのグループを合成する。すでに同じなら何もしない。
    /// @param node1 
    /// @param node2 
    void merge(int node1, int node2){
        int root1 = findroot(node1);
        int root2 = findroot(node2);

        if (root1 == root2){
            modify_info(B[root1], node1, node2);
            return;
        }

        groups--;
        
        if (-A[root1] > -A[root2]){
            A[root1] += A[root2];
            A[root2] = root1;
            merge_info(B[root1], B[root2], node1, node2);
        }
        else{
            A[root2] += A[root1];
            A[root1] = root2;
            merge_info(B[root2], B[root1], node2, node1);
        }
    }

    /// @brief 連結成分ごとに分解し、各成分に属する頂点をまとめたリストを作成する。
    /// @param one_indexed 1-indexedかどうか(デフォルトで1-indexed)
    /// @return 
    vector<vector<int>> connected_groups(bool one_indexed = true){
        unordered_map<int,vector<int>> BB;//仮の答え保存用
        vector<int> C(A.size(),-1);//どの連結成分にいるかを管理

        for (int i = one_indexed, sz = A.size(); i <= sz-1; i++){
            if (A[i] < 0){
                BB[i].push_back(i);
                C[i] = i;
            }
        }

        vector<int> passed_node;//たどっている途中の頂点を保持するスタック

        for (int i = one_indexed, sz = A.size(); i <= sz-1; i++){
            if (C[i] != -1){
                continue;
            }
            int temp = i;
            while (C[temp] == -1){
                passed_node.push_back(temp);
                temp = A[temp];
            }
            while (!passed_node.empty()){
                BB[C[temp]].push_back(passed_node.back());
                C[passed_node.back()] = C[temp];
                passed_node.pop_back();
            }
        }
        vector<vector<int>> ret;
        for (auto &pvec : BB){
            ret.push_back(pvec.second);
        }
        return ret;
    }
};

}
namespace string_algorithm{

//kmp_search.hpp
/// @brief kmp_searchに使うスキップテーブルを作成
vector<int> create_partial_match_table(const string &t){
    vector<int> table(t.size(),0);
    table[0] = -1;
    ll j = -1;
    for (int i = 0; i < (int)t.size()-1; i++){
        while (j >= 0 && t[i] != t[j]){
            j = table[j];
        }
        table[i+1] = j+1;
        j++;
    }
    return table;
}

/// @brief sの中にtが含まれているかを判定し、あるなら最初に現れる位置を求める
/// @param s 
/// @param t 
/// @return 
int kmp_search(const string &s, const string &t){
    auto table = create_partial_match_table(t);
    int i = 0, j = 0;
    while (i+j < (int)s.size()){
        if (s[i+j] == t[j]){
            j++;
            if (j == (int)t.size()){
                return i;
            }
        }
        else{
            i = i+j-table[j];
            if (j > 0){
                j = table[j];
            }
        }
    }
    return -1;
}

//rolling_hash.hpp
/// @brief ローリングハッシュの作成、接続など
struct rolling_hash{

    inline __int128_t modpow1111111111111111111(__int128_t x, int n){
        __int128_t r = 1;
        if (n){
            while (n){
                if (n%2){
                    r *= x;
                    r %= 1111111111111111111;
                }
                x *= x;
                x %= 1111111111111111111;
                n >>= 1;
            }
            return r;
        }
        return 1;
    }
    inline __int128_t modpow1234567894987654321(__int128_t x, int n){
        __int128_t r = 1;
        if (n){
            while (n){
                if (n%2){
                    r *= x;
                    r %= 1234567894987654321;
                }
                x *= x;
                x %= 1234567894987654321;
                n >>= 1;
            }
            return r;
        }
        return 1;
    }


    random_device rd;
    uniform_int_distribution<int> ui1;
    uniform_int_distribution<int> ui2;
    __int128_t B = 0, C = 0;
    __int128_t Binv,Cinv;

    /// @brief ローリングハッシュライブラリを初期化
    rolling_hash(){
        mt19937 gen(rd());
        ui1 = uniform_int_distribution<int>(134217728, 2147483647);
        ui2 = uniform_int_distribution<int>(1, 2147483647);
        //Bを設定
        while (B % 1111111111111111111 == 0){
            B = (((__int128_t)ui1(gen))<<31) + ui2(gen);
        }
        //Cを設定
        while (C % 1234567894987654321 == 0){
            C = (((__int128_t)ui1(gen))<<31) + ui2(gen);
        }
        B %= 1111111111111111111;
        C %= 1234567894987654321;
        Binv = (1111111111111111111+(math::axby1<__int128_t>(B,1111111111111111111)[0])%1111111111111111111)%1111111111111111111;
        Cinv = (1234567894987654321+(math::axby1<__int128_t>(C,1234567894987654321)[0])%1234567894987654321)%1234567894987654321;
    }

    inline __int128_t char_to_digit(char c){
        return (141*((3*c)^0b0101001101))^0b10010101110;
    }

    /// @brief 文字列Sにおいて、indexが0以上i以下の部分文字列のローリングハッシュを生成する。
    /// @param S 
    /// @return ローリングハッシュを記録した配列
    vector<pll> gen_hash(const string &S){
        vector<pll> r(S.size());
        r[0] = {(ll)char_to_digit(S[0]), (ll)char_to_digit(S[0])};
        array<__int128_t, 2> temp{B, C};
        for (size_t i = 1; i < S.size(); i++){
            r[i][0] = (r[i-1][0] + char_to_digit(S[i])*temp[0])%1111111111111111111;
            r[i][1] = (r[i-1][1] + char_to_digit(S[i])*temp[1])%1234567894987654321;
            temp[0] *= B;
            temp[0] %= 1111111111111111111;
            temp[1] *= C;
            temp[1] %= 1234567894987654321;
        }
        return r;
    }

    /// @brief indexが`l`以上`r`以下の部分文字列のハッシュを生成する。
    /// @param l 
    /// @param r 
    /// @return `[l,r]`のハッシュ
    pll get_hash(const vector<pll> &hs, int l, int r){
        array<__int128_t, 2> ret = {1111111111111111111 + hs[r][0] - (l == 0 ? 0 : hs[l-1][0]), 1234567894987654321 + hs[r][1] - (l == 0 ? 0 : hs[l-1][1])};
        ret[0] %= 1111111111111111111;
        ret[1] %= 1234567894987654321;
        ret[0] = (ret[0]*modpow1111111111111111111(Binv, l))%1111111111111111111;
        ret[1] = (ret[1]*modpow1234567894987654321(Cinv, l))%1234567894987654321;
        return {(ll)ret[0], (ll)ret[1]};
    }

    /// @brief aのハッシュ、aの長さ、bのハッシュを用いてa+bのハッシュを生成する。
    /// @param a 
    /// @param len_a 
    /// @param b 
    /// @return 
    pll connect_hash(const pll &a, int len_a, const pll &b){
        return {(ll)((a[0]+(b[0]*modpow1111111111111111111(B,len_a))%1111111111111111111)%1111111111111111111), (ll)((a[1]+(b[1]*modpow1234567894987654321(B,len_a))%1234567894987654321)%1234567894987654321)};
    }
};


}
namespace general_algorithm{

/// @brief 累積和を作成する
template<typename T>
vector<T> cumulative_sum(vector<T> A){
    for (size_t i = 1, sz = A.size(); i < sz; i++){
        A[i] += A[i-1];
    }
    return A;
}

/// @brief vectorの和を求める
/// @tparam T 
/// @param A 
/// @return 
template<typename T>
T sum(const vector<T> &A){
    T init_val = 0;
    for (auto &v : A){
        init_val += v;
    }
    return init_val;
}

/// @brief 前方向イテレーターで半壊区間を指定して、その間の要素の和を求める。
/// @tparam T 
/// @param A 
/// @return
template<typename Iterator>
ll sum(Iterator begin, Iterator end){
    ll init_val = 0;
    while (begin != end){
        init_val += *begin;
        ++begin;
    }
    return init_val;
}


//RLE.hpp
/// @brief AをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
template<typename T>
vector<pair<T,ll>> RLEvec(const vector<T> &A){
    vector<pair<T,ll>> R;
    T previous = A[0];
    ll combo = 0;
    for (int i = 0;;){
        if (i == (int)A.size()){
            R.push_back({previous,combo});
            break;
        }
        if (A[i] == previous){
            combo++;
            i++;
        }
        else{
            R.push_back({previous, combo});
            previous = A[i];
            combo = 0;
        }
    }
    return R;
}

/// @brief SをRLEした結果を返す。{{何が, 何個}...}の形で返される。
/// @param A 
/// @return 
vector<pair<char,ll>> RLEstr(const string &S){
    vector<pair<char,ll>> R;
    char previous = S[0];
    ll combo = 0;
    for (int i = 0;;){
        if (i == (int)S.size()){
            R.push_back({previous,combo});
            break;
        }
        if (S[i] == previous){
            combo++;
            i++;
        }
        else{
            R.push_back({previous, combo});
            previous = S[i];
            combo = 0;
        }
    }
    return R;
}

//LIS.hpp
/// @brief 最長増加部分列（LIS）の長さを返します
/// @tparam Strict 狭義単調増加の場合 true, 広義単調増加の場合 false
/// @tparam Type 数列の要素の型
/// @param v 数列
/// @return 最長増加部分列（LIS）の長さ
/// @note 1.2 最長増加部分列の長さの取得
/// @see https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming/viewer/lis
template <bool Strict, class Type>
size_t LIS_length(const std::vector<Type>& v){
    std::vector<Type> dp;

    auto it = dp.begin();

    for (const auto& elem : v)
    {
        if constexpr (Strict)
        {
            it = std::lower_bound(dp.begin(), dp.end(), elem);
        }
        else
        {
            it = std::upper_bound(dp.begin(), dp.end(), elem);
        }

        if (it == dp.end())
        {
            dp.push_back(elem);
        }
        else
        {
            *it = elem;
        }
    }

    return dp.size();
}

/// @brief 最長増加部分列（LIS）のインデックスを返します
/// @tparam Strict 狭義単調増加の場合 true, 広義単調増加の場合 false
/// @tparam Type 数列の要素の型
/// @param v 数列
/// @return 最長増加部分列（LIS）のインデックス
/// @note 1.4 最長増加部分列の復元
/// @see https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming/viewer/lis
template <bool Strict, class Type>
std::vector<int> LIS_construction(const std::vector<Type>& v)
{
    std::vector<Type> dp;

    auto it = dp.begin();

    std::vector<int> positions;

    for (const auto& elem : v)
    {
        if constexpr (Strict)
        {
            it = std::lower_bound(dp.begin(), dp.end(), elem);
        }
        else
        {
            it = std::upper_bound(dp.begin(), dp.end(), elem);
        }

        positions.push_back(static_cast<int>(it - dp.begin()));

        if (it == dp.end())
        {
            dp.push_back(elem);
        }
        else
        {
            *it = elem;
        }
    }

    std::vector<int> subseq(dp.size());

    int si = static_cast<int>(subseq.size()) - 1;

    int pi = static_cast<int>(positions.size()) - 1;

    while ((0 <= si) && (0 <= pi))
    {
        if (positions[pi] == si)
        {
            subseq[si] = pi;

            --si;
        }

        --pi;
    }

    return subseq;
}

}

}

using namespace a1073741824;
using namespace AVL;
using namespace math;
using namespace array_datastructure;
using namespace graph;
using namespace string_algorithm;
using namespace general_algorithm;
using mll = math::mint<998244353>;
using dmll = math::dynamic_mint;



void solve(){
    
}

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ll T = 1;
    //cin >> T;
    while (T--){
        solve();
    }
}


