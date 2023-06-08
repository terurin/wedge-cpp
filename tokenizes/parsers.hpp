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

#include "mappers.hpp"
#include "primitive.hpp"
#include "repeats.hpp"

namespace tokenizes {

template <class T>
class shell {
public:
    using output = T;
    using parser_t = std::function<std::optional<T>(std::istream &)>;

private:
    parser_t parser;

public:
    shell(const parser_t &_parser) : parser(_parser) {}
    shell(parser_t &&_parser) : parser(_parser) {}
    std::optional<T> operator()(std::istream &is) const { return parser(is); }

    template <class F>
    auto map(const F &func) const {
        using T2 = std::invoke_result_t<F, T>;
        return shell(mappers::mapper<T, T2>(*this, func));
    }
    template <class F>
    auto map(F &&func) const {
        using T2 = std::invoke_result_t<F, T>;
        return shell(mappers::mapper<T, T2>(std::move(*this), std::move(func)));
    }

    template <class F>
    auto opt_map(const F &func) const {
        using T2 = std::invoke_result_t<F, T>::value_type;
        return shell(mappers::opt_mapper<T, T2>(*this, func));
    }

    template <class F>
    auto opt_map(F &&func) const {
        using T2 = std::invoke_result_t<F, T>::value_type;
        return shell(mappers::opt_mapper<T, T2>(std::move(*this), std::move(func)));
    }

    template <class V>
    auto to_value(const V &v) const{
        return shell(mappers::to_value<T, V>(*this, v));
    }

    template <class V>
    auto to_value(V &&v) {
        return shell(mappers::to_value<T, V>(std::move(*this), std::move(v)));
    }
};

template <class F>
    requires std::invocable<F, std::istream &>
shell(F) -> shell<typename std::invoke_result_t<F, std::istream &>::value_type>;

// atom //
template <class T>
    requires std::constructible_from<primitive::atom, T>
shell<char> atom(T c) {
    return shell(primitive::atom(c));
}
static inline shell<char> ranged_atom(unsigned char first, unsigned char last) {
    return shell(primitive::atom::from_range(first, last));
}

const static inline shell<char> sign = shell(primitive::sign);
const static inline shell<char> dot = shell(primitive::dot);
const static inline shell<char> space = shell(primitive::space);
const static inline shell<char> digit = shell(primitive::digit);
const static inline shell<char> small = shell(primitive::small);
const static inline shell<char> large = shell(primitive::large);
const static inline shell<char> alpha = shell(primitive::alpha);
const static inline shell<char> alnum = shell(primitive::alnum);
const static inline shell<char> hexdigit = shell(primitive::hexdigit);

// tag //
static inline shell<std::string> tag(std::string_view sv) { return shell(primitive::tag(sv)); }

} // namespace tokenizes
