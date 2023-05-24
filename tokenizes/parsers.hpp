#pragma once
#include <bitset>
#include <compare>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace tokenizes {

struct escaped_char {
    char c;
    escaped_char(char _c) : c(_c) {}
};

std::ostream &operator<<(std::ostream &os, const escaped_char &ec);

struct empty_t {
    empty_t() = default;
    empty_t(const empty_t &) = default;
    empty_t(empty_t &&) = default;
};

template <class R, class L> struct base {
    using right_t = R;
    using left_t = L;
    virtual bool operator()(std::stringstream &, right_t &, left_t &) const = 0;
};

struct atom : public base<char, empty_t>, public base<std::string, empty_t> {
    using chars_t = std::bitset<256>;
    chars_t chars;

public:
    constexpr atom() = default;
    constexpr atom(const chars_t &_chars) : chars(_chars) {}
    constexpr atom(const atom &) = default;
    constexpr const chars_t &get_chars() const { return chars; }
    constexpr chars_t &get_chars() { return chars; }

    virtual bool operator()(std::stringstream &, char &, empty_t &) const;
    virtual bool operator()(std::stringstream &, std::string &, empty_t &) const;

    static std::shared_ptr<atom> from(const chars_t &chars);
    static std::shared_ptr<atom> from(uint8_t c);
    static std::shared_ptr<atom> from(std::string_view sv);
    static std::shared_ptr<atom> from_range(uint8_t first, uint8_t last);
};
using atom_ptr = std::shared_ptr<atom>;

std::ostream &operator<<(std::ostream &, const atom &);
atom_ptr operator|(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator|=(atom_ptr &a, const atom_ptr &b);
atom_ptr operator&(const atom_ptr &a, const atom_ptr &b);
atom_ptr &operator&=(atom_ptr &a, const atom_ptr &b);
atom_ptr operator^(const atom_ptr &a, const atom_ptr &b);
atom_ptr &operator^=(atom_ptr &a, const atom_ptr &b);
atom_ptr operator~(const atom_ptr &a);

const static inline atom_ptr sign = atom::from("+-");
const static inline atom_ptr dot = atom::from(".");
const static inline atom_ptr small = atom::from_range('a', 'z');
const static inline atom_ptr large = atom::from_range('A', 'Z');
const static inline atom_ptr alpha = small | large;
const static inline atom_ptr digit = atom::from_range('0', '9');
const static inline atom_ptr alnum = small | large | digit;

template <class L = empty_t> struct repeat_string : public base<std::string, L> {
    const size_t min, max;
    std::shared_ptr<base<std::string, L>> parser;

public:
    repeat_string(std::shared_ptr<base<std::string, L>> _parser, size_t _min = 0, size_t _max = SIZE_MAX)
        : parser(_parser), min(_min), max(_max) {}
    virtual bool operator()(std::stringstream &ss, std::string &r, L &l) const {
        size_t count = 0;

        for (; count < min; count++) {
            if (!(*parser)(ss, r, l)) {
                return false;
            }
        }
        for (; count < max; count++) {
            if (!(*parser)(ss, r, l)) {
                return true;
            }
        }
        return true;
    }

    static std::shared_ptr<repeat_string> make(std::shared_ptr<base<std::string, L>> parser, size_t min = 0,
                                               size_t max = SIZE_MAX) {
        return std::shared_ptr<repeat_string>(parser, min, max);
    }
};

} // namespace tokenizes
