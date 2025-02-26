#include "debug.hpp"
#include <iostream>
namespace Artyr99M {
#ifdef NDEBUG
std::ostream& debug = std::cerr;
#else
// template <typename T>
// debuging& debuging::operator<<(const T x) {
//     return *this;
// }

debuging& debuging::operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&)) {
    return *this;
}
debuging& debuging::flush() {
    return *this;
}
debuging debug;
#endif //DEBUG
} //namespace Artyr99M
