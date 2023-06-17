#pragma once
#include <bitset>
#include <cassert>
#include <compare>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "concepts"
#include "either.hpp"
#include "mappers.hpp"
#include "primitive.hpp"
#include "repeats.hpp"

namespace tokenizes {

using tokenizes::concepts::left_of;
using tokenizes::concepts::parsable;
using tokenizes::concepts::right_of;
using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;

template <class R, class L>
class shell {
public:
    using right_t = R;
    using left_t = L;
    using parser_t = std::function<either<R, L>(std::istream &)>;

private:
    parser_t parser;

public:
    shell(const parser_t &_parser) : parser(_parser) {}
    shell(parser_t &&_parser) : parser(_parser) {}
    either<R, L> operator()(std::istream &is) const { return parser(is); }

    // map_*
    template <class F>
    auto map_right(F &&func) const {
        return shell(mappers::mapper_right(*this, std::move(func)));
    }

    template <class F>
    auto map_left(F &&func) const {
        return shell(mappers::mapper_left(*this, std::move(func)));
    }

    // const_*
    template <class V>
    auto const_right(V &&v) const {
        return shell(mappers::constant_right(*this, std::move(v)));
    }

    template <class V>
    auto const_left(V &&v) const {
        return shell(mappers::constant_left(*this, std::move(v)));
    }

    // repeat, many
    auto repeat(size_t n, size_t m) const { return shell(repeats::repeat(*this, n, m)); }
    auto many0() const { return shell(repeats::many0(*this)); }
    auto many1() const { return shell(repeats::many1(*this)); }

    // erase_*
    auto erase_right() const { return shell(eraser_right(*this)); }
    auto erase_left() const { return shell(eraser_left(*this)); }
    auto erase_both() const { return shell(eraser_both(*this)); }
};

template <parsable P>
shell(P) -> shell<right_of<P>, left_of<P>>;

using shell_char = shell<char, nullptr_t>;

// atom //
template <class T>
    requires std::constructible_from<primitive::atom, T>
shell_char atom(T c) {
    return shell(primitive::atom(c));
}
static inline shell_char ranged_atom(unsigned char first, unsigned char last) {
    return shell(primitive::atom::from_range(first, last));
}

const static inline shell_char sign = shell(primitive::sign);
const static inline shell_char dot = shell(primitive::dot);
const static inline shell_char space = shell(primitive::space);
const static inline shell_char digit = shell(primitive::digit);
const static inline shell_char small = shell(primitive::small);
const static inline shell_char large = shell(primitive::large);
const static inline shell_char alpha = shell(primitive::alpha);
const static inline shell_char alnum = shell(primitive::alnum);
const static inline shell_char hexdigit = shell(primitive::hexdigit);

// tag //
static inline shell<std::string, nullptr_t> tag(std::string_view sv) { return shell(primitive::tag(sv)); }

// tag_list
static inline shell<std::string, nullptr_t> tag_list(std::vector<std::string> &items) {
    return shell(primitive::tag_list(items));
}
static inline shell<std::string, nullptr_t> tag_list(std::initializer_list<std::string_view> items) {
    return shell(primitive::tag_list(items));
}

} // namespace tokenizes
