#include "mappers.hpp"

namespace tokenizes::mappers {

template <class T>
template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
constexpr tag_mapper<T>::node_ptr tag_mapper<T>::build_root(R &&r) {

    node_ptr root = std::make_unique<node_t>();
    for (const auto &[key, value] : r) {
        build_node(*root, key, value);
    }
    return root;
}

template <class T>
constexpr void tag_mapper<T>::build_node(node_t &node, std::string_view key, const T &value) {
    if (key.size() == 0) {
        node.value = value; // terminal
        return;
    }

    const size_t index = static_cast<size_t>(key[0]);
    const std::string_view next_key = key.substr(1);
    assert(index < 256);

    if (!node.table[index]) {
        node_ptr next_node = std::make_unique<node_t>();
        build_node(*next_node, next_key, value);
        node.table[index] = std::move(next_node);
    } else {
        build_node(*node.table[index], next_key, value);
    }
}

template <class T>
constexpr std::optional<T> tag_mapper<T>::walk_node(const node_t &node, std::istream &is) {

    const int index = is.get();

    if (index == -1) {
        return node.value;
    }
    assert(0 <= index && index < 256);

    if (node.table[index]) {
        const node_t &next_node = *node.table[index];
        const std::streampos pos = is.tellg();
        if (const std::optional<T> next_value = walk_node(next_node, is); next_value) {
            return next_value;
        }
        is.seekg(pos);
    }

    return node.value;
}

template <class T>
constexpr tag_mapper<T>::node_ptr tag_mapper<T>::copy_root(const node_t &src) {
    node_ptr root = std::make_unique<node_t>();
    copy_node(*root, src);
    return root;
}

template <class T>
constexpr void tag_mapper<T>::copy_node(node_t &dest, const node_t &src) {
    dest.value = src.value;
    for (size_t i = 0; i < 256; i++) {
        if (src.table[i]) {
            dest.table[i]=std::make_unique<node_t>();
            copy_node(*dest.table[i], *src.table[i]);
        }
    }
}

} // namespace tokenizes::mappers