#include "debug.hpp"
#include <iostream>
namespace Artyr99M {
#ifdef DEBUG
auto & debug = std::cerr;
#else
debuging debug;
debuging& debuging::operator<<(T elem) {
    return *this;
}
debuging& debuging::operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&)) {
    return *this;
}
debuging& debuging::flush() {
    return *this;
}
#endif //DEBUG
} //namespace Artyr99M
