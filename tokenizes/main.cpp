#include "parsers.hpp"
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

    auto parser = atom::create("a")
                      ->map<std::string>([](std::string &out, const std::string &in) -> bool {
                          out += in + "0";
                          return true;
                      })
                      ->repeat_n(2);
    if ((*parser)(ss, s)) {
        cout << "pass:" << s << endl;
    } else {
        cout << "fail:" << s << endl;
    }
}