#include"IRGenerate.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include "llvm/ADT/APInt.h"
llvm::Value* NumberNode::IRGenerate(X4A_Ctx& context){
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context.llvmContext_),value_, true);
}

llvm::Value* CharNode::IRGenerate(X4A_Ctx& context){
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context.llvmContext_),value_, false);
}

llvm::Value* BinaryOPNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* leftVal=left_->IRGenerate(context);  //由于Expr里的IRGenerate是纯虚的，因此这里直接多态了，不会出现未实现错误
    if(!leftVal) return NULL;
    llvm::Value* rightVal=right_->IRGenerate(context);
    if(!rightVal) return NULL;
    switch(op_){
        case ADD:{
            return context.llvmBuilder_->CreateAdd(leftVal,rightVal,"exprAdd");
            break;
        }
        case SUB:{
            return context.llvmBuilder_->CreateSub(leftVal,rightVal,"exprSub");
            break;
        }
        case MUL:{
            return context.llvmBuilder_->CreateMul(leftVal,rightVal,"exprMul");
            break;
        }
        case DIV:{
            return context.llvmBuilder_->CreateSDiv(leftVal,rightVal,"exprDiv");
            break;
        }
        case EQUAL:{
            return context.llvmBuilder_->CreateICmpEQ(leftVal, rightVal, "exprEqual");
            break;
        }
        case HIGHER:{
            return context.llvmBuilder_->CreateICmpSGT(leftVal, rightVal, "exprHigher");
        }
        case LOWER:{
            return context.llvmBuilder_->CreateICmpSLT(leftVal, rightVal, "exprLower");
        }
        default:{
            return NULL;
        }
    }
}

llvm::Value* VarReferNode::IRGenerate(X4A_Ctx& context){
    llvm::AllocaInst* memAlloc=context.llvmSymTable_[name_];
    if(memAlloc){
        return context.llvmBuilder_->CreateLoad(memAlloc->getAllocatedType(),memAlloc,name_);
    }
    else return NULL;
}

void VarDeclareNode::IRGenerate(X4A_Ctx& context){
    llvm::Type* varType=NULL;
    switch(type_){
        case QWORD:{
            varType=llvm::Type::getInt64Ty(*context.llvmContext_);
            break;
        }
        case CHAR:{
            varType=llvm::Type::getInt8Ty(*context.llvmContext_);
            break;
        }
        default:{
            varType=llvm::Type::getInt64Ty(*context.llvmContext_);
            break;
        }
    }
    //先分配空间
    llvm::AllocaInst* memAlloc=context.llvmBuilder_->CreateAlloca(varType, nullptr, name_);
    context.llvmSymTable_[name_]=memAlloc;
    //赋值吗？先判空
    if(value_){
        llvm::Value* valPtr=value_->IRGenerate(context);
        context.llvmBuilder_->CreateStore(valPtr, memAlloc);
    }
    return;
}

void AssignStmtNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* rightValue=rightValue_->IRGenerate(context);
    if(rightValue==NULL) return;
    else{
        VarReferNode* tmpVar=(VarReferNode*)leftValue_;
        llvm::AllocaInst* memAlloc=context.llvmSymTable_[tmpVar->GetName()];
        if(memAlloc){
            context.llvmBuilder_->CreateStore(rightValue, memAlloc);
            return;
        }
        else return;
    }
}

void StmtLists::IRGenerate(X4A_Ctx& context){
    for (size_t i = 0; i < stmts_.size(); ++i) {
        stmts_[i]->IRGenerate(context);
    }
}

void BlockNode::IRGenerate(X4A_Ctx& context){
    for (size_t i = 0; i < stmts_.size(); ++i) {
        stmts_[i]->IRGenerate(context);
    }
}

void IfElseNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* cond=condition_->IRGenerate(context);
    llvm::Value* result=context.llvmBuilder_->CreateICmpNE(cond, llvm::ConstantInt::get(cond->getType(), 0),"conditionRes");
    llvm::Function* parentFunc=context.llvmBuilder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* ifAction=llvm::BasicBlock::Create(*context.llvmContext_,"ifAction",parentFunc);
    llvm::BasicBlock* elseAction=llvm::BasicBlock::Create(*context.llvmContext_,"elseAction",parentFunc);
    llvm::BasicBlock* continueCode=llvm::BasicBlock::Create(*context.llvmContext_,"continue",parentFunc);
    context.llvmBuilder_->CreateCondBr(result, ifAction, elseAction);

    context.llvmBuilder_->SetInsertPoint(ifAction);
    ifBlock_->IRGenerate(context);
    context.llvmBuilder_->CreateBr(continueCode);

    if(elseAction){
        context.llvmBuilder_->SetInsertPoint(elseAction);
        elseBlock_->IRGenerate(context);
        context.llvmBuilder_->CreateBr(continueCode);
    }

    context.llvmBuilder_->SetInsertPoint(continueCode);
}