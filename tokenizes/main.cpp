// #include "parsers.hpp"

#include "primitive.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;
// using namespace tokenizes;
int main(int argc, char **argv) {
    stringstream ss;
    //ss << "hello_world";
    ss << "hello_worlda";
    ss.seekg(0);
    char c;

    std::vector<std::string> list = {"hello_world","hello", "hola", ""};

    tokenizes::primitive::tag_list tags(list);
    cout << tags << endl;

    if (auto e = tags(ss); e.is_right()) {
        cout << e.get_right();
    } else {
        cout << "failed" << endl;
    }
}