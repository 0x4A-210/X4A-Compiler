%define parse.error verbose
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
    class ExprStmtNode;
    enum BinaryOP; 
    enum Types;
} 
%{ 
    #include <iostream> 
    #include <vector> 
    #include <cstdlib> 
    #include "../AST/Node.h" 
    extern int lineNo;
    extern char *yytext;
    extern int yylex(); 
    void yyerror(const char* s) { 
      std::cerr << "Error: "<<s<<", at line "<<lineNo<<", near token "<<yytext<<std::endl;
    } 
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
    ExprNode* expr_; 
    StmtNode* stmt_; 
    BlockNode* block_;
} 
// 声明token（和lex.l完全对应） 
%token <str> VARIABLE
%token <type_> TYPE
%token <num_> NUMBER 
%token <charac_> CHARACTER 
%token <op_> HIGHEROP LOWEROP ADDOP SUBOP MULOP DIVOP EQUALOP 
%token IF ELSE LPAREN RPAREN LBRACE RBRACE ASSIGN END
// 优先级与结合性（先乘除后加减） 
%left EQUALOP
%left HIGHEROP LOWEROP
%left ADDOP SUBOP 
%left MULOP DIVOP 
// 语法规则入口 
%type <expr_> expr 
%type <stmt_> Stmt
%type<block_> CodeBlock
%type<block_> Statements 
%start program_root 
%% program_root: 
  program_root Stmt { program.AddStmt($2); } 
  | 
  ; 

Statements:
  {
    $$= new BlockNode();
  }
  | Statements Stmt
      {
        $1->AddStmt($2);
        $$ =$1;
      }
  ;

CodeBlock:
  LBRACE Statements RBRACE{
    $$ = new BlockNode(*$2);
    delete $2;
  }
  ;

Stmt: 
  TYPE VARIABLE ASSIGN expr END  //变量声明同时赋值
    { 
        $$ = new VarDeclareNode(*$2, $4,$1); 
        delete $2;
    }
  | TYPE VARIABLE END{  //变量只声明
    $$ = new VarDeclareNode(*$2,NULL,$1);
    delete $2;
  }
  | IF LPAREN expr RPAREN CodeBlock {$$ = new IfElseNode($3,$5,NULL);}
  | IF LPAREN expr RPAREN CodeBlock ELSE CodeBlock{$$ =new IfElseNode($3,$5,$7);}
  | expr ASSIGN expr END {$$ = new AssignStmtNode($1,$3);}
  ;

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
  | expr HIGHEROP expr { $$ = new BinaryOPNode($1, $2, $3);}
  | expr LOWEROP expr { $$ = new BinaryOPNode($1, $2, $3);}
  | expr EQUALOP expr {$$ = new BinaryOPNode($1, $2, $3);}
  ;
%%