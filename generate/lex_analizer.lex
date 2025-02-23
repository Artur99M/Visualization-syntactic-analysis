%option c++
%option noyywrap
%option yylineno


%{
#include "debug.hpp"
#include "tokens.hpp"
#include <iostream>
#include <cstdlib>

size_t num_line = 1;
%}

endstr [;]
end    [$]
delim  [ \t\n]
ws     {delim}+
letter [A-Za-z]
digit  [0-9]
id     (_)*({letter})({letter}|{digit}|_)*
number {digit}+(\.{digit}+)?((e|E)[+-]?{digit}+)?
is     [=]
add    [+]
sub    [-]
mul    [*]
div    [/]


%%
\n          {++num_line;}
{ws}        {}
{endstr}    {return (int) token_types::ENDSTR;}
({end}|EOF) {return (int) token_types::END;}
{number}    {return (int) token_types::NUMBER;}
{is}        {return (int) token_types::IS;}
{add}       {return (int) token_types::ADD;}
{sub}       {return (int) token_types::SUB;}
{mul}       {return (int) token_types::MUL;}
{div}       {return (int) token_types::DIV;}
{id}        {return (int) token_types::ID;}
[(]         {return (int) token_types::START_SCOPE;}
[)]         {return (int) token_types::END_SCOPE;}
.           {return (int)token_types::NO_TOKENS; }//std::cerr << "unknow symbol in line " << num_line << std::endl; exit(1);}
%%
