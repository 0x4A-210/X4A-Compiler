#pragma once
#include"llvm/IR/Value.h"
#include<string>
#include<vector>
class X4A_Ctx;
enum Types{
    QWORD=1,
    DWORD,
    WORD,
    BYTE,
    CHAR,
    PTR,
    Function
};
enum BinaryOP{
    ADD=1,
    SUB,
    MUL,
    DIV,
    ASSIGN,
    HIGHER,
    LOWER,
};

class Node{
public:
    virtual ~Node() {}
};

class ExprNode:public Node{
    /*
    表达式类型，比如a=1+3;这里1和3都是一个表达式（或者说ExprNode的子类）
    表达式必须有一个值
    */
  //默认有了析构和IRGenerate函数
public:
    virtual llvm::Value* IRGenerate(X4A_Ctx& context) =0;  //这里返回类型必须是一个llvm::Value类型
    virtual void ShowASTNode() =0;
};

class StmtNode:public Node{
    //默认有了析构和IRGenerate函数
    /*
    语句类型，实现一个效果，没有值
    */
public:
    virtual void IRGenerate(X4A_Ctx& context)  =0;  //语句，没有值
    virtual void ShowASTNode() =0;
};

class NumberNode:public ExprNode{
    /*
    数值，以表达式看待，比如一个单个的1，也是一个表达式
    因此是ExprNode的子类
    */
    long long value_;
public:
    NumberNode(long long value) : value_(value){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override; 
    void ShowASTNode();
};

class CharNode:public ExprNode{
    char value_;
public:
    CharNode(char value) : value_(value){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override; 
    void ShowASTNode();
};

class BinaryOPNode:public ExprNode{
    ExprNode* left_;
    ExprNode* right_;
    BinaryOP op_;
public:
    BinaryOPNode(ExprNode* left, BinaryOP op, ExprNode* right) : left_(left), op_(op), right_(right){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override;
    void ShowASTNode();
};

class VarReferNode: public ExprNode{
    std::string name_;
public:
    VarReferNode(std::string name) : name_(name){}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class VarDeclareNode:public StmtNode{
    Types type_;
    std::string name_;
    ExprNode* value_;
public:
    VarDeclareNode(std::string name, ExprNode* value,Types type=QWORD) : name_(name), value_(value),type_(type){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class StmtLists: public Node{
    std::vector<StmtNode*> stmts_;
public:
    StmtLists() {}
    void IRGenerate(X4A_Ctx& context);
    void AddStmt(StmtNode* stmt);
    void ShowAST();
};