#pragma once

#include <iostream>
#include <cstring>
#include "syntax_analysis_LR0.hpp"
#include "syntax_analysis.hpp"

constexpr size_t COLUMN_WIDTH = 33;
constexpr size_t N_COLUMN     = 3;
constexpr size_t N_VERT       = 4;
void tableHeaderGenerate() {
    for (size_t i = 0; i < COLUMN_WIDTH * N_COLUMN + N_VERT; ++i)
        std::cout << '_';
    std::cout << std::endl;
    std::cout << '|';
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("STACK")) / 2; ++i) {
        std::cout << ' ';
    }
    std::cout << "STACK";
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("STACK")) / 2; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("INPUT")) / 2; ++i) {
        std::cout << ' ';
    }
    std::cout << "INPUT";
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("INPUT")) / 2; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("ACTION")) / 2 + 1; ++i) {
        std::cout << ' ';
    }
    std::cout << "ACTION";
    for (size_t i = 0; i < (COLUMN_WIDTH - strlen("ACTION")) / 2; ++i) {
        std::cout << ' ';
    }
    std::cout << '|';
    std::cout << std::endl;
    for (size_t i = 0; i < COLUMN_WIDTH * N_COLUMN + N_VERT; ++i)
        std::cout << '_';
    std::cout << std::endl;
}

void tableBottomGenerate() {
    for (size_t i = 0; i < COLUMN_WIDTH * N_COLUMN + N_VERT; ++i)
        std::cout << '_';
    std::cout << std::endl;
}
using namespace syntax_analysis;
void tableLineGenerate(SyntaxAnalizer<Grammar::LR0> An) {
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
}
