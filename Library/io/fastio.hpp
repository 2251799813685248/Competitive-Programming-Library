#ifndef FASTIO__HPP_
#define FASTIO__HPP_

#include <array>
#include <bit>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

// C++20. Integer input must fit the destination type. For modint input, the
// magnitude must fit uint64_t; negative values are reduced modulo the modulus.
// FILE* is borrowed: it must outlive the reader/writer. Do not mix buffered
// readers on one stream. The default input buffer is intended for batch judges;
// interactive programs need BasicInput<1> and explicit output flush().
namespace fastio {
namespace detail {
template<class T> inline constexpr bool integer = std::is_integral_v<T>;
template<class T> struct unsigned_type { using type = std::make_unsigned_t<T>; };
template<> struct unsigned_type<bool> { using type = unsigned char; };
#if defined(__SIZEOF_INT128__)
template<> inline constexpr bool integer<__int128_t> = true;
template<> inline constexpr bool integer<__uint128_t> = true;
template<> struct unsigned_type<__int128_t> { using type = __uint128_t; };
template<> struct unsigned_type<__uint128_t> { using type = __uint128_t; };
#endif
template<class T> using unsigned_t = typename unsigned_type<T>::type;
template<class T> inline constexpr bool signed_integer = integer<T> && T(-1) < T(0);
template<class T> concept modular = requires(T x) { x.val; } &&
    (requires { T::mod_value; } || requires { T::mod(); });
template<class T> constexpr auto modulus() {
    if constexpr (requires { T::mod_value; }) return T::mod_value;
    else return T::mod();
}
template<class T> concept indexed_range = requires(T& x) { std::size(x); x[0]; };
template<class T> concept tuple_like = requires { std::tuple_size<T>::value; };
template<class> inline constexpr bool unsupported = false;
inline constexpr auto digit_pairs = [] {
    std::array<char, 200> result{};
    for (int i = 0; i < 100; ++i) {
        result[2*i] = char('0'+i/10);
        result[2*i+1] = char('0'+i%10);
    }
    return result;
}();
inline constexpr auto digit_quads = [] {
    std::array<std::array<char,4>,10000> result{};
    for (unsigned i = 0; i < 10000; ++i) {
        result[i] = {char('0'+i/1000),char('0'+i/100%10),
                     char('0'+i/10%10),char('0'+i%10)};
    }
    return result;
}();
inline char* leading_quad(char* p, unsigned value) {
    if (value >= 1000) { std::memcpy(p,digit_quads[value].data(),4); return p+4; }
    if (value >= 100) { *p++ = char('0'+value/100); value %= 100; }
    else if (value < 10) { *p++ = char('0'+value); return p; }
    std::memcpy(p,digit_pairs.data()+2*value,2);
    return p+2;
}
inline char* decimal32(char* p, std::uint32_t value) {
    if (value >= 100000000) {
        p = leading_quad(p,value/100000000);
        value %= 100000000;
        std::memcpy(p,digit_quads[value/10000].data(),4);
        std::memcpy(p+4,digit_quads[value%10000].data(),4);
        return p+8;
    }
    if (value >= 10000) {
        p = leading_quad(p,value/10000);
        std::memcpy(p,digit_quads[value%10000].data(),4);
        return p+4;
    }
    return leading_quad(p,value);
}
template<class T> inline constexpr bool output32 = [] {
    if constexpr (integer<T>) return sizeof(T) <= 4 && !std::is_same_v<T,char>;
    else if constexpr (modular<T>) return integer<decltype(T{}.val)> && sizeof(T{}.val) <= 4;
    else return false;
}();
template<class T> inline char* decimal_small(char* p, const T& value) {
    if constexpr (modular<T>) return decimal_small(p,value.val);
    else {
        std::uint32_t u = static_cast<std::uint32_t>(value);
        if constexpr (signed_integer<T>) {
            if (value < 0) { *p++ = '-'; u = 0-u; }
        }
        return decimal32(p,u);
    }
}
} // namespace detail

template<std::size_t BufferSize = 1 << 18>
class BasicInput {
    static_assert(BufferSize > 0);
    std::FILE* file_;
    char buffer_[BufferSize+1];
    std::size_t position_ = 0, size_ = 0;
    bool good_ = true;

    int peek() {
        if (position_ == size_) {
            size_ = std::fread(buffer_, 1, BufferSize, file_);
            buffer_[size_] = '\0';
            position_ = 0;
            if (size_ == 0) return EOF;
        }
        return static_cast<unsigned char>(buffer_[position_]);
    }
    int skip_space() {
        int c;
        while ((c = peek()) != EOF && c <= ' ') ++position_;
        return c;
    }
    template<class U, bool SingleBlock = false> bool magnitude(U& value, bool& negative) {
        int c = skip_space();
        if (c == EOF) return false;
        negative = c == '-';
        if (c == '-' || c == '+') { ++position_; c = peek(); }
        if (static_cast<unsigned>(c-'0') > 9) return false;
        value = 0;
        do {
            const char* p = buffer_+position_;
            if constexpr (sizeof(U) >= 4 && std::endian::native == std::endian::little) {
                while (buffer_+size_-p >= 8) {
                    std::uint64_t block;
                    std::memcpy(&block,p,8);
                    // All eight bytes must be ASCII decimal digits.
                    if (((block+0x4646464646464646ULL) | (block-0x3030303030303030ULL)) & 0x8080808080808080ULL) break;
                    block -= 0x3030303030303030ULL;
                    block = (block*10+(block>>8)) & 0x00ff00ff00ff00ffULL;
                    block = (block*100+(block>>16)) & 0x0000ffff0000ffffULL;
                    block = (block*10000+(block>>32)) & 0xffffffffULL;
                    value = value*100000000+static_cast<std::uint32_t>(block);
                    p += 8;
                    if constexpr (SingleBlock || sizeof(U) == 4) break;
                }
            }
            unsigned digit;
            // The sentinel stops this loop without an EOF/buffer check for
            // every digit. Only a token crossing a refill needs the outer loop.
            while ((digit = static_cast<unsigned>(static_cast<unsigned char>(*p)-'0')) <= 9) {
                value = value*10+digit;
                ++p;
            }
            position_ = static_cast<std::size_t>(p-buffer_);
            if (position_ != size_) break;
            c = peek();
        } while (static_cast<unsigned>(c-'0') <= 9);
        return true;
    }
    template<class T> bool read_one(T& value) {
        if constexpr (std::is_same_v<T, char>) {
            const int c = skip_space();
            if (c == EOF) return false;
            value = static_cast<char>(c);
            ++position_;
            return true;
        } else if constexpr (detail::integer<T>) {
            detail::unsigned_t<T> u;
            bool negative;
            if (!magnitude(u,negative)) return false;
            if constexpr (detail::signed_integer<T>) {
                // Never negate the minimum signed value, even for int128.
                value = negative && u != 0 ? -static_cast<T>(u-1)-1 : static_cast<T>(u);
            } else {
                if (negative || (std::is_same_v<T,bool> && u > 1)) return false;
                value = static_cast<T>(u);
            }
            return true;
        } else if constexpr (detail::modular<T>) {
            std::uint64_t u;
            bool negative;
            // Most modular coefficients have at most ten digits. After the
            // first eight, parse the short tail without another wide probe.
            if (!magnitude<std::uint64_t,true>(u,negative)) return false;
            const auto mod = detail::modulus<T>();
            if (u >= mod) u %= mod;
            value.val = negative && u != 0 ? mod-u : u;
            return true;
        } else if constexpr (std::is_same_v<T,std::string>) {
            int c = skip_space();
            if (c == EOF) return false;
            value.clear();
            do {
                value.push_back(static_cast<char>(c));
                ++position_;
                c = peek();
            } while (c != EOF && c > ' ');
            return true;
        } else if constexpr (std::is_floating_point_v<T>) {
            std::string token;
            if (!read_one(token)) return false;
            const char* first = token.data();
            const char* last = first+token.size();
            if (first != last && *first == '+') ++first;
            const auto result = std::from_chars(first,last,value);
            return result.ec == std::errc{} && result.ptr == last;
        } else if constexpr (detail::indexed_range<T>) {
            const auto n = std::size(value);
            if constexpr (requires { std::data(value); }) {
                auto* data = std::data(value);
                for (decltype(std::size(value)) i = 0; i < n; ++i)
                    if (!read_one(data[i])) return false;
            } else {
                for (decltype(std::size(value)) i = 0; i < n; ++i)
                    if (!read_one(value[i])) return false;
            }
            return true;
        } else if constexpr (detail::tuple_like<T>) {
            return std::apply([&](auto&... x) { return (read_one(x) && ...); },value);
        } else {
            static_assert(detail::unsupported<T>, "Unsupported fastio input type");
        }
    }
public:
    explicit BasicInput(std::FILE* file = stdin) : file_(file) {}
    BasicInput(const BasicInput&) = delete;
    BasicInput& operator=(const BasicInput&) = delete;

    template<class... T> bool read(T&... values) {
        if (!good_) return false;
        return good_ = (read_one(values) && ...);
    }
    template<class T> BasicInput& operator>>(T& value) { read(value); return *this; }
    explicit operator bool() const { return good_; }
};

template<std::size_t BufferSize = 1 << 18>
class BasicOutput {
    static_assert(BufferSize > 0);
    std::FILE* file_;
    char buffer_[BufferSize];
    std::size_t size_ = 0;
    bool good_ = true;

    bool drain() {
        if (!good_) return false;
        std::size_t done = 0;
        while (done < size_) {
            const auto n = std::fwrite(buffer_+done,1,size_-done,file_);
            if (n == 0) { size_ = 0; return good_ = false; }
            done += n;
        }
        size_ = 0;
        return true;
    }
    bool put(char c) {
        if (!good_ || (size_ == BufferSize && !drain())) return false;
        buffer_[size_++] = c;
        return true;
    }
    bool bytes(const char* data, std::size_t count) {
        if (!good_) return false;
        while (count != 0) {
            if (size_ == BufferSize && !drain()) return false;
            const auto room = BufferSize-size_;
            const auto n = count < room ? count : room;
            std::memcpy(buffer_+size_,data,n);
            size_ += n; data += n; count -= n;
        }
        return true;
    }
    template<class U> bool unsigned_number(U value, bool negative) {
        if constexpr (sizeof(U) <= 4 && BufferSize >= 12) {
            if (!good_ || (BufferSize-size_ < 12 && !drain())) return false;
            char* p = buffer_+size_;
            if (negative) *p++ = '-';
            p = detail::decimal32(p,value);
            size_ = static_cast<std::size_t>(p-buffer_);
            return true;
        }
        // 128-bit unsigned integers need at most 39 decimal digits.
        char digits[3*sizeof(U)+2];
        char* const end = digits+sizeof(digits);
        char* p = end;
        while (value >= 100) {
            const U q = value/100;
            const unsigned r = static_cast<unsigned>(value-q*100);
            p -= 2;
            std::memcpy(p,detail::digit_pairs.data()+2*r,2);
            value = q;
        }
        if (value < 10) *--p = char('0'+static_cast<unsigned>(value));
        else {
            p -= 2;
            std::memcpy(p,detail::digit_pairs.data()+2*static_cast<unsigned>(value),2);
        }
        if (negative) *--p = '-';
        if constexpr (BufferSize >= sizeof(digits)) {
            if (!good_ || (BufferSize-size_ < sizeof(digits) && !drain())) return false;
            auto count = static_cast<std::size_t>(end-p);
            char* dest = buffer_+size_;
            size_ += count;
            // Constant-size copies avoid a library call per integer. Each copy
            // stays inside the initialized digits and the reserved buffer area.
            if constexpr (sizeof(digits) >= 8)
                while (count >= 8) {
                    std::memcpy(dest,p,8);
                    dest += 8; p += 8; count -= 8;
                }
            if (count >= 4) { std::memcpy(dest,p,4); dest += 4; p += 4; count -= 4; }
            if (count >= 2) { std::memcpy(dest,p,2); dest += 2; p += 2; count -= 2; }
            if (count) *dest = *p;
            return true;
        }
        return bytes(p,static_cast<std::size_t>(end-p));
    }
    template<class T> bool write_one(const T& value) {
        if constexpr (std::is_same_v<T,char>) {
            return put(value);
        } else if constexpr (detail::integer<T>) {
            using U = detail::unsigned_t<T>;
            U u = static_cast<U>(value);
            bool negative = false;
            if constexpr (detail::signed_integer<T>) {
                negative = value < 0;
                if (negative) u = U(0)-u;
            }
            return unsigned_number(u,negative);
        } else if constexpr (detail::modular<T>) {
            return write_one(value.val);
        } else if constexpr (std::is_convertible_v<const T&,std::string_view>) {
            const std::string_view text(value);
            return bytes(text.data(),text.size());
        } else if constexpr (std::is_floating_point_v<T>) {
            char text[128];
            const auto result = std::to_chars(text,text+sizeof(text),value);
            if (result.ec != std::errc{}) return false;
            return bytes(text,static_cast<std::size_t>(result.ptr-text));
        } else if constexpr (detail::indexed_range<const T>) {
            const auto n = std::size(value);
            using Element = std::remove_cvref_t<decltype(value[0])>;
            if constexpr (detail::indexed_range<Element> || detail::tuple_like<Element>){
                for (decltype(std::size(value)) i = 0; i < n; ++i) {
                    if (i != 0 && !put('\n')) return false;
                    if (!write_one(value[i])) return false;
                }
                return true;
            }
            if constexpr (BufferSize >= 32 && detail::output32<Element> && requires { std::data(value); }) {
                const auto* data = std::data(value);
                char* p = buffer_+size_;
                for (decltype(std::size(value)) i = 0; i < n; ++i) {
                    if (buffer_+BufferSize-p < 12) {
                        size_ = static_cast<std::size_t>(p-buffer_);
                        if (!drain()) return false;
                        p = buffer_;
                    }
                    if (i != 0) *p++ = ' ';
                    p = detail::decimal_small(p,data[i]);
                }
                size_ = static_cast<std::size_t>(p-buffer_);
                return true;
            }
            for (decltype(std::size(value)) i = 0; i < n; ++i) {
                if (i != 0 && !put(' ')) return false;
                if (!write_one(value[i])) return false;
            }
            return true;
        } else if constexpr (detail::tuple_like<T>) {
            [[maybe_unused]] bool first = true;
            return std::apply([&](const auto&... x) {
                return (((std::exchange(first,false) || put(' ')) && write_one(x)) && ...);
            },value);
        } else {
            static_assert(detail::unsupported<T>, "Unsupported fastio output type");
        }
    }
public:
    explicit BasicOutput(std::FILE* file = stdout) : file_(file) {}
    BasicOutput(const BasicOutput&) = delete;
    BasicOutput& operator=(const BasicOutput&) = delete;
    ~BasicOutput() { flush(); }

    // write concatenates its arguments; print separates them with spaces and
    // appends a newline. Ranges and tuples use spaces between their elements.
    template<class... T> bool write(const T&... values) {
        if (!good_) return false;
        return good_ = (write_one(values) && ...);
    }
    template<class... T> bool print(const T&... values) {
        if (!good_) return false;
        [[maybe_unused]] bool first = true;
        good_ = (((std::exchange(first,false) || put(' ')) && write_one(values)) && ...);
        return good_ && put('\n');
    }
    template<class... T> bool printflush(const T&... values) {
        if (!good_) return false;
        [[maybe_unused]] bool first = true;
        good_ = (((std::exchange(first,false) || put(' ')) && write_one(values)) && ...);
        return good_ && put('\n') && flush();
    }
    bool flush() { return drain() && (good_ = std::fflush(file_) == 0); }
    template<class T> BasicOutput& operator<<(const T& value) { write(value); return *this; }
    explicit operator bool() const { return good_; }
};


#ifdef LOCAL_a1073741824
using FastInput = BasicInput<1>;
#else
using FastInput = BasicInput<>;
#endif
using FastOutput = BasicOutput<>;

// template<class... T> bool read(T&... values) { return in.read(values...); }
// template<class... T> bool write(const T&... values) { return out.write(values...); }
// template<class... T> bool print(const T&... values) { return out.print(values...); }
// template<class... T> bool printflush(const T&... values) { return out.print(values...) && out.flush(); }
// inline bool flush() { return out.flush(); }

} // namespace fastio

#endif /* FASTIO__HPP_ */
