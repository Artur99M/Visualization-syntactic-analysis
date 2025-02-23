#include "tokens.hpp"

std::string token_to_str(token_types val) {
    switch(val) {
        case token_types::ID:
            return "id";
        case token_types::NUMBER:
            return "num";
        case token_types::ADD:
            return "+";
        case token_types::SUB:
            return "-";
        case token_types::MUL:
            return "*";
        case token_types::DIV:
            return "/";
        case token_types::IS:
            return "=";
        case token_types::START_SCOPE:
            return "(";
        case token_types::END_SCOPE:
            return ")";
        default:
            return "unknown lexem";
    }

}
