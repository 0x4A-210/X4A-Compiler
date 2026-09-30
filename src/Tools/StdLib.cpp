#include"StdLib.h"
#include<iostream>
std::unordered_map<std::string,StdLib> standardLibFunc={
    {"Print", StdLib::PRINTF},
    {"Scan", StdLib::SCANF}
    // {"Read", READ},
    // {"Write", WRITE},
    // {"Open", OPEN},
    // {"Close", CLOSE}
};

void ResolveGLIBC_printf(X4A_Ctx& context,int funcID_){
    llvm::FunctionType* funcType=llvm::FunctionType::get(llvm::Type::getInt32Ty(*context.llvmContext_), true);  //可变参数
    //注册函数
    llvm::Function* funcEnternity=llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "printf", *context.llvmModule_);
    
    context.llvmFuncTable_[funcID_]=funcEnternity;  //函数表可以是全局的
}

void ResolveGLIBC_scanf(X4A_Ctx& context,int funcID_){
    llvm::FunctionType* funcType=llvm::FunctionType::get(llvm::Type::getInt32Ty(*context.llvmContext_),true);
    llvm::Function* funcEnternity=llvm::Function::Create(funcType,llvm::Function::ExternalLinkage,"scanf",*context.llvmModule_);
    context.llvmFuncTable_[funcID_]=funcEnternity;
}

void RuntimeResolveGLIBC(X4A_Ctx& context,int funcID_,std::string& funcName){
    if(context.llvmFuncTable_.find(funcID_)!=context.llvmFuncTable_.end()){
        //说明已经解析过了
        return;
    }
    else{
        switch(standardLibFunc[funcName]){
            case StdLib::PRINTF:{
                ResolveGLIBC_printf(context,funcID_);
                break;
            }
            case StdLib::SCANF:{
                ResolveGLIBC_scanf(context,funcID_);
                break;
            }
            default:{
                std::cerr<<"This func have'nt support"<<std::endl;
                break;
            }
        }
    }
}