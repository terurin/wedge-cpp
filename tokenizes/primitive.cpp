#include "primitive.hpp"

namespace tokenizes::primitive {

std::ostream &operator<<(std::ostream &os, const escaped_char &ec) {
    const char c = ec.c;

    if (isgraph(c)) {
        char buffer[16];
        snprintf(buffer, 256, "'%c'", c);
        return os << buffer;
    }

    if (!isspace(c)) {
        char buffer[16];
        snprintf(buffer, 256, "%02x", c);
        return os << buffer;
    }

    switch (c) {
    case ' ':
        return os << "' '";
    case '\f':
        return os << "\\f";
    case '\n':
        return os << "\\n";
    case '\r':
        return os << "\\n";
    case '\t':
        return os << "\\t";
    case '\v':
        return os << "\\v";
    default:
        return os;
    }
}

std::ostream &operator<<(std::ostream &os, const atom &s) {
    const auto chars = s.get_chars();

    if (chars.none()) {
        return os << "{}";
    }
    if (chars.all()) {
        return os << "{all}";
    }

    os << "{";
    bool use_comma = false;
    for (size_t i = 0; i < 256; i++) {
        if (!chars.test(i)) continue;

        if (use_comma) {
            os << "," << escaped_char((char)i);
        } else {
            os << escaped_char((char)i);
        }
        use_comma = true;
    }
    os << "}";
    return os;
}

std::optional<char> atom::operator()(std::istream &ss) const {
    std::string r;

    const int input = ss.peek();
    if (input == -1||  !chars.test(input)) {
        return std::nullopt;
    }
    ss.ignore();
    return input;
}

std::optional<std::string> tag::operator()(std::istream &ss) const {
    std::string r;
    auto pos = ss.tellg();
    for (const char c : str) {
        const int input = ss.get();
        if (input != -1 && input != c) {
            ss.seekg(pos);
            return std::nullopt;
        }
    }
    return str;
}

} // namespace tokenizes