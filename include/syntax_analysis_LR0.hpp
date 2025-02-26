#pragma once

#include "syntax_analysis.hpp"
#include "tokens.hpp"
#include <deque>
#include <iostream>
#include <string>
#include <vector>


namespace syntax_analysis {


template<>
class SyntaxAnalizer<Grammar::LR0> {
private:
    enum non_terminal : unsigned char {
         EE
        ,I
        ,E
        ,T
        ,F
        //auxiliary values
        ,ID_NUM
        ,ADD_SUB
        ,MUL_DIV
        ,RBRACE
        ,LBRACE
        ,IS
        ,ID
        ,END
        ,ERROR_NON_TERMINAL
    };

    enum action : unsigned char {
         SHIFT
        ,REDUCE_F_ID_NUM
        ,REDUCE_T_TF
        ,REDUCE_T_F
        ,REDUCE_E_ET
        ,REDUCE_E_T
        ,REDUCE_I_ID_E
        ,REDUCE_I_E
        ,REDUCE_EE_I
        ,REDUCE_F_L_E_R
        ,ACTION_ERROR
        ,ACTION_EMPTY
    };
    static std::string  action_to_string          (action      a);
    static std::string  nontostr                  (non_terminal );
    static non_terminal token_type_to_non_terminal(token_types t);

    //std::deque was chosen because I needs to erase in front of and in back of container.
    //Opportunity to dump container with using deque remains.
    std::vector<non_terminal> stack_;
    std::deque<token_types>   input_;
    action                    last_action_ = ACTION_EMPTY;

public:
    SyntaxAnalizer(std::deque<token_types>& stream_tokens);
    SyntaxAnalizer(std::istream&            is = std::cin);
public:
    std::string dump       () const;
    bool        step       ()      ;
    std::string input_dump () const;
    std::string stack_dump () const;
    std::string last_action() const;
private:
    template <token_types T>
    inline void shift      ()      ;
    void        reduce     ()      ;
};

} //namespace sintax_analysis
