#ifndef LIS_HPP_
#define LIS_HPP_

#include <vector>
#include <algorithm>
using namespace std;



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




#endif /* LIS_HPP_ */