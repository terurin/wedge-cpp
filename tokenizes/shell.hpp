#pragma once
#include "base.hpp"
#include "parsers.hpp"

namespace tokenizes {

template <class O>
class shell {
    using output = O;
    std::shared_ptr<base<O>> inner;

public:
    shell(std::shared_ptr<base<O>> &&_inner) : inner(_inner) {}
    virtual bool operator()(std::stringstream &ss, output &r) { return (*inner)(ss, r); }
    auto repeat_n_m(size_t n, size_t m) const { return repeat<O>::create(this->shared_from_this(), n, m); }
    auto repeat_n(size_t n) const { return repeat<O>::create(this->shared_from_this(), n, n); }
    auto many1() const { return repeat<O>::create(this->shared_from_this(), 1); }
    auto many0() const { return repeat<O>::create(this->shared_from_this(), 0); }

    /* mapper */
    template <std::invocable<O> F>
    auto map(F func) const {
        O r;
        return mapper<typename std::invoke_result_t<F, O>, O>::create(inner, func);
    }

    // choose
    virtual std::shared_ptr<choose<O>> or_parser(base_ptr<O> &&insert) { return choose<O>::create({inner, insert}); }
};

template <class B>
auto make_shell(std::shared_ptr<B> &&b) {
    return shell<typename B::output>(std::move(b));
}

}; // namespace tokenizes
