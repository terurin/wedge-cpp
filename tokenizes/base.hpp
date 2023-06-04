#pragma once

#include <memory>

namespace tokenizes {

struct empty_t {
    empty_t() = default;
    empty_t(const empty_t &) = default;
    empty_t(empty_t &&) = default;
};

template <class O>
struct base : public std::enable_shared_from_this<base<O>> {
    using output = O;
    virtual bool operator()(std::stringstream &ss, output &r) const = 0;
    operator std::shared_ptr<base>() { return this->shared_from_this(); }
    operator std::shared_ptr<const base>() const { return this->shared_from_this(); }

};

template <class O>
using base_ptr = std::shared_ptr<const base<O>>;

template <class O>
using base_mut_ptr = std::shared_ptr<base<O>>;

}; // namespace tokenizes