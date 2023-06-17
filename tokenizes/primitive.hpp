#pragma once

#include "either.hpp"
#include <bitset>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace tokenizes::primitive {

using tokenizes::eithers::either;

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

class tag_list {
    std::unordered_map<std::string, bool> table;

public:
    tag_list(const std::vector<std::string> &list);
    tag_list(std::initializer_list<std::string_view> list);

    either<std::string, nullptr_t> operator()(std::istream &) const;

    const std::unordered_map<std::string, bool> &get_table() const { return table; }
};

std::ostream &operator<<(std::ostream &, const tag_list &);

} // namespace tokenizes::primitive