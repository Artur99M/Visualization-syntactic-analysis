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

SyntaxAnalizer<Grammar::LR0>::SyntaxAnalizer(std::deque<token_types>& stream_tokens)
 : input_(stream_tokens) {
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
        case ID:
            return "ID";
            break;
        case F:
            return "F";
            break;
        case ID_NUM:
            return "ID_NUM";
            break;
        case T:
            return "T";
            break;
        case E:
            return "E";
            break;
        case I:
            return "I";
            break;
        case EE:
            return "E\'";
            break;
        case MUL_DIV:
            return "*";
            break;
        case ADD_SUB:
            return "+";
            break;
        case START_SCOPE:
            return "(";
            break;
        case END_SCOPE:
            return ")";
            break;
        case IS:
            return "=";
            break;
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


bool SyntaxAnalizer<Grammar::LR0>::step() {
    if (input_.size() ==  0) {
        return true;
    }

    if (stack_.size() == 0)
        return first_step();

    non_terminal& current_top = *(stack_.end() - 1); //here may be reference invalidation, be caution
    token_types   next_token  = input_[0];

    switch (current_top) {
        case ID_NUM:
            if (next_token == token_types::IS || next_token == token_types::START_SCOPE ||
                next_token == token_types::ID || next_token == token_types::NUMBER) {
                std::cerr << "\x1b[31mERROR:\x1b[0m" << " unexpected symbol!" << std::endl;
                exit(1);
            }
            current_top = F;
            last_action_ = "Reduce F -> id | num";
            break;
        case F:
            if (stack_.size() > 2 && *(stack_.end() - 2) == MUL_DIV && *(stack_.end() - 3) == T) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                last_action_ = "Reduce T -> T * F | T / F";
            } else {
                current_top  = T;
                last_action_ = "Reduce T -> F";
            }
            break;
        case T:
            if (next_token == token_types::MUL || next_token == token_types::DIV) {
                stack_.emplace_back(MUL_DIV);
                input_.erase(input_.begin());
                last_action_ = "Shift";
            } else if (stack_.size() > 2 && *(stack_.end() - 2) == ADD_SUB && *(stack_.end() - 3) == E) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                last_action_ = "Reduce E -> E + T | E - T";
            } else {
                current_top = E;
                last_action_ = "Reduce E -> T";
            }
            break;
        case E:
            if (next_token == token_types::ADD || next_token == token_types::SUB) {
                stack_.emplace_back(ADD_SUB);
                input_.erase(input_.begin());
                last_action_ = "Shift";
            } else if (next_token == token_types::END_SCOPE) {
                stack_.emplace_back(END_SCOPE);
                input_.erase(input_.begin());
                last_action_ = "Shift";
            } else if (stack_.size() > 2 && *(stack_.end() - 2) == IS && *(stack_.end() - 3) == ID) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = I;
                last_action_ = "Reduce I -> id = E";
            } else {
                current_top = I;
                last_action_ = "Reduce I -> E";
            }
            break;
        case I:
            current_top = EE;
            last_action_ = "Reduce E\' -> I";
            break;
        case EE:
            if (stack_.size() > 1) {
                std::cerr << "\x1b[31mERROR:\x1b[0m" << " too many \'=\'!" << std::endl;
                exit(1);
            }
            return true;
        case ID:
            if (next_token == token_types::IS) {
                stack_.emplace_back(IS);
                input_.erase(input_.begin());
                last_action_ = "Shift";
            } else {
                current_top = ID_NUM;
                last_action_ = "Reduce ID_NUM -> ID";
            }
            break;
        case END_SCOPE:
            if (*(stack_.end() - 2) == E && *(stack_.end() - 3) == START_SCOPE) {
                stack_.erase(stack_.end() - 1);
                stack_.erase(stack_.end() - 1);
                *(stack_.end() - 1) = F;
                last_action_ = "Reduce F -> (E)";
            } else {
                std::cerr << "\x1b[31mERROR:\x1b[0m" << " error in scopes!" << std::endl;
                exit(1);
            }
            break;
        default:
            shift();
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
bool SyntaxAnalizer<Grammar::LR0>::first_step() {
    if (input_.size() ==  0) {
        return true;
    }
    token_types next_token = input_[0];
    input_.erase(input_.begin());
    switch (next_token) {
        case token_types::START_SCOPE:
            stack_.emplace_back(START_SCOPE);
            last_action_ = "Shift";
            break;
        case token_types::END_SCOPE:
            stack_.emplace_back(END_SCOPE);
            last_action_ = "Shift";
            break;
        case token_types::ID:
            stack_.emplace_back(ID);
            last_action_ = "Shift";
            break;
        case token_types::NUMBER:
            stack_.emplace_back(ID_NUM);
            last_action_ = "Shift";
            break;
        case token_types::END:
            stack_.emplace_back(EE);
            last_action_ = "Shift";
            break;
        default:
            std::cerr << "\x1b[31mERROR:\x1b[0m" << " undefined first lexem!" << std::endl;
            exit(1);
    }
    return false;
}

void SyntaxAnalizer<Grammar::LR0>::shift() {

    token_types  next_token     = input_[0];
    input_.erase(input_.begin());

    switch (next_token) {
        case token_types::START_SCOPE:
            stack_.emplace_back(START_SCOPE);
            last_action_ = "Shift";
            break;
        case token_types::IS:
            if (*(stack_.end() - 1) != ID) {
                std::cerr << "\x1b[31mERROR:\x1b[0m" << " in front of = have to be ID!" << std::endl;
                exit(1);
            }
            stack_.emplace_back(IS);
            last_action_ = "Shift";
            break;
        case token_types::ID:
            stack_.emplace_back(ID);
            last_action_ = "Shift";
            break;
        case token_types::NUMBER:
            stack_.emplace_back(ID_NUM);
            last_action_ = "Shift";
            break;
        default:
            std::cerr << "\x1b[31mERROR:\x1b[0m" << " undefined lexem!" << std::endl;
            exit(1);
    }
}

} //namespace syntax_analysis
