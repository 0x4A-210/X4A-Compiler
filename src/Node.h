#include"llvm/IR/Value.h"
#include<string>
#include<vector>
enum Types{
    QWORD=1,
    DWORD,
    WORD,
    BYTE,
    CHAR,
    PTR,
    Call
};
enum BinaryOP{
    ADD=1,
    SUB,
    MUL,
    DIV,
    EQUAL,
    HIGHER,
    LOWER,
};

class Node{
public:
    virtual ~Node() {}
};

class ExprNode:public Node{
  //默认有了析构和CodeGenerate函数
public:
    virtual llvm::Value* CodeGenerate(class CodeGenContext& context) =0;
    virtual void ShowASTNode() =0;
};

class StmtNode:public Node{
    //默认有了析构和CodeGenerate函数
public:
    virtual void CodeGenerate(CodeGenContext& context)  =0;
    virtual void ShowASTNode() =0;
};

class NumberNode:public ExprNode{
    long long value_;
public:
    NumberNode(long long value) : value_(value){}
    llvm::Value* CodeGenerate(CodeGenContext& context); 
    void ShowASTNode();
};

class CharNode:public ExprNode{
    char value_;
public:
    CharNode(char value) : value_(value){}
    llvm::Value* CodeGenerate(CodeGenContext& context); 
    void ShowASTNode();
};

class BinaryOPNode:public ExprNode{
    ExprNode* left_;
    ExprNode* right_;
    BinaryOP op_;
public:
    BinaryOPNode(ExprNode* left, BinaryOP op, ExprNode* right) : left_(left), op_(op), right_(right){}
    llvm::Value* CodeGenerate(CodeGenContext& context); 
    void ShowASTNode();
};

class VarReferNode: public ExprNode{
    std::string name_;
public:
    VarReferNode(std::string name) : name_(name){}
    llvm::Value* CodeGenerate(CodeGenContext& context);
    void ShowASTNode();
};

class VarDeclareNode:public StmtNode{
    Types type_;
    std::string name_;
    ExprNode* value_;
public:
    VarDeclareNode(std::string name, ExprNode* value,Types type=QWORD) : name_(name), value_(value),type_(type){}
    void CodeGenerate(CodeGenContext& context);
    void ShowASTNode();
};

class StmtListNode: public Node{
    std::vector<StmtNode*> stmts_;
public:
    StmtListNode() {}
    void CodeGenerate(CodeGenContext& context);
    void AddStmt(StmtNode* stmt);
    void ShowASTNode();
};