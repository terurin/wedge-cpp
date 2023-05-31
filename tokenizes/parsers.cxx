#pragma once
#include "parsers.hpp"

namespace tokenizes {

template <class R, class L>
bool base<R, L>::operator()(std::stringstream &ss, R &r) const {
    L l;
    return (*this)(ss, r, l);
}

template <class R, class L>
bool base<R, L>::operator()(std::stringstream &ss) const {
    R r;
    L l;
    return (*this)(ss, r, l);
}

template <class R, class L>
bool repeat<R, L>::operator()(std::stringstream &ss, R &r, L &l) const {
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

template <class RO, class RI, class L>
bool mapper<RO, RI, L>::operator()(std::stringstream &ss, RO &ro, L &l) const {
    RI ri;
    if (!(*parser)(ss, ri, l)) {
        return false;
    }
    ro = func(ri);
    return true;
}

template <class R, class LO, class LI>
bool mapper_error<R, LO, LI>::operator()(std::stringstream &ss, R &r, LO &lo) const {
    LI li;
    if ((*parser)(ss, r, li)) {
        return true;
    }

    lo = func(li);

    return false;
}

template <class R, class L>
bool choose<R, L>::operator()(std::stringstream &ss, R &r, L &l) const {
    // store
    const auto pos = ss.tellg();
    for (const auto parser : parsers) {
        if ((*parser)(ss, r, l)) {
            return true;
        }
        // restore
        if (pos != ss.tellg()) {
            throw std::runtime_error("overrun, use attempt");
        }
    }
    return false;
}

template <class R, class L>
bool sequence<R, L>::operator()(std::stringstream &ss, R &r, L &l) const {
    return false;//TODO: implement
}

} // namespace tokenizes