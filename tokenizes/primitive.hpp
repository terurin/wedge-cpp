#pragma once

#include <bitset>
#include <iostream>
#include <optional>
#include <string_view>

#include "either.hpp"
namespace tokenizes::primitive {

using tokenizes::eithers::either;

struct escaped_char {
    char c;
    escaped_char(char _c) : c(_c) {}
};

std::ostream &operator<<(std::ostream &os, const escaped_char &ec);

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

    either<char, std::nullopt_t> operator()(std::istream &ss) const;

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
    either<std::string, std::nullopt_t> operator()(std::istream &ss) const;
};

} // namespace tokenizes::primitive