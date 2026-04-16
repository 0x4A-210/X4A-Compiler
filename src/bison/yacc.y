%define parse.error verbose
// 新增：这个块的代码会被写入parser.hpp，解决类型未知问题 
%code requires { 
    // 前向声明，告诉编译器这些东西是类，指针可正常使用 
    class Node; 
    class ExprNode; 
    class StmtNode; 
    class StmtLists; 
    class NumberNode;
    class CharNode; 
    class StringNode;
    class BinaryOPNode; 
    class VarReferNode; 
    class VarDeclareNode; 
    class FuncDefineNode;
    class FuncCallNode;
    class LegalExprStmtNode;
    class ReturnNode;
    class UnaryOPNode;
    enum BinaryOP; 
    enum Types;
    enum UnaryOP;
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
    std::string* strVal_;
    BinaryOP binaryOP_; 
    UnaryOP unaryOP_;
    Types type_;
    ExprNode* expr_; 
    StmtNode* stmt_; 
    BlockNode* block_;
    std::vector<ExprNode*>* callArgs_;
    std::vector<std::pair<Types,std::string>>* declareArgs_;
} 
// 声明token（和lex.l完全对应） 
%token <str> IDENTITY
%token <type_> TYPE
%token <num_> NUMBER 
%token <charac_> CHARACTER 
%token<strVal_> STRING
%token<unaryOP_> ADDR_OF
%token <binaryOP_> HIGHEROP LOWEROP ADDOP SUBOP MULOP DIVOP EQUALOP 
%token IF ELSE COMMA LPAREN RPAREN LBRACE RBRACE ASSIGN RET END
// 优先级与结合性（先乘除后加减） 
%left EQUALOP
%left HIGHEROP LOWEROP
%left ADDOP SUBOP 
%left MULOP DIVOP
%right ADDR_OF
%right DEREF 
// 语法规则入口 
%type <expr_> expr 
%type <stmt_> Stmt
%type<block_> CodeBlock
%type<block_> Statements 
%type<callArgs_> CallArgs
%type<callArgs_> CallArgList
%type<declareArgs_> DeclareArgs
%type<declareArgs_> DeclareArgList
%start program_root 
%% program_root: 
  program_root Stmt { program.AddStmt($2); } 
  | 
  ; 

CallArgs:
  expr{
    $$ =new std::vector<ExprNode*> ();
    $$->push_back($1);
  }
  | CallArgs COMMA expr{
    $1->push_back($3);
    $$ = $1;
  }
  ;

CallArgList:
  {$$= new std::vector<ExprNode*> ();}
  | CallArgs{
    $$ =$1;
  }
  ;

DeclareArgs:
  TYPE IDENTITY{
    $$ = new std::vector<std::pair<Types,std::string>> ();
    $$->push_back(std::pair<Types,std::string>($1,std::string(*$2)));
    delete $2;
  }
  | DeclareArgs COMMA TYPE IDENTITY{
    $1->push_back(std::pair<Types,std::string>($3,std::string(*$4)));
    delete $4;
    $$ = $1;
  }
  ;

DeclareArgList:
  {$$ = new std::vector<std::pair<Types,std::string>> ();}
  | DeclareArgs{
    $$ =$1;
  }
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
  TYPE IDENTITY ASSIGN expr END  //变量声明同时赋值
    { 
        $$ = new VarDeclareNode(*$2, $4,$1); 
        delete $2;
    }
  | TYPE MULOP IDENTITY ASSIGN expr END{  //指针变量
    $$ = new VarDeclareNode(*$3,$5,$1,1);
    delete $3;
  }
  | TYPE IDENTITY END{  //变量只声明
    $$ = new VarDeclareNode(*$2,NULL,$1);
    delete $2;
  }
  | TYPE MULOP IDENTITY END{  //同样，对应的指针变量
    $$ =new VarDeclareNode(*$3,NULL,$1,1);
    delete $3;
  }
  | IF LPAREN expr RPAREN CodeBlock {$$ = new IfElseNode($3,$5,NULL);}
  | IF LPAREN expr RPAREN CodeBlock ELSE CodeBlock{$$ =new IfElseNode($3,$5,$7);}
  | expr ASSIGN expr END {$$ = new AssignStmtNode($1,$3);}
  | TYPE IDENTITY LPAREN DeclareArgList RPAREN{  //函数仅声明
    $$ = new FuncDefineNode(*$2,$1,NULL,*$4);
    delete $2;
    delete $4;
  }
  | TYPE IDENTITY LPAREN DeclareArgList RPAREN CodeBlock{  //函数声明的同时定义
    $$ = new FuncDefineNode(*$2,$1,$6,*$4);
    delete $2;
    delete $4;
  }
  | expr END{
    if(!$1->ValidIndependExpr()){
      yyerror("Ilegal Stand Alone Expr");
    }
    $$ = new LegalExprStmtNode($1);
  }
  | RET expr END{
    $$ = new ReturnNode($2);
  }
  ;

expr: 
  MULOP expr %prec DEREF{
    UnaryOP deref=DE_REF;
    $$ = new UnaryOPNode(deref,$2);
  }
  | ADDR_OF expr{
    $$ = new UnaryOPNode($1,$2);
  }
  | NUMBER { $$ = new NumberNode($1); } 
  | CHARACTER { $$ = new CharNode($1); } 
  | STRING{
    $$ = new StringNode($1->substr(1,$1->size()-2));  //去除开头的两个双引号
    delete $1;
  }
  | IDENTITY { 
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
  | IDENTITY LPAREN CallArgList RPAREN{
      $$ = new FuncCallNode(*$1,*$3);
      delete $1;
      delete $3;
    }
  ;
%%