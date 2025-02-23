#pragma once
#include <iostream>
namespace Artyr99M {
#ifndef DEBUG
class debuging {
    public:
    template <class T>
    debuging& operator<<(T elem);
    debuging& operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&));
    debuging& flush();
};

#else
using debuging = typename std::ostream;
extern std::ostream& debug;
#endif //DEBUG
}
