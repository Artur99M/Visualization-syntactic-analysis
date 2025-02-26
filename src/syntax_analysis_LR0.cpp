#include "syntax_analysis_LR0.hpp"
#include "syntax_analysis.hpp"
#include "debug.hpp"
#include <FlexLexer.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
namespace syntax_analysis {

const std::string RED   = "\033[31m";
const std::string ERROR = RED + "ERROR" + RED;


SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer(std::deque<token_types>& stream_tokens)
 : input_(stream_tokens) {
    stack_.push_back(END);
 }

SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer(std::istream& is) {
    yyFlexLexer lexer{is, std::cerr};
    for (int x = 0; x != (int) token_types::END;) {
        x = lexer.yylex();
        input_.push_back(static_cast<token_types>(x));
    }
    stack_.push_back(END);
}

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
        case LBRACE: return "("     ;
        case RBRACE  : return ")"     ;
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
    throw std::invalid_argument(ERROR + ": unexpected lexem!");
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::ID>() {
    input_.pop_front();
    if (stack_.back() == END && input_.front() == token_types::IS) {
        stack_.push_back(ID);
    } else {
        stack_.push_back(ID_NUM);
    }
    last_action_ = "Shift";
}

template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::NUMBER>() {
    input_.pop_front();
    stack_.push_back(ID_NUM);
    last_action_ = "Shift";
}

template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::RBRACE>() {
    input_.pop_front();
    stack_.push_back(RBRACE);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::LBRACE>() {
    input_.pop_front();
    stack_.push_back(LBRACE);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::MUL>() {
    input_.pop_front();
    stack_.push_back(MUL_DIV);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::DIV>() {
    input_.pop_front();
    stack_.push_back(MUL_DIV);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::ADD>() {
    input_.pop_front();
    stack_.push_back(ADD_SUB);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::SUB>() {
    input_.pop_front();
    stack_.push_back(ADD_SUB);
    last_action_ = "Shift";
}
template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::IS>() {
    input_.pop_front();
    stack_.push_back(IS);
    last_action_ = "Shift";
}

void SyntaxAnalizer<Grammar::LR0>::reduce() {
    non_terminal current_top = stack_.back();
    switch(current_top) {
        case ID_NUM:
            stack_.back() = F;
            last_action_ = "Reduce F -> id | num";
            break;
        case F:
            if (stack_.size() > 2 && *(stack_.end() - 2) == MUL_DIV && *(stack_.end() - 3) == T) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = T;
                last_action_ = "Reduce T -> T * F";
            } else {
                stack_.back() = T;
                last_action_ = "Reduce T -> F";
            }
            break;
        case T:
            if (stack_.size() > 2 && *(stack_.end() - 2) == ADD_SUB && *(stack_.end() - 3) == E) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = E;
                last_action_ = "Reduce E -> E + T";
            } else {
                stack_.back() = E;
                last_action_ = "Reduce E -> T";
            }
            break;
        case E:
            if (stack_.size() > 2 && *(stack_.end() - 2) == IS && *(stack_.end() - 3) == ID) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = I;
                last_action_ = "Reduce I -> ID = E";
            } else {
                stack_.back() = I;
                last_action_ = "Reduce I -> E";
            }
            break;
        case I:
            stack_.back() = EE;
            last_action_ = "Reduce E\' -> I";
            break;
        case RBRACE:
            if (stack_.size() > 2 && *(stack_.end() - 2) == E && *(stack_.end() - 3) == LBRACE) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = F;
                last_action_ = "Reduce F -> (E)";
                break;
            }
        default:
            throw std::logic_error(ERROR + ": can\'t reduce");
    }
}

bool SyntaxAnalizer<Grammar::LR0>::step() {

    non_terminal current_top  = stack_.back();
    token_types   next_token  = input_.front();

    switch (next_token) {
        case token_types::END:
            if (current_top != EE)
                reduce();
            else
                return true;
            break;
        case token_types::ID:
            if (current_top == ADD_SUB || current_top == LBRACE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::ID>();
            } else {
                reduce();
            }
            break;
        case token_types::NUMBER:
            if (current_top == ADD_SUB || current_top == LBRACE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::NUMBER>();
            } else {
                reduce();
            }
            break;
        case token_types::RBRACE:
            if (current_top == E) {
                shift<token_types::RBRACE>();
            } else {
                reduce();
            }
            break;
        case token_types::LBRACE:
            if (current_top == ADD_SUB || current_top == LBRACE ||
                current_top == MUL_DIV || current_top == IS || current_top == END) {
                shift<token_types::LBRACE>();
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
            throw std::invalid_argument(ERROR + ": unexpected lexem!");
    }

    return false;
}

} //namespace syntax_analysis
