#pragma once
#include"../AST/Node.h"
#include"../Scope/Scope.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include"llvm/ADT/APInt.h"
#include<unordered_map>
#include<memory>
struct VarInfo{
    llvm::Value* addr_;
    Types type_;
    int ptrLevel_;
    VarInfo(Types type=QWORD,llvm::Value* addr=NULL,int ptrLevel=0):type_(type),addr_(addr),ptrLevel_(ptrLevel){}
};
struct X4A_Ctx{
    //LLVM必须的接口
    std::unique_ptr<llvm::LLVMContext> llvmContext_;
    std::unique_ptr<llvm::Module> llvmModule_;
    std::unique_ptr<llvm::IRBuilder<>> llvmBuilder_;
    //自定义结构，遍历语法树时一律以符号ID为键进行查找
    std::unordered_map<int,llvm::Value*> llvmSymTable_;
    std::unordered_map<int, llvm::Function*> llvmFuncTable_;
    ScopeManager* scopeMgr_;
};