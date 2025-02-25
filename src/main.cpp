#include "syntax_analysis_LR0.hpp"
#include <iostream>
#include "debug.hpp"
#include <FlexLexer.h>
int main() {
    using namespace syntax_analysis;
    SyntaxAnalizer<Grammar::LR0> An;
    for (size_t i = 0; i < 103; ++i)
        std::cout << '_';
    std::cout << std::endl;
    std::cout << '|';
    for (size_t i = 0; i < 14; ++i) {
        std::cout << ' ';
    }
    std::cout << "STACK";
    for (size_t i = 0; i < 14; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    for (size_t i = 0; i < 14; ++i) {
        std::cout << ' ';
    }
    std::cout << "INPUT";
    for (size_t i = 0; i < 14; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    for (size_t i = 0; i < 14; ++i) {
        std::cout << ' ';
    }
    std::cout << "ACTION";
    for (size_t i = 0; i < 13; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    std::cout << std::endl;
    for (size_t i = 0; i < 103; ++i)
        std::cout << '_';
    std::cout << std::endl;

    do {
        // Artyr99M::debug << An.dump() << std::endl;
        std::cout << '|' << An.stack_dump();
        if (An.stack_dump().size() <= 33)
        for (size_t i = 0; i < 33 - An.stack_dump().size(); ++i)
            std::cout << ' ';
        std::cout << '|' << An.input_dump();
        if (An.input_dump().size() <= 33)
        for (size_t i = 0; i < 33 - An.input_dump().size(); ++i)
            std::cout << ' ';
        std::cout << '|' << An.last_action();
        for (size_t i = 0; i < 33 - An.last_action().size(); ++i)
            std::cout << ' ';
        std::cout << '|';
        std::cout << std::endl;
    } while (An.step() == false);

    for (size_t i = 0; i < 103; ++i)
        std::cout << '_';
    std::cout << std::endl;
}
