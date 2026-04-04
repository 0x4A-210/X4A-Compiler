%{ #include <string> 
#include "../Node.h" 
#include "../bison/yacc.hpp" 
extern "C" int yywrap() { return 1;} 
%} 

%% 
[ \t\n]                     ; 
[a-zA-Z][a-zA-Z0-9_]*       {yylval.str=new std::string(yytext, yyleng); return VARIABLE;} 
[0-9]+                      {yylval.num_=std::stoll(yytext); return NUMBER;} 
"'"([\x20-\x7E])"'"         {yylval.charac_=yytext[1]; return CHARACTER;} 
"="                         {yylval.op_=EQUAL; return EQUALOP;} 
">"                         {yylval.op_=HIGHER; return HIGHEROP;} 
"<"                         {yylval.op_=LOWER; return LOWEROP;} 
"+"                         {yylval.op_=ADD; return ADDOP;} 
"-"                         {yylval.op_=SUB; return SUBOP;} 
"*"                         {yylval.op_=MUL; return MULOP;} 
"/"                         {yylval.op_=DIV; return DIVOP;} 
";"                         return END; 
%%