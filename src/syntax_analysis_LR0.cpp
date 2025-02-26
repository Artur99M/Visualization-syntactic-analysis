#include "syntax_analysis_LR0.hpp"
#include "syntax_analysis.hpp"
#include "debug.hpp"
#include <FlexLexer.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include <cstdlib>
// #include <type_traits>
#include <typeinfo>
namespace syntax_analysis {

const std::string RED   = "\033[31m";
const std::string ERROR = RED + "ERROR" + RED;


SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer(std::deque<token_types>& stream_tokens)
 : input_(stream_tokens) {
    stack_.emplace_back(END);
 }

SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer(std::istream& is) {
    yyFlexLexer lexer{is, std::cerr};
    std::deque<token_types> stream_tokens;
    for (int x = 0; x != (int) token_types::END;) {
        x = lexer.yylex();
        stream_tokens.push_back(static_cast<token_types>(x));
        // Artyr99M::debug << x << " ";
        // Artyr99M::debug.flush();
    }
    // stream_tokens.push_back(token_types::END);
    // Artyr99M::debug << std::endl;
    input_ = std::move(stream_tokens);
    stack_.emplace_back(END);
}

// SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer() {
//     yyFlexLexer lexer;
//     std::deque<token_types> stream_tokens;
//     for (int x = lexer.yylex(); x != (int) token_types::END; x = lexer.yylex()) {
//         stream_tokens.push_back(static_cast<token_types>(x));
//         Artyr99M::debug << x << " ";
//         Artyr99M::debug.flush();
//     }
//     Artyr99M::debug << std::endl;
//     input_ = std::move(stream_tokens);
// }

std::string SyntaxAnalizer<Grammar::LR0>::last_action() const {
    return last_action_;
}

std::string SyntaxAnalizer<Grammar::LR0>::dump() const {
    std::ostringstream str;
    str << "stack:\n" << stack_dump() << '\n'
        << "input:\n" << input_dump() << '\n';

    return str.str();
}

std::string SyntaxAnalizer<Grammar::LR0>::input_dump() const {
    std::ostringstream str;
    for (const auto x: input_) {
        str << token_to_str(x) << ' ';
    }
    return str.str();
}

std::string SyntaxAnalizer<Grammar::LR0>::nontostr(non_terminal x) {
    switch (x) {
        case ID         : return "ID"    ;
        case F          : return "F"     ;
        case ID_NUM     : return "ID_NUM";
        case T          : return "T"     ;
        case E          : return "E"     ;
        case I          : return "I"     ;
        case EE         : return "E\'"   ;
        case MUL_DIV    : return "*"     ;
        case ADD_SUB    : return "+"     ;
        case START_SCOPE: return "("     ;
        case END_SCOPE  : return ")"     ;
        case IS         : return "="     ;
        case END        : return "$"     ;
        default:
            return "unknown non-terminal";
    }
}
std::string SyntaxAnalizer<Grammar::LR0>::stack_dump() const {
    std::ostringstream str;
    for (const auto x: stack_) {
        str << nontostr(x) << ' ';
    }
    return str.str();
}

template<token_types T>
inline void SyntaxAnalizer<Grammar::LR0>::shift() {
    std::cerr << ERROR << ": unexpected lexem!" << std::endl;
    exit(1);
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::ID>() {
    input_.erase(input_.begin());
    if (*(stack_.end() - 1) == END && input_[0] == token_types::IS) {
        stack_.emplace_back(ID);
    } else {
        stack_.emplace_back(ID_NUM);
    }
    last_action_ = "Shift";
}

template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::NUMBER>() {
    input_.erase(input_.begin());
    stack_.emplace_back(ID_NUM);
    last_action_ = "Shift";
}

template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::END_SCOPE>() {
    input_.erase(input_.begin());
    stack_.emplace_back(END_SCOPE);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::START_SCOPE>() {
    input_.erase(input_.begin());
    stack_.emplace_back(START_SCOPE);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::MUL>() {
    input_.erase(input_.begin());
    stack_.emplace_back(MUL_DIV);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::DIV>() {
    input_.erase(input_.begin());
    stack_.emplace_back(MUL_DIV);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::ADD>() {
    input_.erase(input_.begin());
    stack_.emplace_back(ADD_SUB);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::SUB>() {
    input_.erase(input_.begin());
    stack_.emplace_back(ADD_SUB);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::IS>() {
    input_.erase(input_.begin());
    stack_.emplace_back(IS);
    last_action_ = "Shift";
}

void SyntaxAnalizer<Grammar::LR0>::reduce() {
    non_terminal current_top = *(stack_.end() - 1);
    switch(current_top) {
        case ID_NUM:
            *(stack_.end() - 1) = F;
            last_action_ = "Reduce F -> id | num";
            break;
        case F:
            if (stack_.size() > 2 && *(stack_.end() - 2) == MUL_DIV && *(stack_.end() - 3) == T) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = T;
                last_action_ = "Reduce T -> T * F";
            } else {
                *(stack_.end() - 1) = T;
                last_action_ = "Reduce T -> F";
            }
            break;
        case T:
            if (stack_.size() > 2 && *(stack_.end() - 2) == ADD_SUB && *(stack_.end() - 3) == E) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = E;
                last_action_ = "Reduce E -> E + T";
            } else {
                *(stack_.end() - 1) = E;
                last_action_ = "Reduce E -> T";
            }
            break;
        case E:
            if (stack_.size() > 2 && *(stack_.end() - 2) == IS && *(stack_.end() - 3) == ID) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = I;
                last_action_ = "Reduce I -> ID = E";
            } else {
                *(stack_.end() - 1) = I;
                last_action_ = "Reduce I -> E";
            }
            break;
        case I:
            *(stack_.end() - 1) = EE;
            last_action_ = "Reduce E\' -> I";
            break;
        case END_SCOPE:
            if (stack_.size() > 2 && *(stack_.end() - 2) == E && *(stack_.end() - 3) == START_SCOPE) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = F;
                last_action_ = "Reduce F -> (E)";
                break;
            }
        default:
            std::cerr << ERROR << ": can\'t reduce" << std::endl;
            exit(1);
    }
}

bool SyntaxAnalizer<Grammar::LR0>::step() {

    non_terminal current_top  = *(stack_.end() - 1);
    token_types   next_token  = input_[0];

    switch (next_token) {
        case token_types::END:
            if (current_top != EE)
                reduce();
            else
                return true;
            break;
        case token_types::ID:
            if (current_top == ADD_SUB || current_top == START_SCOPE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::ID>();
            } else {
                reduce();
            }
            break;
        case token_types::NUMBER:
            if (current_top == ADD_SUB || current_top == START_SCOPE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::NUMBER>();
            } else {
                reduce();
            }
            break;
        case token_types::END_SCOPE:
            if (current_top == E) {
                shift<token_types::END_SCOPE>();
            } else {
                reduce();
            }
            break;
        case token_types::START_SCOPE:
            if (current_top == ADD_SUB || current_top == START_SCOPE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::START_SCOPE>();
            } else {
                reduce();
            }
            break;
        case token_types::MUL:
            if (current_top == T) {
                shift<token_types::MUL>();
            } else {
                reduce();
            }
            break;
        case token_types::DIV:
            if (current_top == T) {
                shift<token_types::DIV>();
            } else {
                reduce();
            }
            break;
        case token_types::ADD:
            if (current_top == E) {
                shift<token_types::ADD>();
            } else {
                reduce();
            }
            break;
        case token_types::SUB:
            if (current_top == E) {
                shift<token_types::SUB>();
            } else {
                reduce();
            }
            break;
        case token_types::IS:
            if (current_top == ID) {
                shift<token_types::IS>();
            } else {
                reduce();
            }
            break;
        default:
            std::cerr << ERROR << ": unexpected lexem!" << std::endl;
            exit(1);
    }

    return false;

}

// SyntaxAnalizer<Grammar::LR0>::Node::operator std::string() const {
//     return "bark";
// }
// SyntaxAnalizer<Grammar::LR0>::Node::~Node() {
//     for (BaseNode* x : children) {
//         if (typeid(x) == typeid(*this)) {
//             dynamic_cast<Node&>(*x).~Node();
//         }
//     }
// }

// SyntaxAnalizer<Grammar::LR0>::Leaf::operator std::string() const {
//     return token_to_str(token);
// }
} //namespace syntax_analysis
