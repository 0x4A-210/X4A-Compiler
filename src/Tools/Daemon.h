#include"../AST/Node.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include"llvm/ADT/APInt.h"
class InsertD{
    llvm::IRBuilder<>& builder_;
    llvm::BasicBlock* savedBlock_;
    llvm::BasicBlock::iterator savedItera_;
public:
    InsertD(llvm::IRBuilder<>& builder):builder_(builder), savedBlock_(builder.GetInsertBlock()), savedItera_(builder.GetInsertPoint()){}
    ~InsertD(){
        if(savedBlock_!=NULL){
            builder_.SetInsertPoint(savedBlock_, savedItera_);
        }
    }
};