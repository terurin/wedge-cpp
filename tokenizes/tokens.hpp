#pragma once
#include "either.hpp"
#include <string>
#include <variant>
namespace tokenizes::tokens {

using eithers::either;

constexpr static inline uint32_t token_id_marks = 1 << 16;

enum class token_id : uint32_t {
    // literals
    variable,
    boolean,
    integer,
    real,
    // marks
    assign = token_id_marks,
    add,
    sub,
    mul,
    div,

};

constexpr static inline bool is_mark(token_id id) {
    const uint32_t value = static_cast<uint32_t>(id);
    return (value & token_id_marks) == token_id_marks;
}

using value_t = std::variant<std::monostate, bool, int, float, std::string>;

class token {
    token_id id;
    value_t value;
    size_t begin, end;
};

class token_parser {
    // NOTE: cache parsers to reduce memory allcation
public:
    token_parser();
    either<token, std::nullptr_t> operator()(std::istream &is);
};

} // namespace tokenizes::tokens