#ifndef RANGE__HPP_
#define RANGE__HPP_

#include <algorithm>
#include <iostream>
#include <cassert>
#include <functional>
using uint = unsigned int;
using ll = long long;
using namespace std;


class range{
    public:

    range(int begin, int end, int step = 1): begin_(begin), end_(end), step_(step){}
    range(int end){range(0, end, 1);}

    int begin_;
    int end_;
    int step_;
};


#endif /* RANGE__HPP_ */