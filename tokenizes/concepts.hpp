#pragma once
#include "either.hpp"
#include <concepts>
namespace tokenizes::concepts {
template <typename P>
concept parsable = std::invocable<P, std::istream &> && std::move_constructible<P> && std::copy_constructible<P> &&
                   requires(const P &p, std::istream &is) {
                       typename decltype(p(is))::right_t;
                       typename decltype(p(is))::left_t;
                   };

template <parsable P>
using right_of = std::invoke_result_t<P, std::istream &>::right_t;
template <parsable P>
using left_of = std::invoke_result_t<P, std::istream &>::left_t;
}