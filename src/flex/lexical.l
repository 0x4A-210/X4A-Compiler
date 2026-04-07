%{ #include <string> 
#include "../AST/Node.h" 
#include "../bison/yacc.hpp" 
extern "C" int yywrap() { return 1;} 
%} 

%% 
[ \t\n]                     ; 
"Num64"                     {yylval.type_=QWORD;return TYPE;}
"Num32"                     {yylval.type_=DWORD;return TYPE;}
"Char"                      {yylval.type_=CHAR;return TYPE;}
[a-zA-Z][a-zA-Z0-9_]*       {yylval.str=new std::string(yytext, yyleng); return VARIABLE;} 
[0-9]+                      {yylval.num_=std::stoll(yytext); return NUMBER;} 
"'"([\x20-\x7E])"'"         {yylval.charac_=yytext[1]; return CHARACTER;} 
"="                         {yylval.op_=ASSIGN; return ASSIGNOP;} 
">"                         {yylval.op_=HIGHER; return HIGHEROP;} 
"<"                         {yylval.op_=LOWER; return LOWEROP;} 
"+"                         {yylval.op_=ADD; return ADDOP;} 
"-"                         {yylval.op_=SUB; return SUBOP;} 
"*"                         {yylval.op_=MUL; return MULOP;} 
"/"                         {yylval.op_=DIV; return DIVOP;} 
";"                         return END; 
%%