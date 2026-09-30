#pragma once
#include"../IR/IRGenerate.h"
#include"../AST/Node.h"
#include<unordered_map>
enum class StdLib{
    PRINTF=1,
    SCANF,
    READ,
    WRITE,
    OPEN,
    CLOSE
};
extern std::unordered_map<std::string, StdLib> standardLibFunc;
void RuntimeResolveGLIBC(X4A_Ctx& context,int funcID_,std::string& funcName);