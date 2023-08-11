#pragma once

#include "either.hpp"
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
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

template <class T>
class tag_mapper {

public:
    struct node_t {
        std::optional<T> value{std::nullopt};
        std::array<std::unique_ptr<node_t>, 256> table;
        node_t() { std::ranges::fill(table, nullptr); }
    };
    using node_ptr = std::unique_ptr<node_t>;
    node_ptr root;

private:
    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
    constexpr static node_ptr build_root(R &&);
    constexpr static void build_node(node_t &, std::string_view, const T &);
    constexpr static std::optional<T> walk_node(const node_t &, std::istream &);

public:
    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
    constexpr tag_mapper(R &&r) : root(build_root(r)) {}
    constexpr tag_mapper(std::initializer_list<std::tuple<std::string_view, T>> &&r) : root(build_root(r)) {}
    constexpr tag_mapper(const tag_mapper &) = delete;
    constexpr either<T, std::nullptr_t> operator()(std::istream &is) const {
        if (const std::optional<T> opt = walk_node(*root, is); opt) {
            return right(*opt);
        }
        return left(nullptr);
    }
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
    either<T, unsigned_errors> operator()(std::istream &is) const;
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
    either<T, signed_errors> operator()(std::istream &is) const;
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
    either<T, integer_errors> operator()(std::istream &is) const;
};

enum class string_errors { not_begin, not_end, bad_escape };

class string_parser {
    tag quote;

public:
    string_parser(std::string_view _quote = "'") : quote(_quote) {}
    either<std::string, string_errors> operator()(std::istream &is) const;
};

enum class raw_string_errors { not_begin, not_end };

class raw_string_parser {
    tag quote;

public:
    raw_string_parser(std::string_view _quote = "\"\"\"") : quote(_quote) {}
    either<std::string, raw_string_errors> operator()(std::istream &is) const;
};

} // namespace tokenizes::primitive

#include "primitive.cxx"
