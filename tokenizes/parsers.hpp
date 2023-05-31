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

template <class R>
struct base;

template <class R>
using base_ptr = std::shared_ptr<const base<R>>;

template <class R>
using base_mut_ptr = std::shared_ptr<base<R>>;

template <class R>
class repeat;

template <class RO, class RI>
class mapper;

template <class R>
class choose;

template <class R>
struct base : public std::enable_shared_from_this<base<R>> {
    using right_t = R;
    virtual bool operator()(std::stringstream &ss, right_t &r) const = 0;
    operator std::shared_ptr<base>() { return this->shared_from_this(); }
    operator std::shared_ptr<const base>() const { return this->shared_from_this(); }
    std::shared_ptr<base> as_base() { return this->shared_from_this(); }
    std::shared_ptr<const base> as_base() const { return this->shared_from_this(); }

    /* repeat */
    auto repeat_n_m(size_t n, size_t m) const { return repeat<R>::create(this->shared_from_this(), n, m); }
    auto repeat_n(size_t n) const { return repeat<R>::create(this->shared_from_this(), n, n); }
    auto many1() const { return repeat<R>::create(this->shared_from_this(), 1); }
    auto many0() const { return repeat<R>::create(this->shared_from_this(), 0); }

    /* mapper */
    template <std::invocable<R> F>
    auto map(F func) const {
        R r;
        return mapper<typename std::invoke_result_t<F, R>, R>::create(this->shared_from_this(), func);
    }

    // choose
    virtual std::shared_ptr<choose<R>> or_parser(base_ptr<R> &&insert) {
        return choose<R>::create({this->shared_from_this(), insert});
    }
};

class atom : public base<std::string> {
    using chars_t = std::bitset<256>;
    chars_t chars;

public:
    constexpr atom(const chars_t &_chars) : chars(_chars) {}
    atom(const atom &) = default;
    atom(atom &&) = default;
    constexpr chars_t &get_chars() { return chars; }
    constexpr const chars_t &get_chars() const { return chars; }

    virtual bool operator()(std::stringstream &ss, std::string &r) const override;
    static std::shared_ptr<atom> create(const chars_t &chars);
    static std::shared_ptr<atom> create(uint8_t c);
    static std::shared_ptr<atom> create(std::string_view sv);
    static std::shared_ptr<atom> create_range(uint8_t first, uint8_t last);
};

using atom_ptr = std::shared_ptr<const atom>;
using atom_mut_ptr = std::shared_ptr<atom>;

std::ostream &operator<<(std::ostream &, const atom &);
atom_ptr operator+(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator-(const atom_ptr &a, const atom_ptr &b);
atom_ptr operator-(const atom_ptr &a);

atom_mut_ptr &operator+=(atom_mut_ptr &a, const atom_ptr &b);
atom_mut_ptr &operator-=(atom_mut_ptr &a, const atom_ptr &b);

static inline atom_ptr sign = atom::create("+-");
static inline atom_ptr dot = atom::create(".");
static inline atom_ptr small = atom::create_range('a', 'z');
static inline atom_ptr large = atom::create_range('A', 'Z');
static inline atom_ptr alpha = small + large;
static inline atom_ptr digit = atom::create_range('0', '9');
static inline atom_ptr alnum = small + large + digit;

template <class R>
class repeat : public base<R> {
    const size_t min, max;
    base_ptr<R> parser;

public:
    repeat(base_ptr<R> &&_parser, size_t _min = 0, size_t _max = SIZE_MAX) : parser(_parser), min(_min), max(_max) {
        assert(_min <= _max);
    }

    virtual bool operator()(std::stringstream &ss, R &r) const override;

    static std::shared_ptr<repeat<R>> create(base_ptr<R> &&p, size_t min = 0, size_t max = SIZE_MAX) {
        return std::make_shared<repeat<R>>(std::move(p), min, max);
    }
};

template <class R>
using repeat_mut_ptr = std::shared_ptr<repeat<R>>;
template <class R>
using repeat_ptr = std::shared_ptr<const repeat<R>>;

template <class R>
auto repeat_n_m(base_ptr<R> &&p, size_t n, size_t m) {
    return repeat<R>::create(std::move(p), n, m);
}

static inline repeat_mut_ptr<std::string> repeat_n_m(base_ptr<std::string> &&p, size_t n, size_t m) {
    return repeat<std::string>::create(std::move(p), n, m);
}

template <class R>
repeat_mut_ptr<R> repeat_n(base_ptr<R> &&p, size_t n) {
    return repeat<R>::create(std::move(p), n, n);
}

static inline repeat_mut_ptr<std::string> repeat_n(base_ptr<std::string> &&p, size_t n) {
    return repeat<std::string>::create(std::move(p), n, n);
}

template <class R>
repeat_mut_ptr<std::string> many1(base_ptr<R> &&p) {
    return repeat<R>::create(std::move(p), 1);
}

static inline auto many1(base_ptr<std::string> &&p) { return repeat<std::string>::create(std::move(p), 1); }

template <class R>
repeat_mut_ptr<std::string> many0(base_ptr<R> &&p, size_t n) {
    return repeat<R>::create(std::move(p));
}

static inline repeat_mut_ptr<std::string> many0(base_ptr<std::string> &&p, size_t n) {
    return repeat<std::string>::create(std::move(p));
}

template <class RO, class RI>
class mapper : public base<RO> {
public:
    using func_t = std::function<RO(const RI &)>;

private:
    base_ptr<RI> parser;
    const func_t func;

public:
    mapper(base_ptr<RI> &&_parser, func_t _func) : parser(_parser), func(_func) {}
    mapper(const mapper &) = default;
    mapper(mapper &&) = default;
    virtual bool operator()(std::stringstream &ss, RO &ro) const override;

    static std::shared_ptr<mapper<RO, RI>> create(base_ptr<RI> &&parser, func_t func) {
        return std::make_shared<mapper<RO, RI>>(std::move(parser), func);
    }
};

template <class RO, class RI>
using mapper_ptr = std::shared_ptr<const mapper<RO, RI>>;
template <class RO, class RI>
using mapper_mut_ptr = std::shared_ptr<mapper<RO, RI>>;

template <class RO, class RI>
mapper_mut_ptr<RO, RI> map(base_ptr<RI> base, std::function<RO(const RI &)> func) {
    return mapper<RO, RI>::create(base, func);
}

class tag : public base<std::string> {
    std::string str;

public:
    tag(std::string_view sv) : str(sv) {}
    tag &set(std::string_view sv) { return str = sv, *this; }
    virtual bool operator()(std::stringstream &ss, std::string &r) const override;

    static std::shared_ptr<tag> create(std::string_view sv) { return std::make_shared<tag>(sv); }
};

template <class R>
class choose : public base<R> {
    std::vector<base_ptr<R>> parsers;

public:
    choose(std::vector<base_ptr<R>> &&_parsers) : parsers(_parsers){};
    virtual bool operator()(std::stringstream &ss, R &r) const override;

    virtual std::shared_ptr<choose<R>> or_parser(base_ptr<R> &&insert) override {
        return choose<R>::create({this->shared_from_this(), insert});
    }

    static std::shared_ptr<choose<R>> create(std::vector<base_ptr<R>> &&parsers) {
        return std::make_shared<choose<R>>(std::move(parsers));
    };
};

template <typename P>
concept base_both = requires(P) { typename P::right_t; };

template <base_both P>
auto list(std::shared_ptr<const P> &&parser) {
    return choose<typename P::right_t>::create(std::move(parser));
}

template <class R>
auto list(base_ptr<R> &&parsers...) {
    std::vector<base_ptr<R>> ps({parsers});
    return choose<R>::create({std::move(ps)});
}

template <base_both B>
auto list(std::vector<std::shared_ptr<B>> &&parsers) {
    return choose<typename B::right_t>::create(std::move(parsers));
}

template <base_both BR, base_both BL>
    requires std::same_as<typename BR::right_t, typename BL::right_t>
auto operator+(std::shared_ptr<BR> &&r, std::shared_ptr<BL> &&l) {
    return r->or_parser(l);
}

template <class R>
class sequence : public base<R> {
    std::vector<base_ptr<R>> items;

public:
    sequence(std::vector<base_ptr<R>> &&_items) : items(_items) {}
    virtual bool operator()(std::stringstream &ss, R &r) const override;

    static std::shared_ptr<sequence<R>> create(std::vector<base_ptr<R>> &&items) {
        return std::make_shared<sequence<R>>(std::move(items));
    }
};

} // namespace tokenizes
#include "parsers.cxx"
