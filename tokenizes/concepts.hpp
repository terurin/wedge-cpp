#pragma once
#include "either.hpp"
#include <concepts>
#include <istream>
namespace tokenizes::concepts {
template <typename P>
concept parsable = std::invocable<P, std::istream &> && std::move_constructible<P> && std::copy_constructible<P> &&
                   requires(const P &p, std::istream &is) {
                       typename decltype(p(is))::right_t;
                       typename decltype(p(is))::left_t;
                   };

template <parsable P>
using either_of = typename std::invoke_result_t<P, std::istream &>;

template <parsable P>
using right_of = typename std::invoke_result_t<P, std::istream &>::right_t;
template <parsable P>
using left_of = typename std::invoke_result_t<P, std::istream &>::left_t;

template <class C, class I>
concept has_push_back = requires(C &c, const I &item) { c.push_back(item); };

} // namespace tokenizes::concepts