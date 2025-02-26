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
    enum non_terminal {
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
    };

    //std::deque was chosen because I needs to erase in front of and in back of container.
    //Opportunity to dump container with using deque remains.
    std::vector<non_terminal> stack_;
    std::deque<token_types>  input_;
    std::string              last_action_;

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



public:
    static std::string nontostr(non_terminal);


};

} //namespace sintax_analysis
