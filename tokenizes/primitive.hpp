#pragma once

#include "either.hpp"
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace tokenizes::primitive {

using tokenizes::eithers::either;
using tokenizes::eithers::left;
using tokenizes::eithers::right;

class atom {
    using chars_t = std::bitset<256>;
    chars_t chars;

public:
    atom(const chars_t &_chars) : chars(_chars) {}
    atom(std::string_view sv) {
        for (auto c : sv) {
            chars.set((unsigned)c);
        }
    }
    atom(unsigned char _char) { chars.set(_char); }

    atom(const atom &) = default;
    atom(atom &&) = default;
    virtual ~atom() = default;

    either<char, std::nullptr_t> operator()(std::istream &ss) const;

    const chars_t &get_chars() const { return chars; }

    atom operator+(const atom &x) const { return atom(chars | x.chars); }
    atom operator-(const atom &x) const { return atom(chars & ~x.chars); }
    atom operator-() const { return atom(~chars); }

    static inline atom from_range(unsigned char first, unsigned last) {
        chars_t chars;
        for (unsigned int c = first; c <= last; c++) {
            chars.set(c);
        }
        return atom(chars);
    }
};

std::ostream &operator<<(std::ostream &, const atom &);

static inline atom sign("+-");
static inline atom dot(".");
static inline atom space(" \t\r\n");
static inline atom small = atom::from_range('a', 'z');
static inline atom large = atom::from_range('A', 'Z');
static inline atom alpha = small + large;
static inline atom digit = atom::from_range('0', '9');
static inline atom alnum = small + large + digit;
static inline atom hexdigit = digit + atom::from_range('a', 'f') + atom::from_range('A', 'F');

class tag {
    std::string str;

public:
    tag(std::string_view sv) : str(sv) {}
    tag &set(std::string_view sv) { return str = sv, *this; }
    either<std::string, std::nullptr_t> operator()(std::istream &ss) const;
    const std::string &get_str() const { return str; }
};

std::ostream &operator<<(std::ostream &, const tag &);

class tag_list_builder;
class tag_list {
    std::unordered_map<std::string, bool> table;
    size_t buffer_size;

public:
    tag_list(const std::vector<std::string> &list);
    tag_list(std::initializer_list<std::string_view> list);
    either<std::string, nullptr_t> operator()(std::istream &) const;
    const std::unordered_map<std::string, bool> &get_table() const { return table; }
    static tag_list_builder builder();
};

std::ostream &operator<<(std::ostream &, const tag_list &);

class tag_list_builder {
    std::vector<std::string> items;

public:
    tag_list_builder() = default;
    tag_list_builder &add(std::string_view item) { return items.emplace_back(item), *this; }
    tag_list build() const { return tag_list(items); }
};

class digit_parser {
    unsigned int base;

public:
    digit_parser(unsigned int _base) : base(_base) {}
    either<int, std::nullptr_t> operator()(std::istream &) const;
    unsigned int get_base() const { return base; }
};

std::ostream &operator<<(std::ostream &, const digit_parser &);

enum class unsigned_errors { overflow, not_digit };

// [0-(base-1)]+
template <std::unsigned_integral T = unsigned int>
class unsigned_parser {
    const digit_parser digit;

public:
    unsigned_parser(unsigned int _base = 10) : digit(_base) {}
    either<T, unsigned_errors> operator()(std::istream &is) const {
        using namespace std;
        const std::streampos pos = is.tellg();
        T result = 0;

        // first
        if (const either<int, nullptr_t> e = digit(is); e.is_right()) {
            result = e.get_right();
        } else {
            return left(unsigned_errors::not_digit);
        }

        // lasts
        const unsigned int base = get_base();
        for (either<int, nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
            const T limit = std::numeric_limits<T>::max() - result;
            const int d = e.get_right();

            // shift
            if (result * (base - 1) + d > limit) {
                is.seekg(pos);
                return left(unsigned_errors::overflow);
            }
            result = result * base + d;
        }
        return right(result);
    }
    unsigned int get_base() const { return digit.get_base(); }
};

template <std::unsigned_integral T>
static inline std::ostream &operator<<(std::ostream &os, const unsigned_parser<T> &d) {
    return os << "unsigned(" << d.get_base() << ")";
}

enum class signed_errors { not_digit, overflow, underflow };

// [-+]?[0-(base-1)]+
template <std::signed_integral T = signed int>
class signed_parser {
    digit_parser digit;

public:
    signed_parser(unsigned int _base = 10) : digit(_base) {}
    either<T, signed_errors> operator()(std::istream &is) const {
        const std::streampos pos = is.tellg();

        // [-+]?
        bool sign = false;
        if (const int s = is.peek(); s == '+' || s == '-') {
            sign = s == '-';
            is.ignore();
        }

        //[0-(base-1)]
        T result = 0;
        if (const either<int, std::nullptr_t> e = digit(is); e.is_right()) {
            result = sign ? -e.get_right() : e.get_right();
        } else {
            return left(signed_errors::not_digit);
        }

        //[0-(base-1)]*
        const unsigned int base = digit.get_base();
        for (either<int, std::nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
            const int d = e.get_right();
            if (sign) {
                const T limit = std::numeric_limits<T>::min() - result;
                if (result * (base - 1) - d < limit) {
                    is.seekg(pos);
                    return left(signed_errors::underflow);
                }
                result = result * base - d;

            } else {
                const T limit = std::numeric_limits<T>::max() - result;
                if (result * (base - 1) + d > limit) {
                    is.seekg(pos);
                    return left(signed_errors::overflow);
                }
                result = result * base + d;
            }
        }

        return right(result);
    }
    unsigned int get_base() const { return digit.get_base(); }
};

template <std::signed_integral T>
static inline std::ostream &operator<<(std::ostream &os, const signed_parser<T> &d) {
    return os << "signed(" << d.get_base() << ")";
}

enum class integer_errors {
    not_digit,
    overflow,
    underflow,
};

template <std::signed_integral T = int>
class integer_parser {
public:
    integer_parser() = default;
    either<T, integer_errors> operator()(std::istream &is) const {
        const std::streampos pos = is.tellg();
        // [+-]?
        bool sign = false;
        if (const int s = is.peek(); s == '+' || s == '-') {
            sign = s == '-';
            is.ignore();
        }

        // attempt {0b,0q,0o,0d,0x}?
        const int base = [](std::istream &is) -> int {
            const std::streampos pos = is.tellg();
            if (is.peek() != '0') {
                return 10;
            }
            is.ignore();
            switch (is.get()) {
            case 'b':
                return 2;
            case 'q':
                return 4;
            case 'o':
                return 8;
            case 'd':
                return 10;
            case 'x':
                return 16;
            default:
                is.seekg(pos);
                return 10;
            }
        }(is);
        const digit_parser digit(base);

        // firts
        T result = 0;

        if (const either<int, std::nullptr_t> e = digit(is); e.is_right()) {
            result = sign ? -e.get_right() : e.get_right();
        } else {
            return left(integer_errors::not_digit);
        }

        //[0-(base-1)]*
        for (either<int, std::nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
            const int d = e.get_right();
            if (sign) {
                const T limit = std::numeric_limits<T>::min() - result;
                if (result * (base - 1) - d < limit) {
                    is.seekg(pos);
                    return left(integer_errors::underflow);
                }
                result = result * base - d;

            } else {
                const T limit = std::numeric_limits<T>::max() - result;
                if (result * (base - 1) + d > limit) {
                    is.seekg(pos);
                    return left(integer_errors::overflow);
                }
                result = result * base + d;
            }
        }

        return right(result);
    }
};

} // namespace tokenizes::primitive