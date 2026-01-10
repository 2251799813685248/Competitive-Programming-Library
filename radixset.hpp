/// @brief 64bit整数用のset
struct radixset{
    private:
    struct radixsetnode{
        int child_num;//自分以下のノードの数の総和
        ull value;
        char d;//このノードは10^dの位を扱う
        array<radixsetnode*, 10> child{};//次の位が何かによって子を分類
        radixsetnode(const int &child_num_, const int &d_): child_num(child_num_), d(d_){}
    };

    radixsetnode* root;
    
    public:
    
    radixset(): root(new radixsetnode(0, 20)){};

    ///双方向イテレーターの実装
    class iterator{
        public:
        radixsetnode* current;
        int order;//同一のノードの中で何番目を指しているか
        const radixset* tree; // 所属する木へのポインタ


        // コンストラクタで木への参照を受け取る
        iterator(radixsetnode* node, const radixset* t) : current(node), tree(t) {}

        ll operator*() const {
            if (current == nullptr) {
                cerr << "Error: Getting element of end() iterator" << endl;
                assert(false);
            }
            return current->value-9223372036854775808ull;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }

        // --- 前方移動 (++) ---
        iterator& operator++() {
            
        }

        // --- 後方移動 (--) ---
        iterator& operator--() {
        
        }
    };

    iterator begin() const{

    }
    iterator end() const{
        
    }


    int count(const ll &x){
        ull v = x + 9223372036854775808ull;
        radixsetnode* tracking_node = root;
        while (tracking_node->d > 0){
            if (tracking_node->child[(v/pow10ll[tracking_node->d-1])%10] == nullptr){
                return 0;
            }
            tracking_node = tracking_node->child[(v/pow10ll[tracking_node->d-1])%10];
        }
        return tracking_node->child_num;
    }

    void insert(const ll &x){
        ull v = x + 9223372036854775808ull;
        radixsetnode* tracking_node = root;
        while (tracking_node->d > 0){
            if (tracking_node->child[(v/pow10ll[tracking_node->d-1])%10] == nullptr){
                tracking_node->child[(v/pow10ll[tracking_node->d-1])%10] = new radixsetnode(0, tracking_node->d-1);
            }
            tracking_node->child_num++;
            tracking_node = tracking_node->child[(v/pow10ll[tracking_node->d-1])%10];
        }
        tracking_node->child_num++;
        tracking_node->value = v;
    }

    iterator find(const ll &x){
        ull v = x + 9223372036854775808ull;
        radixsetnode* tracking_node = root;
        while (tracking_node->d > 0){
            if (tracking_node->child[(v/pow10ll[tracking_node->d-1])%10] == nullptr){
                return end();
            }
            tracking_node = tracking_node->child[(v/pow10ll[tracking_node->d-1])%10];
        }
        return iterator(tracking_node, this);
    }

    
    private:
    radixsetnode getminnode(){

    }
    


    

};
