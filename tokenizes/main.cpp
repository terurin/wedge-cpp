// #include "parsers.hpp"

#include "primitive.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;
// using namespace tokenizes;
int main(int argc, char **argv) {
    stringstream ss;
    ss << "aa";
    ss.seekg(0);
    char c;

    std::vector<std::string> list = {"hello", "hola",""};

    tokenizes::primitive::tag_list tags(list);

    cout << tags << endl;
}