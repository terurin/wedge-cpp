#pragma once
#include <algorithm>
#include <concepts>
#include <optional>

namespace tokenize::eithers {

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

template <class R, class L>
class either {
    constexpr static size_t max_size = std::max(sizeof(R), sizeof(L));
    either_mode mode;
    std::byte memory[max_size];

    // allocate
    void allocate() { mode = either_mode::none; }
    void allocate(R right) {
        new (memory) R(right);
        mode = either_mode::right;
    }

    void allocate(L left) {
        new (memory) L(left);
        mode = either_mode::left;
    }

    void allocate(const either<R, L> &origin) {
        using enum either_mode;
        switch (origin.mode) {
        case right:
            allocate(origin.get_right());
            break;
        case left:
            allocate(origin.get_left());
            break;
        case none:
            allocate();
            break;
        }
    }

    void allocate(either<R, L> &&origin) {
        using enum either_mode;
        switch (origin.mode) {
        case right:
            allocate(origin.into_right());
            break;
        case left:
            allocate(origin.into_left());
            break;
        case none:
            allocate();
            break;
        }
        origin.release();
    }

    // release
    void release() {
        using enum either_mode;
        switch (mode) {
        case right:
            get_right().~R();
            mode = none;
            return;
        case left:
            get_left().~L();
            mode = none;
            return;
        case none:
            return;
        }
    }

public:
    either() : mode(either_mode::none) {}
    either(const right<R> &_right) { allocate(_right.value); }
    either(right<R> &&_right) { allocate(std::move(_right.value)); }
    either(const left<L> &_left) { allocate(_left.value); }
    either(left<L> &&_left) { allocate(std::move(_left.value)); }
    either(const either<R, L> &_either) { allocate(_either); }
    either(either<R, L> &&_either) { allocate(std::move(_either)); }

    ~either() { release(); }

    // operator =
    either &operator=(const either<R, L> &origin) {
        using enum either_mode;
        release();
        allocate(origin);
        return *this;
    }
    either &operator=(either<R, L> &&origin) {
        using enum either_mode;
        release();
        allocate(std::move(origin));

        return *this;
    }

    either &operator=(const right<R> &_right) {
        release();
        allocate(_right.value);
        return *this;
    }
    either &operator=(right<R> &&_right) {
        release();
        allocate(std::move(_right.value));
        return *this;
    }
    either &operator=(const left<L> &_left) {
        release();
        allocate(_left.value);
        return *this;
    }
    either &operator=(left<L> &&_left) {
        release();
        allocate(std::move(_left.value));
        return *this;
    }

    // reset
    void reset() { release(); }

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
        return *(R *)memory;
    }
    std::optional<L> opt_left() const {
        if (mode != either_mode::left) {
            return std::nullopt;
        }
        return *(L *)memory;
    }
    // get-*
    R &get_right() {
        if (!is_right()) {
            throw std::range_error("either is not right");
        }
        return *(R *)memory;
    }
    const R &get_right() const {
        if (!is_right()) {
            throw std::range_error("either is not right");
        }
        return *(R *)memory;
    }
    R &&into_right() {
        if (!is_right()) {
            throw std::range_error("either is not right");
        }
        return std::move(*(R *)memory);
    }

    L &get_left() {
        if (!is_left()) {
            throw std::range_error("either is not left");
        }
        return *(L *)memory;
    }
    const L &get_left() const {
        if (!is_left()) {
            throw std::range_error("either is not left");
        }
        return *(L *)memory;
    }
    L &&into_left() {
        if (!is_left()) {
            throw std::range_error("either is not left");
        }
        return std::move(*(L *)memory);
    }
    // *-or

    R get_right_or(R &&value) const {
        if (is_right()) {
            return get_right();
        } else {
            return value;
        }
    }
    R get_right_or(const R &value) const {
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
    L get_left_or(const L &value) const {
        if (is_left()) {
            return get_left();
        } else {
            return value;
        }
    }

    // *-map
    template <std::invocable<const R&> F>
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
            throw std::range_error("unexpected mode");
        }
    }
    template <std::invocable<const L&> F>
    auto left_map(F &&func) {
        using L2 = std::invoke_result_t<F, L>;
        using E = either<R, L2>;
        switch (mode) {
        case either_mode::right:
            return E(right(get_right()));
        case either_mode::left:
            return E(left(func(get_left())));
        case either_mode::none:
            return E();
        default:
            throw std::range_error("unexpected mode");
        }
    }

    // into-*-map
    template <std::invocable<R&&> F>
    auto into_right_map(F &&func) {
        using R2 = std::invoke_result_t<F, R>;
        using E = either<R2, L>;
        E result;
        switch (mode) {
        case either_mode::right:
            result = E(right<R2>(func(into_right())));
            break;
        case either_mode::left:
            result = E(left<L>(into_left()));
            break;
        case either_mode::none:
            result = E();
            break;
        default:
            throw std::range_error("unexpected mode");
        }
        release();
        return result;
    }
    template <std::invocable<L&&> F>
    auto into_left_map(F &&func) {
        using L2 = std::invoke_result_t<F, L>;
        using E = either<R, L2>;
        E result;
        switch (mode) {
        case either_mode::right:
            result = E(right(into_right()));
            break;
        case either_mode::left:
            result = E(left(func(into_left())));
            break;
        case either_mode::none:
            result = E();
            break;
        default:
            throw std::range_error("unexpected mode");
        }
        release();
        return result;
    }
};

} // namespace tokenize::eithers