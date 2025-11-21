


//=======AVLset=========


/// @brief AVLset用ノード
/// @tparam T
template<typename T>
struct AVLsetNode {
    T key; //値
    AVLsetNode* left;
    AVLsetNode* right;
    AVLsetNode* parent; //親へのポインタ
    int size; //この部分木に含まれるノードの総数
    int height; //部分木から各ノードへの経路長の最大値

    AVLsetNode(T k, AVLsetNode* p = nullptr) : key(k), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
};

/// @brief AVL木による重複なし集合
/// @tparam T 木に載せる要素の型
template <typename T>
struct AVLset {
private: int treesize = 0;//全体の要素数
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
    bool contains(const T& key) {
        AVLsetNode<T>* current = root;
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
    iterator lower(const T& key){
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
    iterator upper(const T& key){
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


    void inOrder(AVLsetNode<T>* root) {
        if (root != nullptr) {
            inOrder(root->left);
            // T型を cout で出力できる必要がある
            std::cout << root->key << " "; 
            inOrder(root->right);
        }
    }
};



//===============AVLmap=====================


/// @brief AVLmap用ノード
/// @tparam T 
template <typename T, typename U>
struct AVLmapNode {
    T key; //連想配列のキー
    U value; //連想配列に入れる値
    AVLmapNode* left;
    AVLmapNode* right;
    AVLmapNode* parent; //親へのポインタ
    int size; //この部分木に含まれるノードの総数
    int height; //部分木から各ノードへの経路長の最大値

    AVLmapNode(T k, U v, AVLmapNode* p = nullptr) : key(k), value(v), left(nullptr), right(nullptr), parent(p), size(1), height(1) {}
};


/// @brief AVL木による連想配列
/// @tparam T 木に載せる要素の型
template <typename T, typename U>
struct AVLmap {
private: int treesize = 0;//全体の要素数
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

        pair<T,U>& operator*() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return make_pair(current->key, current->value);
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
    bool contains(const T& key) {
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
    iterator lower(const T& key){
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
    iterator upper(const T& key){
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


    void inOrder(AVLmapNode<T,U>* root) {
        if (root != nullptr) {
            inOrder(root->left);
            // T型を cout で出力できる必要がある
            std::cout << root->key << ":" << root->value << " "; 
            inOrder(root->right);
        }
    }
};

