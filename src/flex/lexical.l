%{ #include <string> 
#include "../AST/Node.h" 
#include "../bison/yacc.hpp" 
int lineNo=1;
extern "C" int yywrap() { return 1;} 
%} 

%% 
[ \t]                       ; 
\n                          {lineNo+=1;}
"return"                    {return RET;}
"if"                        {return IF;}
"else"                      {return ELSE;}
"Num64"                     {yylval.type_=QWORD;return TYPE;}
"Num32"                     {yylval.type_=DWORD;return TYPE;}
"Num16"                     {yylval.type_=WORD;return TYPE;}
"Num8"                      {yylval.type_=BYTE;return TYPE;}
"Char"                      {yylval.type_=CHAR;return TYPE;}
"Void"                      {yylval.type_=VOID;return TYPE;}
[a-zA-Z][a-zA-Z0-9_]*       {yylval.str=new std::string(yytext, yyleng); return IDENTITY;} 
[0-9]+                      {yylval.num_=std::stoll(yytext); return NUMBER;} 
"'"([\x20-\x7E])"'"         {yylval.charac_=yytext[1]; return CHARACTER;}
"="                         {return ASSIGN;} 
"=="                        {yylval.op_=EQUAL; return EQUALOP;}
">"                         {yylval.op_=HIGHER; return HIGHEROP;} 
"<"                         {yylval.op_=LOWER; return LOWEROP;} 
"+"                         {yylval.op_=ADD; return ADDOP;} 
"-"                         {yylval.op_=SUB; return SUBOP;} 
"*"                         {yylval.op_=MUL; return MULOP;} 
"/"                         {yylval.op_=DIV; return DIVOP;}
","                         {return COMMA;} 
"("                         {return LPAREN;}
")"                         {return RPAREN;}
"{"                         {return LBRACE;}
"}"                         {return RBRACE;}
";"                         {return END; }
%%