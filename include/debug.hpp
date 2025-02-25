#pragma once
#include <iostream>
namespace Artyr99M {
#ifndef DEBUG
class debuging {
    public:
    // template <typename T>
    // debuging& operator<<(T elem);
    debuging& operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&));
    debuging& flush();
    template <typename T>
    friend debuging& operator<<(debuging& d, T x);
};
extern debuging debug;
#else
extern std::ostream& debug;
#endif //DEBUG
}
