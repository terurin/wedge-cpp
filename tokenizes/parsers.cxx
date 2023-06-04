#pragma once
#include "parsers.hpp"

namespace tokenizes {

template <class O>
bool repeat<O>::operator()(std::stringstream &ss, O &r) const {
    size_t count = 0;

    for (; count < min; count++) {
        if (!(*parser)(ss, r)) {
            return false;
        }
    }
    for (; count < max; count++) {
        if (!(*parser)(ss, r)) {
            return true;
        }
    }
    return true;
}

template <class RO, class RI>
bool mapper<RO, RI>::operator()(std::stringstream &ss, RO &ro) const {
    RI ri;
    if (!(*parser)(ss, ri)) {
        return false;
    }
    ro = func(ri);
    return true;
}

template <class O>
bool choose<O>::operator()(std::stringstream &ss, O &r) const {
    // store
    const auto pos = ss.tellg();
    for (const auto parser : parsers) {
        if ((*parser)(ss, r)) {
            return true;
        }
        // restore
        if (pos != ss.tellg()) {
            throw std::runtime_error("overrun, use attempt");
        }
    }
    return false;
}

template <class O>
bool sequence<O>::operator()(std::stringstream &ss, O &r) const {
    return false;//TODO: implement
}

} // namespace tokenizes