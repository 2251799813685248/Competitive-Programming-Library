using namespace std;

// ------------------------------
// 1. ノードの定義 (テンプレート化)
// ------------------------------
template <typename T>
struct AVLmultisetNode {
    T key; // intではなくT型にする
    AVLmultisetNode* left;
    AVLmultisetNode* right;
    AVLmultisetNode* parent; //親へのポインタ
    int size; //この部分木に含まれるノードの総数
    int height;

    AVLmultisetNode(T k, AVLmultisetNode* p = nullptr) : key(k), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
};


// ------------------------------
// 2. AVL木クラス (テンプレート化)
// ------------------------------
template <typename T>
struct AVLmultiset {
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
    bool contains(const T& key) {
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

    iterator lower(const T& key){
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
    iterator upper(const T& key){
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

