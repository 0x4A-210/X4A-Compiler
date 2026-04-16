#include"../AST/Node.h"
#include<string>
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Type.h"
#include "llvm/ADT/APInt.h"
#include"../Generator/IRGenerate.h"
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

llvm::Type* Trans2LLVMType(Types type_,X4A_Ctx& context,int ptrLevel=0){
    if(ptrLevel>0){
        return llvm::PointerType::get(*context.llvmContext_, 0);
    }
    else{
        switch(type_){
            case QWORD:{
                return llvm::Type::getInt64Ty(*context.llvmContext_);
                break;
            }
            case DWORD:{
                return llvm::Type::getInt32Ty(*context.llvmContext_);
                break;
            }
            case WORD:{
                return llvm::Type::getInt16Ty(*context.llvmContext_);
                break;
            }
            case BYTE:{
                return llvm::Type::getInt8Ty(*context.llvmContext_);
                break;
            }
            case CHAR:{
                return llvm::Type::getInt8Ty(*context.llvmContext_);
                break;
            }
            case VOID:{
                return llvm::Type::getVoidTy(*context.llvmContext_);
                break;
            }
            case STR:{
                return llvm::PointerType::get(*context.llvmContext_, 0);
                break;
            }
            default:{
                return llvm::Type::getVoidTy(*context.llvmContext_);
                break;
            }
        }
    }
}