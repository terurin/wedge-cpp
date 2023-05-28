#pragma once
#include <bitset>
#include <cassert>
#include <compare>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
namespace tokenizes {

struct escaped_char {
    char c;
    escaped_char(char _c) : c(_c) {}
};

std::ostream &operator<<(std::ostream &os, const escaped_char &ec);

struct empty_t {
    empty_t() = default;
    empty_t(const empty_t &) = default;
    empty_t(empty_t &&) = default;
};

template <class R, class L>
struct base;

template <class R, class L>
using base_ptr = std::shared_ptr<const base<R, L>>;

template <class R, class L>
using base_mut_ptr = std::shared_ptr<base<R, L>>;

template <class R, class L>
class repeat;

template <class RO, class RI, class L>
class mapper;

template <class RO, class RI, class L>
class mapper_error;

template <class R, class L>
class choose;

template <class R, class L>
struct base : public std::enable_shared_from_this<base<R, L>> {
    using right_t = R;
    using left_t = L;
    virtual bool operator()(std::stringstream &, right_t &r, left_t &) const = 0;
    virtual bool operator()(std::stringstream &ss, right_t &r) const;
    virtual bool operator()(std::stringstream &ss) const;
    operator std::shared_ptr<base>() { return this->shared_from_this(); }
    operator std::shared_ptr<const base>() const { return this->shared_from_this(); }
    std::shared_ptr<base> as_base() { return this->shared_from_this(); }
    std::shared_ptr<const base> as_base() const { return this->shared_from_this(); }

    /* repeat */
    std::shared_ptr<repeat<R, L>> repeat_n_m(size_t n, size_t m) const {
        return repeat<R, L>::create(this->shared_from_this(), n, m);
    }

    std::shared_ptr<repeat<R, L>> repeat_n(size_t n) const {
        return repeat<R, L>::create(this->shared_from_this(), n, n);
    }

    std::shared_ptr<repeat<R, L>> many1() const { return repeat<R, L>::create(this->shared_from_this(), 1); }
    std::shared_ptr<repeat<R, L>> many0() const { return repeat<R, L>::create(this->shared_from_this(), 0); }

    /* mapper */
    template <std::invocable<R> F>
    auto map(F func) const {
        R r;
        return mapper<typename std::invoke_result_t<F, R>, R, L>::create(this->shared_from_this(), func);
    }

    template <std::invocable<L> F>
    auto map_err(F func) const {
        L l;
        return mapper_error<R, typename std::invoke_result_t<F, L>, L>::create(this->shared_from_this(), func);
    }

    // choose
    virtual std::shared_ptr<choose<R, L>> or_parser(base_ptr<R, L> &&insert) {
        return choose<R, L>::create({this->shared_from_this(), insert});
    }
};

class atom : public base<std::string, empty_t> {
    using chars_t = std::bitset<256>;
    chars_t chars;

public:
    constexpr atom(const chars_t &_chars) : chars(_chars) {}
    atom(const atom &) = default;
    atom(atom &&) = default;
    constexpr chars_t &get_chars() { return chars; }
    constexpr const chars_t &get_chars() const { return chars; }

    virtual bool operator()(std::stringstream &, std::string &, empty_t &) const override;
    virtual bool operator()(std::stringstream &ss, std::string &r) const override {
        empty_t l;
        return (*this)(ss, r, l);
    }
    virtual bool operator()(std::stringstream &ss) const override {
        std::string r;
        empty_t l;
        return (*this)(ss, r, l);
    }
    static std::shared_ptr<atom> create(const chars_t &chars);
    static std::shared_ptr<atom> create(uint8_t c);
    static std::shared_ptr<atom> create(std::string_view sv);
    static std::shared_ptr<atom> create_range(uint8_t first, uint8_t last);
};

using atom_ptr = std::shared_ptr<const atom>;
using atom_mut_ptr = std::shared_ptr<atom>;

std::ostream &operator<<(std::ostream &, const atom &);
atom_ptr operator|(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator&(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator^(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator~(const atom_ptr &a);

atom_mut_ptr &operator|=(atom_mut_ptr &a, const atom_ptr &b);
atom_mut_ptr &operator&=(atom_mut_ptr &a, const atom_ptr &b);
atom_mut_ptr &operator^=(atom_mut_ptr &a, const atom_ptr &b);

static inline atom_ptr sign = atom::create("+-");
static inline atom_ptr dot = atom::create(".");
static inline atom_ptr small = atom::create_range('a', 'z');
static inline atom_ptr large = atom::create_range('A', 'Z');
static inline atom_ptr alpha = small | large;
static inline atom_ptr digit = atom::create_range('0', '9');
static inline atom_ptr alnum = small | large | digit;

template <class R, class L>
class repeat : public base<R, L> {
    const size_t min, max;
    base_ptr<std::string, L> parser;

public:
    repeat(base_ptr<R, L> &&_parser, size_t _min = 0, size_t _max = SIZE_MAX) : parser(_parser), min(_min), max(_max) {
        assert(_min <= _max);
    }
    virtual bool operator()(std::stringstream &ss, R &r, L &l) const override;
    virtual bool operator()(std::stringstream &ss, R &r) const override {
        L l;
        return (*this)(ss, r, l);
    }
    virtual bool operator()(std::stringstream &ss) const override {
        R r;
        L l;
        return (*this)(ss, r, l);
    }

    static std::shared_ptr<repeat<R, L>> create(base_ptr<R, L> &&p, size_t min = 0, size_t max = SIZE_MAX) {
        return std::make_shared<repeat<R, L>>(std::move(p), min, max);
    }
};

template <class R, class L>
using repeat_mut_ptr = std::shared_ptr<const repeat<R, L>>;
template <class R, class L>
using repeat_ptr = std::shared_ptr<repeat<R, L>>;

template <class R, class L>
repeat_mut_ptr<R, L> repeat_n_m(base_ptr<R, L> &&p, size_t n, size_t m) {
    return repeat<R, L>::create(std::move(p), n, m);
}

static inline repeat_mut_ptr<std::string, empty_t> repeat_n_m(base_ptr<std::string, empty_t> &&p, size_t n, size_t m) {
    return repeat<std::string, empty_t>::create(std::move(p), n, m);
}

template <class R, class L>
repeat_mut_ptr<R, L> repeat_n(base_ptr<R, L> &&p, size_t n) {
    return repeat<R, L>::create(std::move(p), n, n);
}

static inline repeat_mut_ptr<std::string, empty_t> repeat_n(base_ptr<std::string, empty_t> &&p, size_t n) {
    return repeat<std::string, empty_t>::create(std::move(p), n, n);
}

template <class R, class L>
repeat_mut_ptr<std::string, L> many1(base_ptr<R, L> &&p) {
    return repeat<R, L>::create(std::move(p), 1);
}

static inline repeat_mut_ptr<std::string, empty_t> many1(base_ptr<std::string, empty_t> &&p) {
    return repeat<std::string, empty_t>::create(std::move(p), 1);
}

template <class R, class L>
repeat_mut_ptr<std::string, L> many0(base_ptr<R, L> &&p, size_t n) {
    return repeat<R, L>::create(std::move(p));
}

static inline repeat_mut_ptr<std::string, empty_t> many0(base_ptr<std::string, empty_t> &&p, size_t n) {
    return repeat<std::string, empty_t>::create(std::move(p));
}

template <class RO, class RI, class L>
class mapper : public base<RO, L> {
public:
    using func_t = std::function<RO(const RI &)>;

private:
    base_ptr<RI, L> parser;
    const func_t func;

public:
    mapper(base_ptr<RI, L> &&_parser, func_t _func) : parser(_parser), func(_func) {}
    mapper(const mapper &) = default;
    mapper(mapper &&) = default;
    virtual bool operator()(std::stringstream &ss, RO &ro, L &l) const override;
    virtual bool operator()(std::stringstream &ss, RO &ro) const override {
        L l;
        return (*this)(ss, ro, l);
    }

    virtual bool operator()(std::stringstream &ss) const override {
        RO ro;
        L l;
        return (*this)(ss, ro, l);
    }

    static std::shared_ptr<mapper<RO, RI, L>> create(base_ptr<RI, L> &&parser, func_t func) {
        return std::make_shared<mapper<RO, RI, L>>(std::move(parser), func);
    }
};

template <class RO, class RI, class L>
using mapper_ptr = std::shared_ptr<const mapper<RO, RI, L>>;
template <class RO, class RI, class L>
using mapper_mut_ptr = std::shared_ptr<mapper<RO, RI, L>>;

template <class RO, class RI, class L>
mapper_mut_ptr<RO, RI, L> map(base_ptr<RI, L> base, std::function<RO(const RI &)> func) {
    return mapper<RO, RI, L>::create(base, func);
}

template <class R, class LO, class LI>
class mapper_error : public base<R, LO> {
public:
    using func_t = std::function<LO(const LI &)>;

private:
    base_ptr<R, LI> parser;
    const func_t func;

public:
    mapper_error(base_ptr<R, LI> &&_parser, func_t _func) : parser(_parser), func(_func) {}
    mapper_error(const mapper_error &) = default;
    mapper_error(mapper_error &&) = default;
    virtual bool operator()(std::stringstream &ss, R &r, LO &l) const override;

    static std::shared_ptr<mapper_error<R, LO, LI>> create(base_ptr<R, LI> &&parser, func_t func) {
        return std::make_shared<mapper_error<R, LO, LI>>(std::move(parser), func);
    }
};

class tag : public base<std::string, empty_t> {
    std::string str;

public:
    tag(std::string_view sv) : str(sv) {}
    tag &set(std::string_view sv) { return str = sv, *this; }
    virtual bool operator()(std::stringstream &ss, std::string &r, empty_t &l) const override;
    virtual bool operator()(std::stringstream &ss, std::string &r) const override {
        empty_t l;
        return (*this)(ss, r, l);
    }
    virtual bool operator()(std::stringstream &ss) const override {
        std::string r;
        empty_t l;
        return (*this)(ss, r, l);
    }

    static std::shared_ptr<tag> create(std::string_view sv) { return std::make_shared<tag>(sv); }
};

template <class R, class L>
class choose : public base<R, L> {
    std::vector<base_ptr<R, L>> parsers;

public:
    choose(base_ptr<R, L> &&_parser) : parsers({_parser}){};
    choose(std::vector<base_ptr<R, L>> &&_parsers) : parsers(_parsers){};
    choose<R, L> &add(base_ptr<R, L> &&parser) { return parsers.emplace(parser), *this; }

    virtual bool operator()(std::stringstream &ss, R &r, L &l) const override;
    virtual bool operator()(std::stringstream &ss, R &r) const override {
        L l;
        return (*this)(ss, r, l);
    }
    virtual bool operator()(std::stringstream &ss) const override {
        R r;
        L l;
        return (*this)(ss, r, l);
    }

    static std::shared_ptr<choose<R, L>> create(base_ptr<R, L> &&parser) {
        return std::make_shared<choose<R, L>>(std::move(parser));
    };

    static std::shared_ptr<choose<R, L>> create(std::vector<base_ptr<R, L>> &&parsers) {
        return std::make_shared<choose<R, L>>(std::move(parsers));
    };
};

template <class R, class L>
auto list(base_ptr<R, L> &&parser) {
    return choose<R, L>::create(std::move(parser));
}
template <class R, class L>
auto list(std::vector<base_ptr<R, L>> &&parser) {
    return choose<R, L>::create(std::move(parser));
}
// template <class R, class L> auto list(base_ptr<R, L> &&parser) { return choose<R, L>::create(std::move(parser)); }

} // namespace tokenizes
#include "parsers.cxx"
