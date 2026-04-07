// 新增：这个块的代码会被写入parser.hpp，解决类型未知问题 
%code requires { 
    // 前向声明，告诉编译器ExprNode和StmtNode是类，指针可正常使用 
    class Node; 
    class ExprNode; 
    class StmtNode; 
    class StmtLists; 
    class NumberNode;
    class CharNode; 
    class BinaryOPNode; 
    class VarReferNode; 
    class VarDeclareNode; 
    enum BinaryOP; 
    enum Types;
} 
%{ 
    #include <iostream> 
    #include <vector> 
    #include <cstdlib> 
    #include "../AST/Node.h" 
    extern int yylex(); 
    void yyerror(const char* s) { std::cerr << "Error: " << s << std::endl; } 
    // 根节点：保存所有语句 
    StmtLists program; 
%} 
    // 语义值类型：支持字符串（存储变量名/数字/字符） 
%union { 
    std::string* str; 
    long long num_;
    char charac_; 
    BinaryOP op_; 
    Types type_;
    ExprNode* expr; 
    StmtNode* stmt; 
} 
// 声明token（和lex.l完全对应） 
%token <str> VARIABLE
%token <type_> TYPE
%token <num_> NUMBER 
%token <charac_> CHARACTER 
%token <op_> ASSIGNOP HIGHEROP LOWEROP ADDOP SUBOP MULOP DIVOP END 
// 优先级与结合性（先乘除后加减） 
%left ADDOP SUBOP 
%left MULOP DIVOP 
// 语法规则入口 
%type <expr> expr 
%type <stmt> stmt 
%start program_root 
%% program_root: 
  program_root stmt { program.AddStmt($2); } 
  | 
  ; 
stmt: 
  TYPE VARIABLE ASSIGNOP expr END 
    { 
        $$ = new VarDeclareNode(*$2, $4,$1); 
        delete $2;
    } // 赋值语句：a = 123; ; 
  expr: 
    NUMBER { $$ = new NumberNode($1); } 
    | CHARACTER { $$ = new CharNode($1); } 
    | VARIABLE { 
        $$ = new VarReferNode(*$1); 
        delete $1;
      } 
    | expr ADDOP expr { $$ = new BinaryOPNode($1, $2, $3); } //1+2 
    | expr SUBOP expr { $$ = new BinaryOPNode($1, $2, $3); } 
    | expr MULOP expr { $$ = new BinaryOPNode($1, $2, $3); } 
    | expr DIVOP expr { $$ = new BinaryOPNode($1, $2, $3); } 
    ; 
%%