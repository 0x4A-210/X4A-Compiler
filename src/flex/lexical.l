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
"String"                    {yylval.type_=STR; return TYPE;}
\"([^\\\"]|\\.)*\"          {yylval.strVal_=new std::string(yytext,yyleng); return STRING;}
[a-zA-Z][a-zA-Z0-9_]*       {yylval.str=new std::string(yytext, yyleng); return IDENTITY;} 
[0-9]+                      {yylval.num_=std::stoll(yytext); return NUMBER;} 
"'"([\x20-\x7E])"'"         {yylval.charac_=yytext[1]; return CHARACTER;}
"="                         {return ASSIGN;} 
"=="                        {yylval.binaryOP_=EQUAL; return EQUALOP;}
">"                         {yylval.binaryOP_=HIGHER; return HIGHEROP;} 
"<"                         {yylval.binaryOP_=LOWER; return LOWEROP;} 
"+"                         {yylval.binaryOP_=ADD; return ADDOP;} 
"-"                         {yylval.binaryOP_=SUB; return SUBOP;} 
"*"                         {yylval.binaryOP_=MUL; return MULOP;} 
"/"                         {yylval.binaryOP_=DIV; return DIVOP;}
"&"                         {yylval.unaryOP_=REF; return ADDR_OF;}
","                         {return COMMA;} 
"("                         {return LPAREN;}
")"                         {return RPAREN;}
"{"                         {return LBRACE;}
"}"                         {return RBRACE;}
";"                         {return END; }
%%