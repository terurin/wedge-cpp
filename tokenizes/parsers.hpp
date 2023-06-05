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

#include "primitive.hpp"

namespace tokenizes {

template <class O>
class shell {
public:
    using output = O;
    using func_t = std::function<std::optional<O>(std::istream &)>;

private:
    func_t func;

public:
    shell(const func_t &_func) : func(_func) {}
    shell(func_t &&_func) : func(_func) {}
    std::optional<O> operator()(std::istream &is) const { return func(is); }
};

template <class F>
    requires std::invocable<F, std::istream &>
shell(const F &) -> shell<typename std::invoke_result_t<F, std::istream &>::value_type>;

template <class F>
    requires std::invocable<F, std::istream &>
shell(F &&) -> shell<typename std::invoke_result_t<F, std::istream &>::value_type>;

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
static inline shell<std::string> tag(std::string_view sv){
    return shell(primitive::tag(sv));
}


} // namespace tokenizes
