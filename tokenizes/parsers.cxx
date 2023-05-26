#pragma once
#include "parsers.hpp"

namespace tokenizes {

template <class R, class L> bool base<R, L>::operator()(std::stringstream &ss, R &r) const {
    L l;
    return (*this)(ss, r, l);
}

template <class R, class L> bool base<R, L>::operator()(std::stringstream &ss) const {
    R r;
    L l;
    return (*this)(ss, r, l);
}

template <class R, class L> bool repeat<R, L>::operator()(std::stringstream &ss, R &r, L &l) const {
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

template <class R, class L> bool repeat<R, L>::operator()(std::stringstream &ss, R &r) const {
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

template <class R, class L> bool repeat<R, L>::operator()(std::stringstream &ss) const {
    size_t count = 0;

    for (; count < min; count++) {
        if (!(*parser)(ss)) {
            return false;
        }
    }
    for (; count < max; count++) {
        if (!(*parser)(ss)) {
            return true;
        }
    }
    return true;
}

} // namespace tokenizes