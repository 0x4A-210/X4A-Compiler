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
llvm::Value* ImplicitCast(X4A_Ctx& context,llvm::Value* give,llvm::Type* shouldBe){
    llvm::Value* castRes=NULL;
    /* todo */
    return castRes;
}

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
    if(symID_!=-1){
        llvm::Value* res=context.llvmSymTable_[symID_];
        return res;
    }
    else return NULL;
}

llvm::Value* VarReferNode::DerefValue(X4A_Ctx& context){
    if(symID_!=-1){
        llvm::Value* memAlloc=context.llvmSymTable_[symID_];  //memAlloc是一个ptr，LLVM里只有ptr，不存在*
        if(memAlloc){
            Symbol varInstance=context.scopeMgr_->GetSymbol(symID_);
            llvm::LoadInst* address = context.llvmBuilder_->CreateLoad(Trans2LLVMType(varInstance.type_,context),memAlloc,varInstance.name_);
            // std::cout<<"让我康康拿到了什么样的Value: ";
            // address->getType()->print(llvm::outs());
            // address->print(llvm::outs());
            // std::cout<<std::endl;
            int newLevel=varInstance.ptrLevel_-1;
            int curLevel= (varInstance.ptrLevel_==0? 0:newLevel);
            llvm::Type* ptr2What=Trans2LLVMType(varInstance.type_,context,curLevel);    //int * p，想办法拿到int
            llvm::LoadInst* value=context.llvmBuilder_->CreateLoad(ptr2What,address,varInstance.name_+"_deref");
            return value;
        }
        else return NULL;
    }
    else return NULL;
}

llvm::Value* VarReferNode::LeftValMemLoad(X4A_Ctx& context) {
    return this->LoadAddress(context);
}

llvm::Value* VarReferNode::IRGenerate(X4A_Ctx& context){
    if(symID_!=-1){
        llvm::Value* memAlloc=context.llvmSymTable_[symID_];
        if(memAlloc){
            Symbol varInstance=context.scopeMgr_->GetSymbol(symID_);
            return context.llvmBuilder_->CreateLoad(Trans2LLVMType(varInstance.type_,context),memAlloc,varInstance.name_);
        }
        else return NULL;
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
    Symbol symInstance=context.scopeMgr_->GetSymbol(symID_);
    llvm::Type* varType=Trans2LLVMType(type_, context,ptrLevel_);
    //先分配空间
    llvm::Value* memAlloc=NULL;
    if(symInstance.kind_==SymKind::GLOBAL){
        memAlloc=new llvm::GlobalVariable(*context.llvmModule_,varType,false,llvm::GlobalValue::InternalLinkage,llvm::Constant::getNullValue(varType),symInstance.name_);
    }
    else{
        memAlloc=context.llvmBuilder_->CreateAlloca(varType, nullptr, symInstance.name_);
    }
    context.llvmSymTable_[symID_]=memAlloc;  //变量保存
    //赋值吗？先判空
    if(value_){
        llvm::Value* rightVal=value_->IRGenerate(context);
        context.llvmBuilder_->CreateStore(rightVal, memAlloc); //把rightVal存到memAlloc里面
    }
    return;
}

void AssignStmtNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* rightValue=rightValue_->IRGenerate(context);
    if(rightValue==NULL) {
        throw std::logic_error("no usable right value");
    }
    else{
        /*
        当前只有变量引用可以作为左值，后续在LeftValMemLoad里扩展：*a、a[0]、a.attr
        */
        llvm::Value* memAlloc=leftValue_->LeftValMemLoad(context);
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
    llvm::BasicBlock* elseAction=elseBlock_?llvm::BasicBlock::Create(*context.llvmContext_,"elseAction",parentFunc):NULL;
    llvm::BasicBlock* continueCode=llvm::BasicBlock::Create(*context.llvmContext_,"continue",parentFunc);
    context.llvmBuilder_->CreateCondBr(result, ifAction, elseBlock_?elseAction:continueCode);

    context.llvmBuilder_->SetInsertPoint(ifAction);
    ifBlock_->IRGenerate(context);
    if(!context.llvmBuilder_->GetInsertBlock()->getTerminator()) {context.llvmBuilder_->CreateBr(continueCode);}

    if(elseBlock_!=NULL){
        context.llvmBuilder_->SetInsertPoint(elseAction);
        elseBlock_->IRGenerate(context);
        if(!context.llvmBuilder_->GetInsertBlock()->getTerminator()) {context.llvmBuilder_->CreateBr(continueCode);}
    }

    context.llvmBuilder_->SetInsertPoint(continueCode);
}

void ReturnNode::IRGenerate(X4A_Ctx& context){
    llvm::Value* retVal=retValue_->IRGenerate(context);
    context.llvmBuilder_->CreateRet(retVal);
}

void FuncDefineNode::IRGenerate(X4A_Ctx& context){
    InsertD guard(*context.llvmBuilder_);
    int saveVarCnt=context.llvmSymTable_.size();
    if(standardLibFunc.find(funcName_)!= standardLibFunc.end()){
        std::cerr<<"You can not define a func having the same name with std func"<<std::endl;
        exit(1);
    }
    /*声明时必定会干的事情
    可以重复声明*/
    llvm::Type* retType=Trans2LLVMType(retType_, context);
    std::vector<llvm::Type*> paramTypes;
    for(int i=0;i<paramList_.size();++i){
        llvm::Type* tmpParamTypeInfo=Trans2LLVMType(paramList_[i].first, context);
        paramTypes.push_back(tmpParamTypeInfo);
    }
    llvm::FunctionType* funcType=llvm::FunctionType::get(retType, paramTypes, false);  //注册参数列表
    //注册函数
    if(context.llvmFuncTable_.find(symID_)!=context.llvmFuncTable_.end() && hasDefined_ && funcBody_!=NULL){  //先检查是否重定义
        std::cerr << "Function redefinition: " << funcName_ << std::endl;
        exit(1);
    }
    else if(context.llvmFuncTable_.find(symID_)!=context.llvmFuncTable_.end()&& !hasDefined_&& funcBody_!=NULL){ 
        /*在表里，但定义tag还为假，这次又有函数体，说明之前只是声明过，现在想要实现函数体了*/
        llvm::Function* funcEternity=context.llvmFuncTable_[symID_];
        for(int i=0;i<paramList_.size();i++){
            llvm::Argument* arg=funcEternity->getArg(i);
            arg->setName(paramList_[i].second);
        }

        //处理插入点
        if(funcBody_==NULL) return;
        else{
            //绑定参数
            llvm::BasicBlock* funcBody=llvm::BasicBlock::Create(*context.llvmContext_,"entry",funcEternity);
            context.llvmBuilder_->SetInsertPoint(funcBody);
            for(int i=0;i<paramSymID_.size();i++){
                llvm::Argument* arg=funcEternity->getArg(i);
                Symbol singleParam=context.scopeMgr_->GetSymbol(paramSymID_[i]);
                llvm::Value* paramAlloc=context.llvmBuilder_->CreateAlloca(Trans2LLVMType(singleParam.type_, context),nullptr, singleParam.name_);
                context.llvmBuilder_->CreateStore(arg, paramAlloc);
                context.llvmSymTable_[paramSymID_[i]]=paramAlloc;
            }
            //生成函数体
            funcBody_->IRGenerate(context);
            if(!context.llvmBuilder_->GetInsertBlock()->getTerminator()){
                if(retType->isVoidTy()) context.llvmBuilder_->CreateRetVoid();
                else context.llvmBuilder_->CreateUnreachable();
            }
        }
    }
    else if(context.llvmFuncTable_.find(symID_)!=context.llvmFuncTable_.end() && funcBody_==NULL){  
        /*之前声明过了，这次又是声明，允许*/
        return;
    }
    else{ /*不在表里，funcBody_可以等于或不等于空，但是肯定要声明的*/
        llvm::Function* funcEternity=llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcName_, *context.llvmModule_);
        context.llvmFuncTable_[symID_]=funcEternity;  //函数表可以是全局的

        //绑定参数
        for(int i=0;i<paramList_.size();i++){
            llvm::Argument* arg=funcEternity->getArg(i);
            arg->setName(paramList_[i].second);
        }

        //处理插入点
        if(funcBody_==NULL) return;
        else{
            //绑定参数
            llvm::BasicBlock* funcBody=llvm::BasicBlock::Create(*context.llvmContext_,"entry",funcEternity);
            context.llvmBuilder_->SetInsertPoint(funcBody);
            for(int i=0;i<paramSymID_.size();i++){
                llvm::Argument* arg=funcEternity->getArg(i);
                Symbol singleParam=context.scopeMgr_->GetSymbol(paramSymID_[i]);
                llvm::Value* paramAlloc=context.llvmBuilder_->CreateAlloca(Trans2LLVMType(singleParam.type_, context),nullptr, singleParam.name_);
                context.llvmBuilder_->CreateStore(arg, paramAlloc);
                context.llvmSymTable_[paramSymID_[i]]=paramAlloc;
            }
            //生成函数体
            funcBody_->IRGenerate(context);
            if(!context.llvmBuilder_->GetInsertBlock()->getTerminator()){
                if(retType->isVoidTy()) context.llvmBuilder_->CreateRetVoid();
                else context.llvmBuilder_->CreateUnreachable();
            }
        }
    }
}

llvm::Value* FuncCallNode::IRGenerate(X4A_Ctx& context){
    llvm::Function* func=NULL;
    if(standardLibFunc.find(funcName_)!= standardLibFunc.end()){
        RuntimeResolveGLIBC(context,symID_,funcName_);  //只解析，绑定ID到context的函数表
    }
    func=context.llvmFuncTable_[symID_];
    if(func==NULL) return NULL;
    std::vector<llvm::Value*> paramValues;
    for(int i=0;i<paramList_.size();++i){
        llvm::Value* tmpParamVal=paramList_[i]->IRGenerate(context);
        if(!tmpParamVal) return NULL;
        paramValues.push_back(tmpParamVal);
    }
    Symbol funcSym=context.scopeMgr_->GetSymbol(symID_);
    llvm::Type *retType=Trans2LLVMType(funcSym.type_,context);
    return context.llvmBuilder_->CreateCall(func, paramValues, retType->isVoidTy()?"":"Call");
}

void LegalExprStmtNode::IRGenerate(X4A_Ctx& context){
    indepExpr_->IRGenerate(context);
    return;
}