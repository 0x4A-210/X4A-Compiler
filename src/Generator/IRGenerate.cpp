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
        case ASSIGN:{
            return context.llvmBuilder_->CreateICmpEQ(leftVal, rightVal, "exprAssign");
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
    llvm::AllocaInst* memAlloc=context.llvmBuilder_->CreateAlloca(varType, nullptr, name_);
    context.llvmSymTable_[name_]=memAlloc;
    llvm::Value* valPtr=value_->IRGenerate(context);
    context.llvmBuilder_->CreateStore(valPtr, memAlloc);
    return;
}

void StmtLists::IRGenerate(X4A_Ctx& context){
    for (size_t i = 0; i < stmts_.size(); ++i) {
        stmts_[i]->IRGenerate(context);
    }
}