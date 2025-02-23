#pragma once

#include <FlexLexer.h>
namespace syntax_analysis {

enum class Grammar {
     LR0
    ,LR1
    ,LL0
    ,LL1
    ,LALR
    ,NONE
};

template<Grammar G>
class SyntaxAnalizer {};

} //namespace sintax_analysis
