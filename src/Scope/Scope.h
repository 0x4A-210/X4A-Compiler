#pragma once
#include "Symbol.h"
#include <unordered_map>
#include <vector>

enum class ScopeKind{
    GLOBAL,
    FUNC,
    BUILD_IN,
    BLOCK
};

struct ScopeFrame{
    ScopeKind kind_;
    std::unordered_map<std::string,int> symMap_;
};

class ScopeManager{
    std::vector<ScopeFrame> scopes_;  //活跃作用域
    std::unordered_map<std::string,int> funcMap_; //函数只用一个映射表即可
    std::vector<Symbol> symAllTab_;  // 离开作用域后，也要记录符号信息;
    int AllocSymInst(Symbol newSymInst_);  //插入一个符号，不关心这个符号具体是什么语义（变量、函数or其他）
    int RegistGLIBC(const std::string& name_,Types retType_);
public:
    ScopeManager() { scopes_.push_back({ScopeKind::GLOBAL,{}}); }
    Symbol& GetSymbol(int symID_);
    void EnterScope(ScopeKind kind);
    void ExitScope();
    bool InGlobal();
    bool InFunction();  //为插入参数做准备
    bool InDirectFunction(); //为return语句做准备，因为return可以发生在函数内的任意一个子block。
    void RegistAllStd();
    int SearchVar(const std::string& name_);
    int InsertVar(const std::string& name_,Types type_,int ptrLevel_);
    int SearchFunc(const std::string& name_);
    int InsertParam(const std::string& name_,Types type_,int ptrLevel_);
    int InsertFunc(const std::string& name_,Types retType_,const std::vector<Types>& paramTypes_,bool wantDefine);
};

class ScopeDaemon{
    ScopeKind kind_;
    ScopeManager& scopeMgr_;
public:
    ScopeDaemon(ScopeManager& scopeMgr,ScopeKind kind):scopeMgr_(scopeMgr),kind_(kind){
        scopeMgr_.EnterScope(kind_);
    }
    ~ScopeDaemon(){
        scopeMgr_.ExitScope();
    }

};