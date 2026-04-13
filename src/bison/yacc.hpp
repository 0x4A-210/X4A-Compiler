/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_K40_X4A_SRC_BISON_YACC_HPP_INCLUDED
# define YY_YY_HOME_K40_X4A_SRC_BISON_YACC_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 3 "/home/k40/X4A/src/bison/yacc.y"
 
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
    enum BinaryOP; 
    enum Types;

#line 69 "/home/k40/X4A/src/bison/yacc.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTITY = 258,                /* IDENTITY  */
    TYPE = 259,                    /* TYPE  */
    NUMBER = 260,                  /* NUMBER  */
    CHARACTER = 261,               /* CHARACTER  */
    STRING = 262,                  /* STRING  */
    HIGHEROP = 263,                /* HIGHEROP  */
    LOWEROP = 264,                 /* LOWEROP  */
    ADDOP = 265,                   /* ADDOP  */
    SUBOP = 266,                   /* SUBOP  */
    MULOP = 267,                   /* MULOP  */
    DIVOP = 268,                   /* DIVOP  */
    EQUALOP = 269,                 /* EQUALOP  */
    STD_PRINT = 270,               /* STD_PRINT  */
    STD_SCAN = 271,                /* STD_SCAN  */
    IF = 272,                      /* IF  */
    ELSE = 273,                    /* ELSE  */
    COMMA = 274,                   /* COMMA  */
    LPAREN = 275,                  /* LPAREN  */
    RPAREN = 276,                  /* RPAREN  */
    LBRACE = 277,                  /* LBRACE  */
    RBRACE = 278,                  /* RBRACE  */
    ASSIGN = 279,                  /* ASSIGN  */
    RET = 280,                     /* RET  */
    END = 281                      /* END  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 37 "/home/k40/X4A/src/bison/yacc.y"
 
    std::string* str; 
    long long num_;
    char charac_; 
    std::string* strVal_;
    BinaryOP op_; 
    Types type_;
    ExprNode* expr_; 
    StmtNode* stmt_; 
    BlockNode* block_;
    std::vector<ExprNode*>* callArgs_;
    std::vector<std::pair<Types,std::string>>* declareArgs_;

#line 126 "/home/k40/X4A/src/bison/yacc.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_HOME_K40_X4A_SRC_BISON_YACC_HPP_INCLUDED  */
