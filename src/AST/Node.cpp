#include"Node.h"
#include<iostream>
void NumberNode::ShowASTNode(){
    std::cout<<"[Number Type] value = "<<value_;
}

void CharNode::ShowASTNode(){
    std::cout<<"[Char Type] value = '"<<value_<<"'";
}

void BinaryOPNode::ShowASTNode(){
    std::cout<<"[Binary Operation] operator = ";
    switch (op_){
        case ADD:{
            std::cout<<"'+'";
            break;
        }
        case SUB:{
            std::cout<<"'-'";
            break;
        }
        case MUL:{
            std::cout<<"'*'";
            break;
        }
        case DIV:{
            std::cout<<"'/'";
            break;
        }
        case EQUAL:{
            std::cout<<"'=='";
            break;
        }
        case HIGHER:{
            std::cout<<"'>'";
            break;
        }
        case LOWER:{
            std::cout<<"'<'";
            break;
        }
        default:{
            std::cout<<"unknown operation";
            break;
        }
    }
    std::cout<<" ####### ";
    std::cout<<"[Binary Operation] Left Expr:  {";
    left_->ShowASTNode();
    std::cout<<"}"<<" ####### ";
    std::cout<<"[Binary Operation] Right Expr: {";
    right_->ShowASTNode();
    std::cout<<"}";
}

void VarReferNode::ShowASTNode(){
    std::cout<<"[Variable Reference] name = \""<<name_<<"\"";
}

void VarDeclareNode::ShowASTNode(){
    std::cout<<"[Variable Declaration] type = ";
    switch(type_){
        case QWORD:{
            std::cout<<"Num64";
            break;
        }
        case DWORD:{
            std::cout<<"Num32";
            break;
        }
        case WORD:{
            std::cout<<"Num16";
            break;
        }
        case BYTE:{
            std::cout<<"Num8";
            break;
        }
        case CHAR:{
            std::cout<<"Char";
            break;
        }
        case PTR:{
            std::cout<<"Point";
            break;
        }
        default:{
            std::cout<<"unknow type, default regard as Num64";
            break;
        }
    }
    std::cout<<std::endl;
    std::cout<<"[Variable Declaration] name = \""<<name_<<"\""<<std::endl;
    std::cout<<"[Variable Declaration] value = {";
    value_->ShowASTNode();
    std::cout<<"}";
}

void AssignStmtNode::ShowASTNode(){
    std::cout<<"[Assign Statement] left value: {";
    leftValue_->ShowASTNode();
    std::cout<<"}"<<std::endl;
    std::cout<<"[Assign Statement] right value: {";
    rightValue_->ShowASTNode();
    std::cout<<"}";
}

void StmtLists::ShowAST(){
    for (size_t i = 0; i < stmts_.size(); ++i) {
        stmts_[i]->ShowASTNode();
        std::cout<<std::endl;
    }
}

void StmtLists::AddStmt(StmtNode* stmt){
    stmts_.push_back(stmt);
}

void BlockNode::ShowASTNode(){
    std::cout<<"[Block] {"<<std::endl;
    for(size_t i=0;i<stmts_.size();++i){
        stmts_[i]->ShowASTNode();
        std::cout<<std::endl;
    }
    std::cout<<"}"<<std::endl;
}

void BlockNode::AddStmt(StmtNode* stmt){
    stmts_.push_back(stmt);
}

BlockNode BlockNode::operator =(const BlockNode& other){
    stmts_=other.stmts_;
    return *this;
}

void IfElseNode::ShowASTNode(){
    std::cout<<"[If-Else Statement] condition: {";
    condition_->ShowASTNode();
    std::cout<<"}"<<std::endl;
    std::cout<<"[If-Else Statement] if block: {"<<std::endl;
    ifBlock_->ShowASTNode();
    std::cout<<"}"<<std::endl;
    std::cout<<"[If-Else Statement] else block: {"<<std::endl;
    elseBlock_->ShowASTNode();
    std::cout<<"}"<<std::endl;
}