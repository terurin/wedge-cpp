#pragma once

#include "either.hpp"
#include <bitset>
#include <cstddef>
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

// [0-(base-1)]+
template <std::integral T = unsigned int>
class number {
    unsigned int base;

    inline std::optional<T> digit(int d) {
        if (base <= 10) {
            if ('0' <= d && d < '0' + base) {
                return d - '0';
            }
            return std::nullopt;

        } else {
            if ('0' <= d && d < '0' + 10) {
                return d - '0';
            }
            if ('a' <= d && d < 'a' + base - 10) {
                return d - 'a' + 0xa;
            }
            if ('A' <= d && d < 'A' + base - 10) {
                return d - 'A' + 0xA;
            }
            return std::nullopt;
        }
    }

public:
    number(unsigned int _base = 10) : base(_base) {}
    either<T, std::nullptr_t> operator()(std::istream &is) const {
        T result = 0;
        // first
        if (const auto d = digit(is.peek()); d) {
            result = *d;
            is.ignore();
        } else {
            return left<std::nullptr_t>(nullptr);
        }

        // lasts
        for (auto d = digit(is.peek()); d; d = digit(is.peek())) {
            result = result * base + *d;
            is.ignore();
        }

        return right<T>(result);
    }
};

} // namespace tokenizes::primitive