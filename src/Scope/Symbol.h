#pragma once
#include "../AST/Node.h"
#include <string>
#include <vector>

enum class SymKind{
    LOCAL,
    GLOBAL,
    FUNC,
    ARGS,
    BUILD_IN
};

struct Symbol{
    int symID_=-1;  //符号的ID，唯一的。
    /*符号的元数据 */
    std::string name_;
    Types type_;  //变量：表示变量类型；函数：表示返回值类型
    SymKind kind_;
    int ptrLevel_=0;
    std::vector<Types> paramTypes_;
    bool defined_=false;
};