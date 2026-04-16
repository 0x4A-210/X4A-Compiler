#pragma once
#include"../AST/Node.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include"llvm/ADT/APInt.h"
#include<map>
#include<memory>
struct VarInfo{
    llvm::AllocaInst* addr_;
    Types type_;
    int ptrLevel_;
    VarInfo(Types type=QWORD,llvm::AllocaInst* addr=NULL,int ptrLevel=0):type_(type),addr_(addr),ptrLevel_(ptrLevel){}
};
struct X4A_Ctx{
    std::unique_ptr<llvm::LLVMContext> llvmContext_;
    std::unique_ptr<llvm::Module> llvmModule_;
    std::unique_ptr<llvm::IRBuilder<>> llvmBuilder_;
    std::map<std::string, VarInfo> llvmSymTable_;
    std::map<std::string, llvm::Function*> llvmFuncTable_;
};