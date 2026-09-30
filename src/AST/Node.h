#pragma once
#include"llvm/IR/Value.h"
#include<string>
#include<vector>
#include<cstdint>
#include<limits>
class X4A_Ctx;
class ScopeManager;;
enum Types{
    QWORD=1,
    DWORD,
    WORD,
    BYTE,
    CHAR,
    STR,
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

enum UnaryOP{  //一元运算符
    REF=1,
    DE_REF
};

class Node{
public:
    virtual ~Node() {}
    virtual void ScopeParse(ScopeManager& scopeMgr_) = 0;
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
    virtual llvm::Value* LoadAddress(X4A_Ctx& context){ return NULL;}  //拿取这个表达式的地址，用于给指针赋值
    virtual llvm::Value* DerefValue(X4A_Ctx& context){ return NULL;}  //从一个指针类型表达式中解引用
    virtual llvm::Value* LeftValMemLoad(X4A_Ctx& context) {return NULL;}  //为了后续扩展变量、*p，a[i]等左值
    virtual void ScopeParse(ScopeManager& scopeMgr_) = 0;
};

class StmtNode:public Node{
    //默认有了析构和IRGenerate函数
    /*
    语句类型，实现一个效果，没有值
    */
public:
    virtual void IRGenerate(X4A_Ctx& context)  =0;  //语句，没有值
    virtual void ShowASTNode() =0;
    virtual void ScopeParse(ScopeManager& scopeMgr_) = 0;
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
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class CharNode:public ExprNode{
    char value_;
public:
    CharNode(char value) : value_(value){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override; 
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class StringNode: public ExprNode{
    std::string value_;
public:
    StringNode(const std::string& value) : value_(value){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override;
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class UnaryOPNode: public ExprNode{
    UnaryOP op_;
    ExprNode* expr_;
public:
    UnaryOPNode(UnaryOP op, ExprNode* expr) : op_(op), expr_(expr) {}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class BinaryOPNode:public ExprNode{
    ExprNode* left_;
    ExprNode* right_;
    BinaryOP op_;
public:
    BinaryOPNode(ExprNode* left, BinaryOP op, ExprNode* right) : left_(left), op_(op), right_(right){}
    llvm::Value* IRGenerate(X4A_Ctx& context) override;
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class VarReferNode: public ExprNode{
    std::string name_;  //第一次扫AST时用的字符语义名称
    int symID_=-1;  //生成IR时，用ID
public:
    VarReferNode(const std::string& name) : name_(name){}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    llvm::Value* LoadAddress(X4A_Ctx& context) override;
    llvm::Value* DerefValue(X4A_Ctx& context) override;
    llvm::Value* LeftValMemLoad(X4A_Ctx& context);
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class VarDeclareNode:public StmtNode{
    Types type_;
    std::string name_;
    ExprNode* value_;
    int ptrLevel_;
    int symID_=-1;
public:
    VarDeclareNode(const std::string& name, ExprNode* value,Types type=QWORD,int ptrLevel=0) : name_(name), value_(value),type_(type),ptrLevel_(ptrLevel){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class AssignStmtNode: public StmtNode{
    ExprNode* leftValue_;
    ExprNode* rightValue_;
public:
    AssignStmtNode(ExprNode* leftValue, ExprNode* rightValue) : leftValue_(leftValue), rightValue_(rightValue){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class StmtLists: public Node{
    std::vector<StmtNode*> stmts_;
public:
    StmtLists() {}
    StmtLists(std::vector<StmtNode*> stmts) : stmts_(stmts){}
    void IRGenerate(X4A_Ctx& context);
    void AddStmt(StmtNode* stmt);
    void ShowAST();
    void ScopeParse(ScopeManager& scopeMgr_) override;
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
    void ScopeParse(ScopeManager& scopeMgr_) override;
    void ScopeParseOnly(ScopeManager& scopeMgr_);  //没有守护实例的作用域解析
};

class IfElseNode: public StmtNode{
    ExprNode* condition_;
    BlockNode* ifBlock_;
    BlockNode* elseBlock_;
public:
    IfElseNode(ExprNode* condition, BlockNode* ifBlock, BlockNode* elseBlock) : condition_(condition), ifBlock_(ifBlock), elseBlock_(elseBlock){}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class FuncDefineNode: public StmtNode{  //声明和定义采用一种结构，如果BlockNode为空表示只声明
    std::string funcName_;
    Types retType_;
    BlockNode* funcBody_;
    std::vector<std::pair<Types,std::string>> paramList_;
    int symID_=-1;
    std::vector<int> paramSymID_;  //每个参数的符号ID
    bool hasDefined_;
public:
    FuncDefineNode(const std::string& funcName,Types retType,BlockNode* funcBody,const std::vector<std::pair<Types,std::string>>& paramList,bool hasDefined=false): funcName_(funcName),retType_(retType),funcBody_(funcBody), paramList_(paramList),hasDefined_(hasDefined) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class FuncCallNode: public ExprNode{
    std::string funcName_;
    std::vector<ExprNode*> paramList_;
    int symID_=-1;
public:
    FuncCallNode(const std::string& funcName,const std::vector<ExprNode*> paramList): funcName_(funcName), paramList_(paramList) {}
    llvm::Value* IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
    bool ValidIndependExpr() override {return true;}
};

class LegalExprStmtNode: public StmtNode{  //合法的能作为语句的表达式，比如函数调用
    ExprNode* indepExpr_;
public:
    LegalExprStmtNode(ExprNode* indepExpr): indepExpr_(indepExpr) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};

class ReturnNode: public StmtNode{
    ExprNode* retValue_;
public:
    ReturnNode(ExprNode* retValue): retValue_(retValue) {}
    void IRGenerate(X4A_Ctx& context);
    void ShowASTNode();
    void ScopeParse(ScopeManager& scopeMgr_) override;
};