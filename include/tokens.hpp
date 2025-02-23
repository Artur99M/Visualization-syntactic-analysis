#pragma once
#include <string>
enum class token_types {
    NO_TOKENS = -1
   ,NUMBER
   ,ID
   ,ADD
   ,SUB
   ,DIV
   ,MUL
   ,IS
   ,ENDSTR
   ,END
   ,START_SCOPE
   ,END_SCOPE
};

std::string token_to_str(token_types);
