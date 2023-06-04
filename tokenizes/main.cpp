#include "parsers.hpp"
#include "shell.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;
using namespace tokenizes;
int main(int argc, char **argv) {
    stringstream ss;
    ss << "aa";
    ss.seekg(0);
    char c;
    // empty_t empty;
    string s;

    // auto parser =
    //  mapper<std::string, std::string, empty_t>::create(atom::create('a'), [](std::string &d, const std::string &s) {
    //      d = s + "1";
    //      return true;
    //  });

    auto parser = make_shell(atom::create("a")).map([](const std::string &in) -> int { return 1; });
    int d = 0;
    if ((*parser)(ss, d)) {
        cout << "pass:" << s << endl;
    } else {
        cout << "fail:" << s << endl;
    }
}