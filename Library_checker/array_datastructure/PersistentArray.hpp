#ifndef PERSISTENT_ARRAY_HPP_
#define PERSISTENT_ARRAY_HPP_

#include <iostream>
#include <vector>
#include <array>
using namespace std;


/// @brief 固定長永続配列
/// @tparam T 
template<typename T>
struct PersistentArray{

    /// @brief 永続配列用のノード
    struct parrnode{
        T value;
        array<parrnode*,4> child;//子
    };

    int max_size;
    int depth;//木の高さ(根の高さは0)

    parrnode* root;//根のポインタ

    PersistentArray(const int &sz, const T &init_val){
        if (sz >= 134217728){
            cerr << "Too Large Size" << endl;
            assert(false);
        }
        max_size = sz;
        depth = (33-__builtin_clz(sz-1))/2;
        root = new parrnode;
        int tempd = 0;
        dfs_construction_value(root, 0, tempd, init_val);
    }
    PersistentArray(const vector<T> &A){
        int sz = A.size();
        max_size = sz;
        depth = (33-__builtin_clz(sz-1))/2;
        root = new parrnode;
        int tempd = 0;
        dfs_construction_vector(root, 0, tempd, A);
    }
    PersistentArray(const PersistentArray* pa){
        root = new parrnode;
        root->child = pa->root->child;
        max_size = pa->max_size;
        depth = pa->depth;
    }
    PersistentArray(){
        root = nullptr;
        depth = 0;
        max_size = 0;
    }

    PersistentArray(initializer_list<T> _init): PersistentArray(vector<T>(_init)){};

    PersistentArray clone(){
        return PersistentArray(this);
    }

    void dfs_construction_value(parrnode* parent, const int &idx, int &d, const T &init_val){
        if (d == depth){
            parent->child[0] = nullptr;
            parent->child[1] = nullptr;
            parent->child[2] = nullptr;
            parent->child[3] = nullptr;
            parent->value = init_val;
            return;
        }
        d++;
        for (int i = 0; i < 4; i++){
            if ((idx<<2)+i >= max_size){parent->child[i] = nullptr;continue;}
            parent->child[i] = new parrnode;
            dfs_construction_value(parent->child[i], (idx<<2)+i, d, init_val);
        }
        d--;
    }
    void dfs_construction_vector(parrnode* parent, const int &idx, int &d, const vector<T> &A){
        if (d == depth){
            parent->child[0] = nullptr;
            parent->child[1] = nullptr;
            parent->child[2] = nullptr;
            parent->child[3] = nullptr;
            parent->value = A[idx];
            return;
        }
        d++;
        for (int i = 0; i < 4; i++){
            if ((idx<<2)+i >= max_size){parent->child[i] = nullptr;continue;}
            parent->child[i] = new parrnode;
            dfs_construction_vector(parent->child[i], (idx<<2)+i, d, A);
        }
        d--;
    }

    /// @brief ランダムアクセスイテレーター
    struct iterator{
        // イテレータの型定義
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        const PersistentArray* pa;
        int idx;
        iterator(const PersistentArray* __pa, const int &__idx): pa(__pa), idx(__idx){
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
        }

        T& operator*() const{
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            if (idx == pa->max_size){
                cerr << "Getting Element of end() iterator" << endl;
                assert(false);
            }
            return pa->get(idx);
        }

        T* operator->() const{
            return &pa->get(idx);
        }

        bool operator==(const iterator &other) const {return idx == other.idx;}
        bool operator!=(const iterator &other) const {return idx != other.idx;}
        bool operator<(const iterator &other) const {return idx < other.idx;}
        bool operator>(const iterator &other) const {return idx > other.idx;}
        bool operator<=(const iterator &other) const {return idx <= other.idx;}
        bool operator>=(const iterator &other) const {return idx >= other.idx;}

        iterator& operator++(){
            idx++;
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            return *this;
        }
        iterator operator++(int){
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--(){
            idx--;
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
            }
            return *this;
        }
        iterator operator--(int){
            iterator temp = *this;
            --(*this);
            return temp;
        }

        iterator& operator+=(const int &amount){
            idx += amount;
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            return *this;
        }
        iterator operator+(const int &amount) const{
            iterator temp = *this;
            temp.idx += amount;
            return temp;
        }

        iterator& operator-=(const int &amount){
            idx -= amount;
            if (idx < 0 || idx > pa->max_size){
                cerr << "Index Out Of Bounds(iterator access)" << endl;
                assert(false);
            }
            return *this;
        }
        iterator operator-(const int &amount) const{
            iterator temp = *this;
            temp.idx -= amount;
            return temp;
        }

        difference_type operator-(const iterator &other) const{
            return idx - other.idx;
        }

        T& operator[](const int &i) const{
            return pa->get(idx + i);
        }

    };

    iterator begin() const{return iterator(this, 0);}
    iterator end() const{return iterator(this, max_size);}


    /// @brief indexを指定して要素にアクセスする。変更はできない。
    /// @attention 基本的には[]を使う
    /// @param idx
    /// @return value
    T& get(const int idx) const{
        if (idx >= max_size || idx < 0){
            cerr << "Index Out of Bounds(index access)" << endl;
            assert(false);
        }
        parrnode* tracking_node = root;
        int pos = (this->depth-1)<<1;
        while (pos >= 0){
            tracking_node = tracking_node->child[(idx>>pos)&3];
            pos -= 2;
        }
        return tracking_node->value;
    }
    /// @brief indexを指定して、書き換え先の参照を得る。変更ができる。
    /// @param idx 
    /// @return `value&`
    T& getref(const int idx){
        if (idx >= max_size || idx < 0){
            cerr << "Index Out of Bounds(index access)" << endl;
            assert(false);
        }
        parrnode* tracking_node_1 = root;
        parrnode* tracking_node_2 = root;
        int pos = (this->depth-1)<<1;
        while (pos >= 0){
            tracking_node_1 = tracking_node_1->child[(idx>>pos)&3];
            tracking_node_2->child[(idx>>pos)&3] = new parrnode;
            tracking_node_2 = tracking_node_2->child[(idx>>pos)&3];
            tracking_node_2->child = tracking_node_1->child;
            pos -= 2;
        }
        tracking_node_2->value = tracking_node_1->value;
        return tracking_node_2->value;
    }
    /// @brief indexを指定して要素を書き換える。
    /// @attention 基本的には[]を使う
    /// @param idx 
    void write(const int idx, const T &value){
        if (idx >= max_size || idx < 0){
            cerr << "Index Out of Bounds(index access)" << endl;
            assert(false);
        }
        parrnode* tracking_node_1 = root;
        parrnode* tracking_node_2 = root;
        int pos = (this->depth-1)<<1;
        while (pos >= 0){
            tracking_node_1 = tracking_node_1->child[(idx>>pos)&3];
            tracking_node_2->child[(idx>>pos)&3] = new parrnode;
            tracking_node_2 = tracking_node_2->child[(idx>>pos)&3];
            tracking_node_2->child = tracking_node_1->child;
            pos -= 2;
        }
        tracking_node_2->value = value;
    }
    size_t size(){
        return max_size;
    }
};




#endif /* PERSISTENT_ARRAY_HPP_ */