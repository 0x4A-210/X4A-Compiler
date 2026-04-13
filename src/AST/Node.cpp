#include"Node.h"
#include<iostream>
std::string Type2String(Types type_){
    switch(type_){
        case QWORD:{
            return "Num64";
            break;
        }
        case DWORD:{
            return "Num32";
            break;
        }
        case WORD:{
            return "Num16";
            break;
        }
        case BYTE:{
            return "Num8";
            break;
        }
        case CHAR:{
            return "Char";
            break;
        }
        case VOID:{
            return "Void";
            break;
        }
        default:{
            return "unknown type, default regard as Num64";
            break;
        }
    }
}

void NumberNode::ShowASTNode(){
    std::cout<<"[Number Type] value = "<<value_;
}

void CharNode::ShowASTNode(){
    std::cout<<"[Char Type] value = '"<<value_<<"'";
}

void StringNode::ShowASTNode(){
    std::cout<<"[String Type] value = \""<<value_<<"\"";
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
        case VOID:{
            std::cout<<"Void";
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

StmtNode* BlockNode::FetchStmts(int idx){
    if(idx<stmts_.size()) return stmts_[idx];
    else return NULL;
}

int BlockNode::StmtNums(){
    return stmts_.size();
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

void FuncDefineNode::ShowASTNode(){
    std::cout<<"[Function Definition] name = \""<<funcName_<<"\""<<std::endl;
    std::cout<<"[Function Definition] parameters: {"<<std::endl;
    for(size_t i=0;i<paramList_.size();++i){
        std::cout<<"Type: "<<Type2String(paramList_[i].first)<<", Name: "<<paramList_[i].second<<std::endl;
    }
    std::cout<<"}"<<std::endl;
    std::cout<<"[Function Definition] body: {"<<std::endl;
    funcBody_->ShowASTNode();
    std::cout<<"}";
}

void FuncCallNode::ShowASTNode(){
    std::cout<<"[Function Call] function name = \""<<funcName_<<"\""<<std::endl;
    std::cout<<"[Function Call] arguments: {"<<std::endl;
    for(size_t i=0;i<paramList_.size();++i){
        paramList_[i]->ShowASTNode();
        std::cout<<std::endl;
    }
    std::cout<<"}";
}

void LegalExprStmtNode::ShowASTNode(){
    std::cout<<"[Legal Independ Expression] value: {";
    indepExpr_->ShowASTNode();
    std::cout<<"}";
}

void ReturnNode::ShowASTNode(){
    std::cout<<"[Return Statement] return value: {";
    retValue_->ShowASTNode();
    std::cout<<"}";
}