#include"StdLib.h"
#include<iostream>
std::unordered_map<std::string,StdLib> standardLibFunc={
    {"Print", PRINTF},
    {"Scan", SCANF},
    {"Read", READ},
    {"Write", WRITE},
    {"Open", OPEN},
    {"Close", CLOSE}
};
void ResolveGLIBC_printf(X4A_Ctx& context,std::string& funcName){
    llvm::FunctionType* funcType=llvm::FunctionType::get(llvm::Type::getInt32Ty(*context.llvmContext_), true);  //可变参数
    //注册函数
    llvm::Function* funcEnternity=llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "printf", *context.llvmModule_);
    context.llvmFuncTable_[funcName]=funcEnternity;  //函数表可以是全局的
}

void ResolveGLIBC_scanf(X4A_Ctx& context,std::string& funcName){

}

void RuntimeResolveGLIBC(X4A_Ctx& context,std::string& funcName){
    switch(standardLibFunc[funcName]){
        case PRINTF:{
            ResolveGLIBC_printf(context,funcName);
            break;
        }
        case SCANF:{
            ResolveGLIBC_scanf(context,funcName);
            break;
        }
        default:{
            std::cerr<<"This func have'nt support"<<std::endl;
            break;
        }
    }
}