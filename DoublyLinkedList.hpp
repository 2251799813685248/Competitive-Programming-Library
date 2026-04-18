/// @brief 双方向連結リスト
/// @tparam T 
template<typename T>
struct DoublyLinkedList{

    struct LinkedListNode{
        T val;
        LinkedListNode* prev;
        LinkedListNode* next;
    };

    private: LinkedListNode* front;//先頭
    private: LinkedListNode* back;//末尾の一個次
    private: size_t sz;

    public:

    DoublyLinkedList(){
        sz = 0;
        back = new LinkedListNode;
        back->prev = nullptr;
        back->next = nullptr;

        front = back;
    }

    /// @brief 双方向イテレーター
    struct iterator{
        // イテレータの型定義
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        LinkedListNode* LLN;
        iterator(LinkedListNode* __lln): LLN(__lln){}
        iterator(){}

        T& operator*() const{
            if (LLN == nullptr || LLN->next == nullptr){
                cerr << "Getting Element of end() iterator" << endl;
                assert(false);
            }
            return LLN->val;
        }

        bool operator==(const iterator &other) const {return LLN == other.LLN;}
        bool operator!=(const iterator &other) const {return LLN != other.LLN;}

        iterator& operator++(){
            if (LLN->next == nullptr){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            LLN = LLN->next;
            return *this;
        }
        iterator operator++(int){
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--(){
            if (LLN->prev == nullptr){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            LLN = LLN->prev;
            return *this;
        }
        iterator operator--(int){
            iterator temp = *this;
            --(*this);
            return temp;
        }

        //iterator& operator-=(const int &amount);
        iterator& operator+=(const int &amount){
            if (amount < 0){
                return (*this) -= -amount;
            }
            for (int i = 0; i < amount; i++){
                if (LLN->next == nullptr){
                    cerr << "Index Out Of Bounds(iterator access)" << endl;
                    assert(false);
                }
                LLN = LLN->next;
            }
            return *this;
        }
        iterator operator+(const int &amount) const{
            iterator temp = *this;
            temp += amount;
            return temp;
        }

        iterator& operator-=(const int &amount){
            if (amount < 0){
                return (*this) += -amount;
            }
            for (int i = 0; i < amount; i++){
                if (LLN->prev == nullptr){
                    cerr << "Index Out Of Bounds(iterator access)" << endl;
                    assert(false);
                }
                LLN = LLN->prev;
            }
            return *this;
        }
        iterator operator-(const int &amount) const{
            iterator temp = *this;
            temp -= amount;
            return temp;
        }

        difference_type operator-(const iterator &other) const{
            int temp = 0;
            while (this->LLN != other.LLN){
                this->LLN = this->LLN->next;
                temp++;
            }
            return temp;
        }
    };

    iterator begin() const{
        return iterator(front);
    }
    iterator end() const{
        return iterator(back);
    }

    void push_back(const T &val){
        back->val = val;
        back->next = new LinkedListNode;
        back->next->prev = back;
        back = back->next;
        sz++;
    }
    void pop_back(){
        if (sz == 0){return;}
        back = back->prev;
        delete back->next;
    }
    void push_front(const T &val){
        front->prev = new LinkedListNode{val, nullptr, front};
    }
    void pop_front(){
        if (sz == 0){return;}
        front = front->next;
        delete front->prev;
    }
    void insert_after(const iterator itr, const T &val){
        if (itr == end()){return;}
        itr.LLN->next->prev = new LinkedListNode{val, itr.LLN, itr.LLN->next};
        itr.LLN->next = itr.LLN->next->prev;
    }
    void erase(iterator itr){
        if (itr.LLN == back){return;}
        if (itr.LLN == front){
            pop_front();
        }
        else{
            itr.LLN->prev->next = itr.LLN->next;
            itr.LLN->next->prev = itr.LLN->prev;
            delete itr.LLN;
        }
    }
};
