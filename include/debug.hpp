#pragma once
#include <iostream>
namespace Artyr99M {
#ifndef NDEBUG
class debuging {
public:
    debuging& flush();
    template <typename T>
    inline debuging& operator<<(T x) {
        return *this;
    }
    debuging& operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&));
};
extern debuging debug;
#else
extern std::ostream& debug;
#endif //NDEBUG
}
