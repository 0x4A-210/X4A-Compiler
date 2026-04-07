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
struct X4A_Ctx{
    std::unique_ptr<llvm::LLVMContext> llvmContext_;
    std::unique_ptr<llvm::Module> llvmModule_;
    std::unique_ptr<llvm::IRBuilder<>> llvmBuilder_;
    std::map<std::string, llvm::AllocaInst*> llvmSymTable_;
};