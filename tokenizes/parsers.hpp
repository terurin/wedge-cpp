#pragma once
#include <bitset>
#include <cassert>
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

template <class R, class L = empty_t> struct base : public std::enable_shared_from_this<base<R, L>> {
    using right_t = R;
    using left_t = L;
    virtual bool operator()(std::stringstream &, right_t &, left_t &) const = 0;
    operator std::shared_ptr<base>() { return this->shared_from_this(); }
    operator std::shared_ptr<const base>() const { return this->shared_from_this(); }
};

template <class R> struct base<R, empty_t> : public std::enable_shared_from_this<base<R, empty_t>> {
    using right_t = R;
    using left_t = empty_t;
    virtual bool operator()(std::stringstream &, right_t &, left_t &) const = 0;
    virtual bool operator()(std::stringstream &ss, right_t &r) const {
        empty_t empty;
        return (*this)(ss, r, empty);
    }
    operator std::shared_ptr<base>() { return this->shared_from_this(); }
    operator std::shared_ptr<const base>() const { return this->shared_from_this(); }
};

template <class R, class L = empty_t> using base_ptr = std::shared_ptr<const base<R, L>>;
template <class R, class L = empty_t> using base_mut_ptr = std::shared_ptr<base<R, L>>;

struct atom : public base<std::string, empty_t> {
    using chars_t = std::bitset<256>;
    chars_t chars;

public:
    constexpr atom() = default;
    constexpr atom(const chars_t &_chars) : chars(_chars) {}
    atom(const atom &) = default;
    atom(atom &&) = default;
    constexpr chars_t &get_chars() { return chars; }
    constexpr const chars_t &get_chars() const { return chars; }

    virtual bool operator()(std::stringstream &, std::string &, empty_t &) const;

    static std::shared_ptr<atom> from(const chars_t &chars);
    static std::shared_ptr<atom> from(uint8_t c);
    static std::shared_ptr<atom> from(std::string_view sv);
    static std::shared_ptr<atom> from_range(uint8_t first, uint8_t last);
};

using atom_ptr = std::shared_ptr<const atom>;
using atom_mut_ptr = std::shared_ptr<atom>;

std::ostream &operator<<(std::ostream &, const atom &);
atom_ptr operator|(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator&(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator^(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator~(const atom_ptr &a);

atom_mut_ptr &operator|=(atom_mut_ptr &a, const atom_ptr &b);
atom_mut_ptr &operator&=(atom_mut_ptr &a, const atom_ptr &b);
atom_mut_ptr &operator^=(atom_mut_ptr &a, const atom_ptr &b);

static inline atom_ptr sign = atom::from("+-");
static inline atom_ptr dot = atom::from(".");
static inline atom_ptr small = atom::from_range('a', 'z');
static inline atom_ptr large = atom::from_range('A', 'Z');
static inline atom_ptr alpha = small | large;
static inline atom_ptr digit = atom::from_range('0', '9');
static inline atom_ptr alnum = small | large | digit;

template <class R = std::string, class L = empty_t> struct repeat : public base<R, L> {
    const size_t min, max;
    base_ptr<std::string, L> parser;

public:
    repeat(base_ptr<R, L> _parser, size_t _min = 0, size_t _max = SIZE_MAX) : parser(_parser), min(_min), max(_max) {
        assert(_min <= _max);
    }
    virtual bool operator()(std::stringstream &ss, R &r, L &l) const {
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

    static base_ptr<R, L> create(base_ptr<R, L> p, size_t min = 0, size_t max = SIZE_MAX) {
        return std::make_shared<repeat<R, L>>(p, min, max);
    }
};

template <class R, class L> base_ptr<std::string, L> repeat_n_m(base_ptr<std::string, L> p, size_t n, size_t m) {
    return repeat<L>::create(p, n, m);
}

static inline base_ptr<std::string, empty_t> repeat_n_m(base_ptr<std::string, empty_t> p, size_t n, size_t m) {
    return repeat<std::string, empty_t>::create(p, n, m);
}

template <class R, class L> base_ptr<std::string, L> repeat_n(base_ptr<std::string, L> p, size_t n) {
    return repeat<L>::create(p, n, n);
}

static inline base_ptr<std::string, empty_t> repeat_n(base_ptr<std::string, empty_t> p, size_t n) {
    return repeat<std::string, empty_t>::create(p, n, n);
}

template <class R, class L> base_ptr<std::string, L> many1(base_ptr<std::string, L> p) {
    return repeat<L>::create(p, 1);
}

static inline base_ptr<std::string, empty_t> many1(base_ptr<std::string, empty_t> p) {
    return repeat<std::string, empty_t>::create(p, 1);
}

template <class R, class L> base_ptr<std::string, L> many0(base_ptr<std::string, L> p, size_t n) {
    return repeat<L>::create(p);
}

static inline base_ptr<std::string, empty_t> many0(base_ptr<std::string, empty_t> p, size_t n) {
    return repeat<std::string, empty_t>::create(p);
}

} // namespace tokenizes
