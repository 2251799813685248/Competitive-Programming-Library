#ifndef DEQUE__HPP_
#define DEQUE__HPP_

#include <deque>
#include <iostream>
using ll = long long;
using uint = unsigned int;



template <typename T>
class deque2 {
public:
    inline constexpr deque2() {
        A = std::deque<T>();
    }
    inline constexpr deque2(uint sz, const T& val = T()) {
        A = std::deque<T>(sz, val);
    }
    template <class InputIterator>
    inline constexpr deque2(InputIterator first, InputIterator last) {
        A = std::deque<T>(first, last);
    }

    inline constexpr void append(const T& val) {
        A.push_back(val);
    }
    inline constexpr void appendleft(const T& val) {
        A.push_front(val);
    }
    inline constexpr T pop() {
        if (A.empty()) {
            std::cerr << "IndexError: pop from empty list" << std::endl;
        }
        auto temp = std::move(A.back());
        A.pop_back();
        return std::move(temp);
    }
    inline constexpr T popleft() {
        if (A.empty()) {
            std::cerr << "IndexError: pop from empty list" << std::endl;
        }
        auto temp = std::move(A.front());
        A.pop_front();
        return std::move(temp);
    }
    inline constexpr T& front() {
        return A.front();
    }
    inline constexpr const T& front() const {
        return A.front();
    }
    inline constexpr T& back() {
        return A.back();
    }
    inline constexpr const T& back() const {
        return A.back();
    }

    inline constexpr std::_Deque_iterator<T, T&, T*> begin() {
        return A.begin();
    }
    inline constexpr std::_Deque_iterator<T, T&, T*> begin() const {
        return A.begin();
    }
    inline constexpr std::_Deque_iterator<T, T&, T*> end() {
        return A.end();
    }
    inline constexpr std::_Deque_iterator<T, T&, T*> end() const {
        return A.end();
    }

    inline constexpr ll size() {
        return A.size();
    }
    inline constexpr bool empty() {
        return A.empty();
    }

private:
    std::deque<T> A;
};

#endif /* DEQUE__HPP_ */