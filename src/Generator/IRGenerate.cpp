#include"IRGenerate.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include "llvm/ADT/APInt.h"
#include"../Tools/Daemon.h"
#include"../Tools/StdLib.h"
#include<iostream>
#include"../Tools/Helper.h"
llvm::Value* NumberNode::IRGenerate(X4A_Ctx& context){  //所有数值默认64位
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context.llvmContext_),value_, true);
}

llvm::Value* CharNode::IRGenerate(X4A_Ctx& context){
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context.llvmContext_),value_, false);
}

llvm::Value* StringNode::IRGenerate(X4A_Ctx& context){
    return context.llvmBuilder_->CreateGlobalString(value_);
}

llvm::Value* VarReferNode::LoadAddress(X4A_Ctx& context){
    if(context.llvmSymTable_.find(name_)!=context.llvmSymTable_.end()){
        llvm::AllocaInst* memAlloc=context.llvmSymTable_[name_].addr_;
        // std::cout<<"让我康康拿到了什么样的Value: ";
        // memAlloc->getType()->print(llvm::outs());
        // memAlloc->print(llvm::outs());
        // std::cout<<std::endl;
        return memAlloc;
    }
    else return NULL;
}

llvm::Value* VarReferNode::DerefValue(X4A_Ctx& context){
    llvm::AllocaInst* memAlloc=context.llvmSymTable_[name_].addr_;  //memAlloc是一个ptr，LLVM里只有ptr，不存在**
    if(memAlloc){
        llvm::LoadInst* address = context.llvmBuilder_->CreateLoad(memAlloc->getAllocatedType(),memAlloc,name_);
        // std::cout<<"让我康康拿到了什么样的Value: ";
        // address->getType()->print(llvm::outs());
        // address->print(llvm::outs());
        // std::cout<<std::endl;
        int newLevel=context.llvmSymTable_[name_].ptrLevel_-1;
        int curLevel= (context.llvmSymTable_[name_].ptrLevel_==0? 0:newLevel);
        llvm::Type* ptr2What=Trans2LLVMType(context.llvmSymTable_[name_].type_,context,curLevel);    //int * p，想办法拿到int
        llvm::LoadInst* value=context.llvmBuilder_->CreateLoad(ptr2What,address,name_+"_deref");
        return value;
    }
    else return NULL;
}

llvm::Value* VarReferNode::IRGenerate(X4A_Ctx& context){
    llvm::AllocaInst* memAlloc=context.llvmSymTable_[name_].addr_;
    if(memAlloc){
        return context.llvmBuilder_->CreateLoad(memAlloc->getAllocatedType(),memAlloc,name_);
    }
    else return NULL;
}

llvm::Value* UnaryOPNode::IRGenerate(X4A_Ctx& context){
    switch(op_){
        case REF:{
            return expr_->LoadAddress(context);
            break;
        }
        case DE_REF:{
            return expr_->DerefValue(context);
            break;
        }
        default:{
            return NULL;
            break;
        }
    }
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

void VarDeclareNode::IRGenerate(X4A_Ctx& context){
    llvm::Type* varType=Trans2LLVMType(type_, context,ptrLevel_);
    //先分配空间
    llvm::AllocaInst* memAlloc=context.llvmBuilder_->CreateAlloca(varType, nullptr, name_);
    context.llvmSymTable_[name_]=VarInfo(type_,memAlloc,ptrLevel_);
    //赋值吗？先判空
    if(value_){
        llvm::Value* rightVal=value_->IRGenerate(context);
        context.llvmBuilder_->CreateStore(rightVal, memAlloc); //把rightVal存到memAlloc里面
    }
    return;
}

void AssignStmtNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* rightValue=rightValue_->IRGenerate(context);
    if(rightValue==NULL) return;
    else{
        VarReferNode* tmpVar=(VarReferNode*)leftValue_;
        llvm::AllocaInst* memAlloc=context.llvmSymTable_[tmpVar->GetName()].addr_;
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

void ReturnNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* retVal=retValue_->IRGenerate(context);
    context.llvmBuilder_->CreateRet(retVal);
}

void FuncDefineNode::IRGenerate(X4A_Ctx& context){
    InsertD guard(*context.llvmBuilder_);
    if(standardLibFunc.find(funcName_)!= standardLibFunc.end()){
        std::cerr<<"You can not define a func having the same name with std func"<<std::endl;
        exit(1);
    }
    llvm::Type* retType=Trans2LLVMType(retType_, context);
    std::vector<llvm::Type*> paramTypes;
    for(int i=0;i<paramList_.size();++i){
        llvm::Type* tmpParamTypeInfo=Trans2LLVMType(paramList_[i].first, context);
        paramTypes.push_back(tmpParamTypeInfo);
    }
    llvm::FunctionType* funcType=llvm::FunctionType::get(retType, paramTypes, false);  //注册参数列表
    //注册函数
    llvm::Function* funcEnternity=llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcName_, *context.llvmModule_);
    context.llvmFuncTable_[funcName_]=funcEnternity;  //函数表可以是全局的

    //绑定参数
    for(int i=0;i<paramList_.size();i++){
        llvm::Argument* arg=funcEnternity->getArg(i);
        arg->setName(paramList_[i].second);
    }

    //处理插入点
    if(funcBody_==NULL) return;
    else{
        llvm::BasicBlock* funcBody=llvm::BasicBlock::Create(*context.llvmContext_,"entry",funcEnternity);
        context.llvmBuilder_->SetInsertPoint(funcBody);
        for(int i=0;i<paramList_.size();i++){
            llvm::Argument* arg=funcEnternity->getArg(i);
            llvm::AllocaInst* paramAlloc=context.llvmBuilder_->CreateAlloca(Trans2LLVMType(paramList_[i].first, context),nullptr, paramList_[i].second);
            context.llvmBuilder_->CreateStore(arg, paramAlloc);
            context.llvmSymTable_[paramList_[i].second]=VarInfo(paramList_[i].first,paramAlloc);
        }
        // context.currFunc_=funcEnternity;
        //生成函数体
        funcBody_->IRGenerate(context);
        if(retType_==VOID){
            context.llvmBuilder_->CreateRetVoid();
        }
        else{ //取出最后一条语句，相信程序员，一定会自己写好return
            return;
        }
    }
}

llvm::Value* FuncCallNode::IRGenerate(X4A_Ctx& context){
    llvm::Function* func=NULL;
    if(standardLibFunc.find(funcName_)!= standardLibFunc.end()){
        RuntimeResolveGLIBC(context,funcName_);
    }
    func=context.llvmFuncTable_[funcName_];
    if(func==NULL) return NULL;
    std::vector<llvm::Value*> paramValues;
    for(int i=0;i<paramList_.size();++i){
        llvm::Value* tmpParamVal=paramList_[i]->IRGenerate(context);
        if(!tmpParamVal) return NULL;
        paramValues.push_back(tmpParamVal);
    }
    return context.llvmBuilder_->CreateCall(func, paramValues, "Call");
}

void LegalExprStmtNode::IRGenerate(X4A_Ctx& context){
    indepExpr_->IRGenerate(context);
    return;
}