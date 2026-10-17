#ifndef LIST__HPP_
#define LIST__HPP_

#include <algorithm>
#include <iostream>
#include <cassert>
#include <functional>
#include <utility>
using uint = unsigned int;
using ll = long long;
using namespace std;


template<typename T> concept has_identity = requires (){T();};
template<typename T> concept has_plus_operator = requires (T a, T b){a+b;};
template<typename T> concept has_compare_operator = requires (T a, T b){a < b;};
template<typename T> concept has_equal_operator = requires (T a, T b){a == b; a != b;};


template<typename T>
class list{
    public:

    using value_type = T;

    constexpr list(): sz_(0), max_sz_(0), A(nullptr){}
    constexpr list(uint sz): sz_(sz), max_sz_(sz){
        if (sz == 0){A = nullptr; return;}
        if (sz_ > 2147483647){cerr << "ValueError: too large or negative size" << endl; assert(false);}
        A = new T[sz_]{};
        if (A == nullptr){cerr << "MomoryError: failed to allocate memory" << endl; assert(false);}
    }
    constexpr list(uint sz, T elem): sz_(sz), max_sz_(sz){
        if (sz == 0){A = nullptr; return;}
        if (sz_ > 2147483647){cerr << "ValueError: too large or negative size" << endl; assert(false);}
        A = new T[sz_]{};
        if (A == nullptr){cerr << "MemoryError: failed to allocate memory" << endl; assert(false);}
        for (int i = 0; i < sz_; i++){
            A[i] = elem;
        }
    }
    constexpr list(initializer_list<T> init): sz_(init.size()), max_sz_(init.size()){
        A = new T[sz_]{};
        if (A == nullptr){cerr << "MemoryError: failed to allocate memory" << endl; assert(false);}
        for (int i = 0; i < sz_; i++){
            A[i] = *(init.begin()+i);
        }
    };
    // constexpr list<ll>(const range& r){
    //     if (r.step_ == 0){cerr << "ValueError: step argument must not be zero" << endl; assert(false);}
    //     if (r.step_ > 0){
    //         int imax = r.begin_ >= r.end_ ? 0 : (r.end_-r.begin_+r.step_-1)/r.step_;
    //         sz_ = imax;
    //         max_sz_ = imax;
    //         A = new T[sz_]{};
    //         if (A == nullptr){cerr << "MemoryError: failed to allocate memory" << endl; assert(false);}
    //         for (int i = 0; i < imax; i++){
    //             int target = r.begin_+r.step_*i;
    //             A[i] = target;
    //         }    
    //     }
    //     else{
    //         int imax = r.end_ >= r.begin_ ? 0 : (r.begin_-r.end_-r.step_-1)/(-r.step_);
    //         sz_ = imax;
    //         max_sz_ = imax;
    //         A = new T[sz_]{};
    //         if (A == nullptr){cerr << "MemoryError: failed to allocate memory" << endl; assert(false);}
    //         for (int i = 0; i < imax; i++){
    //             int target = r.begin_+r.step_*i;
    //             A[i] = target;
    //         }
    //     }
    // }
    constexpr list(const list<T>& other): sz_(other.sz_), max_sz_(other.max_sz_){
        A = new T[max_sz_]{};
        for (int i = 0; i < sz_; i++){
            A[i] = other.A[i];
        }
    }
    constexpr list(list<T>&& other) noexcept : sz_(other.sz_), max_sz_(other.max_sz_){
        A = other.A;
        other.sz_ = 0;
        other.max_sz_ = 0;
        other.A = nullptr;
    }
    constexpr ~list() {
        delete[] A;
    }

    class iterator{
        public:
        using difference_type = ll;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
        
        iterator() = delete;
        constexpr iterator(const list* listptr_init, const int &idx_init): listptr(listptr_init), idx(idx_init){}

        inline constexpr value_type& operator* () const {return listptr->A[this->check_idx()];}
        inline constexpr value_type* operator->() const {return &listptr->A[this->check_idx()];}

        inline constexpr bool operator==(const iterator &other) const {return idx == other.idx;}
        inline constexpr bool operator!=(const iterator &other) const {return idx != other.idx;}
        inline constexpr bool operator<(const iterator &other) const {return idx < other.idx;}
        inline constexpr bool operator>(const iterator &other) const {return idx > other.idx;}
        inline constexpr bool operator<=(const iterator &other) const {return idx <= other.idx;}
        inline constexpr bool operator>=(const iterator &other) const {return idx >= other.idx;}

        inline constexpr iterator operator++(){
            this->idx++;
            return *this;
        }
        inline constexpr iterator operator++(int){
            iterator temp = *this;
            this->idx++;
            return temp;
        }
        inline constexpr iterator operator--(){
            this->idx--;
            return *this;
        }
        inline constexpr iterator operator--(int){
            iterator temp = *this;
            this->idx--;
            return temp;
        }

        inline constexpr iterator& operator+=(int amount){
            this->idx += amount;
            return *this;
        }
        inline constexpr iterator operator+(int amount) const {
            iterator temp = *this;
            temp.idx += amount;
            return temp;
        }
        inline constexpr iterator& operator-=(const int &amount){
            this->idx -= amount;
            return *this;
        }
        inline constexpr iterator operator-(const int &amount) const{
            iterator temp = *this;
            temp.idx -= amount;
            return temp;
        }

        inline constexpr difference_type operator-(const iterator &other) const{
            return idx - other.idx;
        }

        
        inline constexpr int check_idx() const {
            if (idx < 0 || idx >= listptr->sz_){
                cerr << "IteratorIndexError: index out of range. current size: " << listptr->sz_ << " target index: " << idx << endl; assert(false);
            }
            return idx;
        }

        private:
        
        const list* listptr;
        int idx;
    };

    inline constexpr iterator begin(){return iterator(this, 0);}
    inline constexpr iterator begin() const {return iterator(this, 0);}
    inline constexpr iterator end(){return iterator(this, sz_);}
    inline constexpr iterator end() const {return iterator(this, sz_);}

    inline constexpr list& operator=(const list& other){
        if (this == &other){return *this;}
        sz_ = other.sz_;
        max_sz_ = other.max_sz_;
        delete[] A;
        A = new T[max_sz_]{};
        for (int i = 0; i < sz_; i++){
            A[i] = other.A[i];
        }
        return *this;
    }
    inline constexpr list& operator=(list&& other) noexcept {
        if (this == &other){return *this;}
        delete[] A;
        sz_ = other.sz_;
        max_sz_ = other.max_sz_;
        A = other.A;
        other.sz_ = 0;
        other.max_sz_ = 0;
        other.A = nullptr;
        return *this;
    }
    inline constexpr bool operator==(const list& other) const {
        if (sz_ != other.sz_){return false;}
        bool res = true;
        for (int i = 0; i < sz_; i++){
            if (A[i] != other.A[i]){
                res = false;
                break;
            }
        }
        return res;
    }
    inline constexpr bool operator!=(const list& other) const {
        return !this->operator==(other);
    }
    inline constexpr bool operator<(const list& other) const {
        int lcp_back = 0;
        for (int i = 0, sz2 = min(sz_, other.sz_); i < sz2; i++){
            if (A[i] != other.A[i]){break;}
            lcp_back++;
        }
        if (lcp_back == sz_ && lcp_back == other.sz_){return false;}
        else if (lcp_back == sz_){return true;}
        else if (lcp_back == other.sz_){return false;}
        else{return A[lcp_back] < other.A[lcp_back];}
    }
    inline constexpr bool operator<=(const list& other) const {
        int lcp_back = 0;
        for (int i = 0, sz2 = min(sz_, other.sz_); i < sz2; i++){
            if (A[i] != other.A[i]){break;}
            lcp_back++;
        }
        if (lcp_back == sz_ && lcp_back == other.sz_){return true;}
        else if (lcp_back == sz_){return true;}
        else if (lcp_back == other.sz_){return false;}
        else{return A[lcp_back] < other.A[lcp_back];}
    }
    inline constexpr bool operator>(const list& other) const {return other < *this;}
    inline constexpr bool operator>=(const list& other) const {return other <= *this;}
    inline constexpr list& operator+=(const list& other){
        for (int i = 0, sz2 = other.size(); i < sz2; i++){
            append(other.A[i]);
        }
        return *this;
    }
    inline constexpr list operator+(const list& other) const {
        return list(*this) += other;
    }
    inline constexpr list& operator+=(const T& val){
        append(val);
        return *this;
    }
    inline constexpr list operator+(const T& val) const {
        return list(*this) += val;
    }
    
    

    inline constexpr void append(const T& val){
        if (sz_ < max_sz_){
            A[sz_] = val;
            sz_++;
        }
        else{
            if (max_sz_ < 2){max_sz_ = 2;}
            else{max_sz_ += max_sz_>>1;}
            auto B = new T[max_sz_]{};
            B[sz_] = val;
            for (int i = 0; i < sz_; i++){B[i] = std::move(A[i]);}
            sz_++;
            delete[] A;
            A = B;
        }
    }
    inline constexpr void append(T&& val){
        if (sz_ < max_sz_){
            A[sz_] = std::move(val);
            sz_++;
        }
        else{
            if (max_sz_ < 2){max_sz_ = 2;}
            else{max_sz_ += max_sz_>>1;}
            auto B = new T[max_sz_]{};
            B[sz_] = std::move(val);
            for (int i = 0; i < sz_; i++){B[i] = std::move(A[i]);}
            sz_++;
            delete[] A;
            A = B;
        }
    }
    inline constexpr T pop(){
        if (sz_ == 0){cerr << "IndexError: pop from empty list" << endl; assert(false);}
        sz_--;
        return std::move(A[sz_]);
    }
    inline constexpr void reserve(uint new_sz){
        if (sz_ > 2147483647){cerr << "ValueError: too large or negative size" << endl; assert(false);}
        if ((int)new_sz <= max_sz_){return;}
        max_sz_ = new_sz;
        auto B = new T[new_sz]{};
        if (B == nullptr){cerr << "MemoryError: failed to reallocate memory" << endl; assert(false);}
        for (int i = 0; i < sz_; i++){B[i] = std::move(A[i]);}
        delete[] A;
        A = B;
    }
    inline constexpr void resize(uint new_sz, T elem = T()){
        if (sz_ > 2147483647){cerr << "ValueError: too large or negative size" << endl; assert(false);}
        if ((int)new_sz <= max_sz_){
            while (sz_ > (int)new_sz){pop();}
            for (int i = sz_, new_sz_int = new_sz; i < new_sz_int; i++){A[i] = elem;}
            sz_ = new_sz;
            return;
        }
        max_sz_ = new_sz + (new_sz >> 1);
        auto B = new T[max_sz_]{};
        if (B == nullptr){cerr << "MemoryError: failed to reallocate memory" << endl; assert(false);}
        for (int i = 0; i < sz_; i++){B[i] = std::move(A[i]);}
        for (int i = sz_, new_sz_int = new_sz; i < new_sz_int; i++){B[i] = elem;}
        sz_ = new_sz;
        delete[] A;
        A = B;
    }
    template <class InputIterator> inline constexpr void assign(InputIterator first, InputIterator last){
        reserve(last-first);
        while (first != last){append(*first); first++;}
    }
    inline constexpr void assign(uint n, const T& val){
        reserve(n);
        sz_ = n;
        for (uint i = 0; i < n; i++){
            A[i] = val;
        }
    }
    inline constexpr iterator insert(iterator pos, const T& val){
        int idx = pos.check_idx();
        append(val);
        for (int i = idx; i < sz_-1; i++){swap(A[i], A[sz_-1]);}
        return pos;
    }
    // inline constexpr void insert(iterator pos, iterator begin, iterator end)
    inline constexpr iterator erase(iterator pos){
        int idx = pos.check_idx();
        for (int i = idx; i < sz_-1; i++){
            swap(A[i], A[i+1]);
        }
        sz_ -= 1;
        return pos;
    }
    inline constexpr iterator erase(iterator start, iterator end){
        int startidx = start.check_idx(), endidx = end.check_idx();
        if (startidx > endidx){cerr << "IndexError: invalid interval: [start, end) = []" << startidx << ", " << endidx << ")" << endl; assert(false);}
        for (int i = startidx; i < endidx; i++){
            A[i].~T();
        }
        if (startidx < endidx){
            for (int i = endidx; i < sz_; i++){
                swap(A[i+startidx-endidx], A[i]);
            }
        }
        sz_ -= endidx-startidx;
        return end;
    }
    inline constexpr void clear(){sz_ = 0; max_sz_ = 0; delete[] A;}
    inline constexpr const T& operator[](int idx) const {
        check_and_adjust_idx(idx);
        return A[idx];
    }
    inline constexpr T& operator[](int idx){
        check_and_adjust_idx(idx);
        return A[idx];
    }
    inline constexpr T& front(){return (*this)[0];}
    inline constexpr const T& front() const {return (*this)[0];}
    inline constexpr T& back(){return (*this)[-1];}
    inline constexpr const T& back() const {return (*this)[-1];}
    inline constexpr list<T> operator[](int left, int right, int step = 1) const {
        if (step == 0){cerr << "ValueError: step argument must not be zero" << endl; assert(false);}
        if (step > 0){
            int imax = left >= right ? 0 : (right-left+step-1)/step;
            list<T> B(imax);
            for (int i = 0; i < imax; i++){
                int target = left+step*i;
                if (target < 0 || target >= sz_){cerr << "IndexError: slice index out of range. current size: " << sz_ << " target index: " << target << endl; assert(false);}
                B[i] = A[target];
            }
            return std::move(B);
        }
        else{
            int imax = right >= left ? 0 : (left-right-step-1)/(-step);
            list<T> B(imax);
            for (int i = 0; i < imax; i++){
                int target = left+step*i;
                if (target < 0 || target >= sz_){cerr << "IndexError: slice index out of range. current size: " << sz_ << " target index: " << target << endl; assert(false);}
                B[i] = A[target];
            }
            return std::move(B);
        }
    }
    inline constexpr T* data() {return empty() ? nullptr : &A[0];}
    inline constexpr const T* data() const {return empty() ? nullptr : &A[0];}
    inline constexpr T sum() const requires has_plus_operator<T> && has_identity<T>{
        auto res = T();
        for (int i = 0; i < sz_; i++){
            res += A[i];
        }
        return res;
    }
    template<typename U> inline constexpr U sum(U init) const requires has_plus_operator<T>{
        for (int i = 0; i < sz_; i++){
            init += A[i];
        }
        return init;
    }
    inline constexpr void sort(bool reverse = false) requires has_compare_operator<T> {
        if (reverse){
            std::sort(begin(), end(), std::move([](const T& a, const T& b){return b < a;}));
        }
        else{
            std::sort(begin(), end());
        }
    }
    inline constexpr void sort(const function<bool(T,T)>& f) requires has_compare_operator<T> {
        std::sort(begin(), end(), f);
    }
    inline constexpr void reverse(){
        std::reverse(begin(), end());
    }
    inline constexpr ll count(const T& val){
        ll res = 0;
        for (int i = 0; i < sz_; i++){res += A[i] == val;}
        return res;
    }
    inline constexpr void apply(const function<T(T)>& f){for (int i = 0; i < sz_; i++){A[i] = f(A[i]);}}
    inline constexpr list<pair<int, T>> RLE(){
        list<pair<int, T>> res;
        for (int l = 0, r = 0; l < sz_;){
            while (r < sz_ && A[l] == A[r]){
                r++;
            }
            res.append(make_pair(r-l, A[l]));
            l = r;
        }
        return res;
    }
    inline constexpr iterator lower_bound(const T& val) const requires has_compare_operator<T> {return std::lower_bound(begin(), end(), val);}
    inline constexpr iterator upper_bound(const T& val) const requires has_compare_operator<T> {return std::upper_bound(begin(), end(), val);}
    inline constexpr ll size() const {return sz_;}
    inline constexpr ll maxsize() const {return max_sz_;}
    inline constexpr bool empty() const {return sz_ == 0;}

    private:

    inline constexpr void check_and_adjust_idx(int& idx) const {
        if (idx >= sz_){cerr << "IndexError: index out of range. current size: " << sz_ << " target index: " << idx << endl; assert(false);}
        if (idx < 0){
            if (idx < -sz_){cerr << "IndexError: index out of range. current size: " << sz_ << " target index: " << idx << endl; assert(false);}
            idx += sz_;
        }
    }

    int sz_;
    int max_sz_;
    T* A;
};


#endif /* LIST__HPP_ */