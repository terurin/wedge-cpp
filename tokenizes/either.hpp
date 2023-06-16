#pragma once
#include <algorithm>
#include <concepts>
#include <optional>
#include <stdexcept>
namespace tokenizes::eithers {

template <class R>
struct right {
    R value;
    right(const R &_value) : value(_value) {}
    right(R &&_value) : value(_value) {}
    R &operator*() { return value; }
    const R &operator*() const { return value; }
    R *operator->() { return &value; }
    const R *operator->() const { return &value; }
};

template <class L>
struct left {
    L value;
    left(const L &_value) : value(_value) {}
    left(L &&_value) : value(_value) {}
    L &operator*() { return value; }
    const L &operator*() const { return value; }
    L *operator->() { return &value; }
    const L *operator->() const { return &value; }
};

enum class either_mode { right, none, left };

template <std::destructible R, std::destructible L>
class either {
    constexpr static size_t max_size = std::max(sizeof(right<R>), sizeof(left<L>));
    either_mode mode{either_mode::none};
    std::byte memory[max_size];

public:
    using right_t = R;
    using left_t = L;
    either() : mode(either_mode::none) {}
    template <std::constructible_from<R> IR>
    either(right<IR> &&_right) : mode(either_mode::right) {
        new (memory) R(*_right);
    }
    template <std::constructible_from<L> IL>
    either(left<IL> &&_left) : mode(either_mode::left) {
        new (memory) L(*_left);
    }
    template <std::constructible_from<R> IR, std::constructible_from<L> IL>
    either(either<IR, IL> &&_either) : mode(_either.get_mode()) {
        switch (mode) {
        case either_mode::right:
            new (memory) R(_either.get_right());
            return;
        case either_mode::left:
            new (memory) L(_either.get_left());
            return;
        case either_mode::none:
            return;
        default:
            throw std::domain_error("mode domain error");
        }
    }
    ~either() { reset(); }

    // operator =
    template <std::constructible_from<R> IR, std::constructible_from<L> IL>
    either &operator=(either<IR, IL> &&_either) {
        reset();
        mode = _either.mode;
        switch (mode) {
        case either_mode::right:
            new (memory) R(std::move(*reinterpret_cast<IR *>(_either.memory)));
            break;
        case either_mode::left:
            new (memory) L(std::move(*reinterpret_cast<IL *>(_either.memory)));
            break;
        case either_mode::none:
            break;
        default:
            throw std::domain_error("mode domain error");
        }
        _either.reset();
        return *this;
    }

    template <std::constructible_from<R> IR>
    either &operator=(right<IR> &&_right) {
        reset();
        mode = either_mode::right, new (memory) R(std::move(*_right));
        return *this;
    }

    template <std::constructible_from<L> IL>
    either &operator=(left<IL> &&_left) {
        reset();
        mode = either_mode::left, new (memory) L(std::move(*_left));
        return *this;
    }

    // reset
    void reset() {
        switch (mode) {
        case either_mode::right:
            get_right().~R();
            break;
        case either_mode::left:
            get_left().~L();
            break;
        case either_mode::none:
            break;
        default:
            throw std::domain_error("mode domain error");
        }
        mode = either_mode::none;
    }

    // mode
    either_mode get_mode() const { return mode; }
    bool is_none() const { return mode == either_mode::none; }
    bool is_right() const { return mode == either_mode::right; }
    bool is_left() const { return mode == either_mode::left; }

    // opt-*
    std::optional<R> opt_right() const {
        if (mode != either_mode::right) {
            return std::nullopt;
        }
        return *reinterpret_cast<const R *>(memory);
    }
    std::optional<L> opt_left() const {
        if (mode != either_mode::left) {
            return std::nullopt;
        }
        return *reinterpret_cast<const L *>(memory);
    }
    // get-*
    R &get_right() {
        if (!is_right()) {
            throw std::out_of_range("mode domain error");
        }
        return *reinterpret_cast<R *>(memory);
    }

    const R &get_right() const {
        if (!is_right()) {
            throw std::out_of_range("mode domain error");
        }
        return *reinterpret_cast<const R *>(memory);
    }

    L &get_left() {
        if (!is_left()) {
            throw std::out_of_range("mode domain error");
        }
        return *reinterpret_cast<L *>(memory);
    }

    const L &get_left() const {
        if (!is_left()) {
            throw std::out_of_range("mode domain error");
        }
        return *reinterpret_cast<const L *>(memory);
    }

    // to-*
    right<R> to_right() const { return right<R>(get_right()); }
    left<L> to_left() const { return left<L>(get_left()); }

    // into_-*
    right<R> into_right() {
        const right<R> result = (std::move(get_right()));
        reset();
        return result;
    }

    left<L> into_left() {
        const left<L> result(std::move(get_left()));
        reset();
        return result;
    }

    // *-or
    R get_right_or(R &&value) const {
        if (is_right()) {
            return get_right();
        } else {
            return value;
        }
    }

    L get_left_or(L &&value) const {
        if (is_left()) {
            return get_left();
        } else {
            return value;
        }
    }

    // *-map
    template <std::invocable<const R &> F>
    auto right_map(F &&func) {
        using R2 = std::invoke_result_t<F, R>;
        using E = either<R2, L>;
        switch (mode) {
        case either_mode::right:
            return E(right<R2>(func(get_right())));
        case either_mode::left:
            return E(left<L>(get_left()));
        case either_mode::none:
            return E();
        default:
            throw std::domain_error("mode domain error");
        }
    }

    template <std::invocable<const L &> F>
    auto left_map(F &&func) {
        using L2 = std::invoke_result_t<F, L>;
        using E = either<R, L2>;
        switch (mode) {
        case either_mode::right:
            return E(right<R>(get_right()));
        case either_mode::left:
            return E(left(func(get_left())));
        case either_mode::none:
            return E();
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

} // namespace tokenizes::eithers