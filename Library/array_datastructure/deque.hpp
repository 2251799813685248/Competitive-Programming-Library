#ifndef DEQUE__HPP_
#define DEQUE__HPP_

#include <deque>
#include <iostream>
#include <iterator>
using namespace std;
using ll = long long;
using uint = unsigned int;

template<typename T>
class delist{
    public:

    inline constexpr delist(){A = deque<T>();}
    inline constexpr delist(uint sz, const T& val = T()){A = deque<T>(sz, val);}
    template <class InputIterator> inline constexpr delist(InputIterator first, InputIterator last){A = deque<T>(first, last);}

    inline constexpr void append(const T& val){A.push_back(val);}
    inline constexpr void appendleft(const T& val){A.push_front(val);}
    inline constexpr T pop(){
        if (A.empty()){cerr << "IndexError: pop from empty list" << endl;}
        auto temp = std::move(A.back());
        A.pop_back();
        return std::move(temp);
    }
    inline constexpr T popleft(){
        if (A.empty()){cerr << "IndexError: pop from empty list" << endl;}
        auto temp = std::move(A.front());
        A.pop_front();
        return std::move(temp);
    }
    inline constexpr T& front(){return A.front();}
    inline constexpr const T& front() const {return A.front();}
    inline constexpr T& back(){return A.back();}
    inline constexpr const T& back() const {return A.back();}

    inline constexpr _Deque_iterator<T, T&, T*> begin(){return A.begin();}
    inline constexpr _Deque_iterator<T, T&, T*> begin() const {return A.begin();}
    inline constexpr _Deque_iterator<T, T&, T*> end(){return A.end();}
    inline constexpr _Deque_iterator<T, T&, T*> end() const {return A.end();}


    inline constexpr ll size(){return A.size();}
    inline constexpr bool empty(){return A.empty();}

    private:


    deque<T> A;
};

#endif /* DEQUE__HPP_ */