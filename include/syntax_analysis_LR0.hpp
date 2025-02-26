#pragma once

#include "syntax_analysis.hpp"
#include "tokens.hpp"
#include <deque>
#include <iostream>
#include <string>


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
        ,END_SCOPE
        ,START_SCOPE
        ,IS
        ,ID
        ,END
    };

//     struct BaseNode {
//         virtual operator std::string() const = 0;
//         virtual ~BaseNode() = default;
//     };
//     struct Leaf : BaseNode {
//         token_types            token;
//         operator std::string() const override;
//         ~Leaf() override = default;
//     };
//     struct Node : BaseNode {
//         non_terminal           expression;
//         std::vector<BaseNode*> children;
//
//         operator std::string() const override;
//         ~Node() override;
//     };

    std::deque<non_terminal> stack_;
    std::deque<token_types>  input_;
    // Node                     parse_tree_;
    std::string              last_action_;
    // token_types                  next_token_;

public:
    // SyntaxAnalizer();
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
