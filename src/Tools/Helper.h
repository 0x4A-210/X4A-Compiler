#pragma once
#include <string>
//前向声明，避免污染其他文件
namespace llvm{
    class Type;
}
class X4A_Ctx;
enum Types;

std::string Type2String(Types type_);
llvm::Type* Trans2LLVMType(Types type_,X4A_Ctx& context);