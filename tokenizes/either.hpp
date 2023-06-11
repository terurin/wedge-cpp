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
    constexpr static size_t max_size = std::max(sizeof(right<R>), sizeof(left<L>));
    either_mode mode{either_mode::none};
    std::byte memory[max_size];

    // allocate
    void allocate() { mode = either_mode::none; }
    void allocate(right<R> &&_right) {
        new (memory) right<R>(std::move(_right));
        mode = either_mode::right;
    }

    void allocate(left<L> &&_left) {
        new (memory) left<L>(std::move(_left));
        mode = either_mode::left;
    }

    void allocate(either<R, L> &&origin) {
        switch (origin.mode) {
        case either_mode::right:
            new (memory) right<R>(std::move(*(R *)origin.memory));
            break;
        case either_mode::left:
            new (memory) left<L>(std::move(*(L *)origin.memory));
            break;
        case either_mode::none:
        default:
            break;
        }
        mode = origin.mode;
        origin.reset();
    }

public:
    either() : mode(either_mode::none) {}
    either(right<R> &&_right) { allocate(std::move(_right)); }
    either(left<L> &&_left) { allocate(std::move(_left)); }
    either(either<R, L> &&_either) { allocate(std::move(_either)); }
    ~either() { reset(); }

    // operator =
    either &operator=(either<R, L> &&origin) {
        reset();
        allocate(std::move(origin));
        return *this;
    }

    either &operator=(right<R> &&_right) {
        reset();
        allocate(std::move(_right));
        return *this;
    }

    either &operator=(left<L> &&_left) {
        reset();
        allocate(std::move(_left));
        return *this;
    }

    // reset
    void reset() {
        using enum either_mode;
        switch (mode) {
        case right:
            get_right()->~R();
            mode = none;
            return;
        case left:
            get_left()->~L();
            mode = none;
            return;
        case none:
            return;
        }
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
        return reinterpret_cast<const right<R> *>(memory)->value;
    }
    std::optional<L> opt_left() const {
        if (mode != either_mode::left) {
            return std::nullopt;
        }
        return reinterpret_cast<const left<L> *>(memory)->value;
    }
    // get-*
    right<R> get_right() const {
        if (!is_right()) {
            throw std::range_error("either is not right");
        }
        return *reinterpret_cast<const right<R> *>(memory);
    }

    left<L> get_left() const {
        if (!is_left()) {
            throw std::range_error("either is not left");
        }
        return *reinterpret_cast<const left<L> *>(memory);
    }

    // *-or
    R get_right_or(R &&value) const {
        if (is_right()) {
            return *get_right();
        } else {
            return value;
        }
    }

    L get_left_or(L &&value) const {
        if (is_left()) {
            return *get_left();
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
            return E(right<R2>(func(*get_right())));
        case either_mode::left:
            return E(get_left());
        case either_mode::none:
            return E();
        default:
            throw std::range_error("unexpected mode");
        }
    }
    template <std::invocable<const L &> F>
    auto left_map(F &&func) {
        using L2 = std::invoke_result_t<F, L>;
        using E = either<R, L2>;
        switch (mode) {
        case either_mode::right:
            return E(get_right());
        case either_mode::left:
            return E(left(func(*get_left())));
        case either_mode::none:
            return E();
        default:
            throw std::range_error("unexpected mode");
        }
    }
};

} // namespace tokenize::eithers