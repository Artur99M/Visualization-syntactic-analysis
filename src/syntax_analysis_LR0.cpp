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
    return action_to_string(last_action_);
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
        case LBRACE     : return "("     ;
        case RBRACE     : return ")"     ;
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

SyntaxAnalizer<Grammar::LR0>::non_terminal
SyntaxAnalizer<Grammar::LR0>::token_type_to_non_terminal(token_types t) {
  switch (t) {
    case token_types::NUMBER: return ID_NUM;
    case token_types::RBRACE: return RBRACE;
    case token_types::LBRACE: return LBRACE;
    case token_types::MUL   : return MUL_DIV;
    case token_types::DIV   : return MUL_DIV;
    case token_types::ADD   : return ADD_SUB;
    case token_types::SUB   : return ADD_SUB;
    case token_types::IS    : return IS;
    default: return ERROR_NON_TERMINAL;
  }
}

template<token_types T>
inline void SyntaxAnalizer<Grammar::LR0>::shift() {
    non_terminal t = token_type_to_non_terminal(T);
    if (t == ERROR_NON_TERMINAL) {
        throw std::invalid_argument("ERROR: unexpected lexem!");
    }

    input_.pop_front();
    stack_.push_back(t);
    last_action_ = SHIFT;
}

template<>
inline void SyntaxAnalizer<Grammar::LR0>::shift<token_types::ID>() {
    input_.pop_front();
    if (*(stack_.end() - 1) == END && input_[0] == token_types::IS) {
        stack_.emplace_back(ID);
    } else {
        stack_.emplace_back(ID_NUM);
    }
    last_action_ = SHIFT;
}

void SyntaxAnalizer<Grammar::LR0>::reduce() {
    non_terminal current_top = stack_.back();
    switch(current_top) {
        case ID_NUM:
            stack_.back() = F;
            last_action_ = REDUCE_F_ID_NUM;
            break;
        case F:
            if (stack_.size() > 2 && *(stack_.end() - 2) == MUL_DIV && *(stack_.end() - 3) == T) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = T;
                last_action_ = REDUCE_T_TF;
            } else {
                stack_.back() = T;
                last_action_ = REDUCE_T_F;
            }
            break;
        case T:
            if (stack_.size() > 2 && *(stack_.end() - 2) == ADD_SUB && *(stack_.end() - 3) == E) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = E;
                last_action_ = REDUCE_E_ET;
            } else {
                stack_.back() = E;
                last_action_ = REDUCE_E_T;
            }
            break;
        case E:
            if (stack_.size() > 2 && *(stack_.end() - 2) == IS && *(stack_.end() - 3) == ID) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = I;
                last_action_ = REDUCE_I_ID_E;
            } else {
                stack_.back() = I;
                last_action_ = REDUCE_I_E;
            }
            break;
        case I:
            stack_.back() = EE;
            last_action_ = REDUCE_EE_I;
            break;
        case RBRACE:
            if (stack_.size() > 2 && *(stack_.end() - 2) == E && *(stack_.end() - 3) == LBRACE) {
                stack_.pop_back();
                stack_.pop_back();
                stack_.back() = F;
                last_action_ = REDUCE_F_L_E_R;
                break;
            }
        default:
            throw std::logic_error("ERROR: can\'t reduce");
    }
}

bool SyntaxAnalizer<Grammar::LR0>::step() {

    non_terminal current_top  = stack_.back();
    token_types   next_token  = input_.front();

    switch (next_token) {
        case token_types::END:
            if (current_top == EE && stack_.size() == 2 && stack_.front() == END) {
                input_.pop_front();
                if (input_.size() == 0)
                    return true;
                throw std::invalid_argument(std::string("ERROR: final input is not empty! Your input in finish: \"") + input_dump() + "\"");
            } else if (current_top != EE) {
                reduce();
            } else {
                throw std::invalid_argument("ERROR: there isn't expected lexem!");
            }
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
            throw std::invalid_argument("ERROR: unexpected lexem!");
    }

    return false;
}

std::string SyntaxAnalizer<Grammar::LR0>::action_to_string (action a) {
    switch (a) {
        case SHIFT          : return "Shift"               ;
        case REDUCE_F_ID_NUM: return "Reduce F -> id | num";
        case REDUCE_T_TF    : return "Reduce T -> T * F"   ;
        case REDUCE_T_F     : return "Reduce T -> F"       ;
        case REDUCE_E_ET    : return "Reduce E -> E + T"   ;
        case REDUCE_E_T     : return "Reduce E -> T"       ;
        case REDUCE_I_ID_E  : return "Reduce I -> ID = E"  ;
        case REDUCE_I_E     : return "Reduce I -> E"       ;
        case REDUCE_EE_I    : return "Reduce E\' -> I"     ;
        case REDUCE_F_L_E_R : return "Reduce F -> (E)"     ;
        case ACTION_EMPTY   : return ""     ;
        default:              return "ERROR"               ;
    }
}
} //namespace syntax_analysis
