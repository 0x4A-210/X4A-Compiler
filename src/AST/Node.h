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
    STRING,
    VOID
};
enum BinaryOP{
    ADD=1,
    SUB,
    MUL,
    DIV,
    HIGHER,
    LOWER,
    EQUAL,
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
    virtual bool ValidIndependExpr() {return false;}
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
    VarReferNode(const std::string& name) : name_(name){}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    std::string GetName() const { return name_; }
};

class VarDeclareNode:public StmtNode{
    Types type_;
    std::string name_;
    ExprNode* value_;
public:
    VarDeclareNode(const std::string& name, ExprNode* value,Types type=QWORD) : name_(name), value_(value),type_(type){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class AssignStmtNode: public StmtNode{
    ExprNode* leftValue_;
    ExprNode* rightValue_;
public:
    AssignStmtNode(ExprNode* leftValue, ExprNode* rightValue) : leftValue_(leftValue), rightValue_(rightValue){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class StmtLists: public Node{
    std::vector<StmtNode*> stmts_;
public:
    StmtLists() {}
    StmtLists(std::vector<StmtNode*> stmts) : stmts_(stmts){}
    void IRGenerate(X4A_Ctx& context);
    void AddStmt(StmtNode* stmt);
    void ShowAST();
};

class BlockNode: public Node{
    std::vector<StmtNode*> stmts_;
public:    
    BlockNode() {}
    BlockNode(std::vector<StmtNode*> stmts) : stmts_(stmts){}
    BlockNode operator =(const BlockNode& other);
    StmtNode* FetchStmts(int idx);
    int StmtNums();
    void IRGenerate(X4A_Ctx& context);
    void AddStmt(StmtNode* stmt);
    void ShowASTNode();
};

class IfElseNode: public StmtNode{
    ExprNode* condition_;
    BlockNode* ifBlock_;
    BlockNode* elseBlock_;
public:
    IfElseNode(ExprNode* condition, BlockNode* ifBlock, BlockNode* elseBlock) : condition_(condition), ifBlock_(ifBlock), elseBlock_(elseBlock){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class FuncDefineNode: public StmtNode{  //声明和定义采用一种结构，如果BlockNode为空表示只声明
    std::string funcName_;
    Types retType_;
    BlockNode* funcBody_;
    std::vector<std::pair<Types,std::string>> paramList_;
public:
    FuncDefineNode(const std::string& funcName,Types retType,BlockNode* funcBody,const std::vector<std::pair<Types,std::string>>& paramList): funcName_(funcName),retType_(retType),funcBody_(funcBody), paramList_(paramList) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class FuncCallNode: public ExprNode{
    std::string funcName_;
    std::vector<ExprNode*> paramList_;
public:
    FuncCallNode(const std::string& funcName,const std::vector<ExprNode*> paramList): funcName_(funcName), paramList_(paramList) {}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    bool ValidIndependExpr() override {return true;}
};

class LegalExprStmtNode: public StmtNode{
    ExprNode* indepExpr_;
public:
    LegalExprStmtNode(ExprNode* indepExpr): indepExpr_(indepExpr) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};

class ReturnNode: public StmtNode{
    ExprNode* retValue_;
public:
    ReturnNode(ExprNode* retValue): retValue_(retValue) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
};