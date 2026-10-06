#include"Scope.h"
#include"../AST/Node.h"
#include"../Tools/StdLib.h"
#include <cstddef>
#include <stdexcept>
#include <utility>
bool ScopeManager::InGlobal(){
    return scopes_.size()==1;
}

bool ScopeManager::InDirectFunction(){
    int back=scopes_.size()-1;
    if(scopes_[back].kind_!=ScopeKind::FUNC){
        return false;
    }
    else{
        return true;
    }
}

bool ScopeManager::InFunction(){
    bool res=false;
    for(int i=scopes_.size()-1;i>=1;i--){
        if(scopes_[i].kind_==ScopeKind::FUNC){
            res=true;
            break;
        }
    }
    return res;
}

Symbol& ScopeManager::GetSymbol(int symID_){
    if(symID_>=symAllTab_.size()|| symID_<0){
        throw std::logic_error("invalid symbol ID");
    }
    return symAllTab_[symID_];
}

void ScopeManager::EnterScope(ScopeKind kind){
    scopes_.push_back({kind,{}});
}

void ScopeManager::ExitScope(){
    if(scopes_.size()<=1){
        throw std::logic_error("can not exit global scope");
    }
    else{
        scopes_.pop_back();
    }
}

int ScopeManager::SearchVar(const std::string& name_){
    int scopeCnt=scopes_.size();
    for(int x=scopeCnt-1;x>=0;x--){
        if(scopes_[x].symMap_.find(name_)!=scopes_[x].symMap_.end()){
            return scopes_[x].symMap_[name_];
        }
    }
    return -1;  //没有找到
}

int ScopeManager::SearchFunc(const std::string& name_){
    if(funcMap_.find(name_)!=funcMap_.end()){
        return funcMap_[name_];
    }
    else{
        return -1;
    }
}

int ScopeManager::AllocSymInst(Symbol newSymInst_){
    //不检查重复，该函数为插入原语，检查工作应由上一层调用者完成
    newSymInst_.symID_=(int)(symAllTab_.size());
    symAllTab_.push_back(newSymInst_);
    return newSymInst_.symID_;
}

int ScopeManager::RegistGLIBC(const std::string& name_,Types retType_){
    if(funcMap_.find(name_)!=funcMap_.end()){
        throw std::logic_error("can not regist BUILD_IN function: ["+name_+"] again");
    }
    else{
        Symbol newFunc;
        newFunc.name_=name_;
        newFunc.type_=retType_;
        newFunc.kind_=SymKind::BUILD_IN;
        int insertResID=AllocSymInst(newFunc);
        this->funcMap_.emplace(name_,insertResID);
        return insertResID;
    }
}

void ScopeManager::RegistAllStd(){
    std::unordered_map<std::string,StdLib>::iterator iterHandler;
    int res;
    for(iterHandler=standardLibFunc.begin();iterHandler!=standardLibFunc.end();iterHandler++){
        res=RegistGLIBC(iterHandler->first,QWORD);
    }
}

int ScopeManager::InsertVar(const std::string& name_,Types type_,int ptrLevel_){
    ScopeFrame& curScope=scopes_[scopes_.size()-1];
    if(curScope.symMap_.find(name_)!=curScope.symMap_.end()){
        throw std::logic_error("symbol already defined in current scope: " + name_);
    }
    else{
        Symbol newVar;
        newVar.name_=name_;
        newVar.type_=type_;
        newVar.ptrLevel_=ptrLevel_;
        newVar.kind_=InGlobal()?SymKind::GLOBAL:SymKind::LOCAL;
        int insertResID=AllocSymInst(newVar);
        if(insertResID!=-1) {curScope.symMap_.emplace(name_,insertResID);}
        return insertResID;
    }
}

int ScopeManager::InsertParam(const std::string& name_,Types type_,int ptrLevel_){
    if(!InDirectFunction()){
        throw std::logic_error("can not insert parameter outside of function scope");
    }
    else{
        ScopeFrame& curScope=scopes_[scopes_.size()-1];
        if(curScope.symMap_.find(name_)!=curScope.symMap_.end()){
            throw std::logic_error("symbol already defined in current scope: " + name_);
        }
        else{
            Symbol newVar;
            newVar.name_=name_;
            newVar.type_=type_;
            newVar.ptrLevel_=ptrLevel_;
            newVar.kind_=SymKind::ARGS;
            int insertResID=AllocSymInst(newVar);
            if(insertResID!=-1) {curScope.symMap_.emplace(name_,insertResID);}
            return insertResID;
        }
    }
}

int ScopeManager::InsertFunc(const std::string& name_,Types retType_,const std::vector<Types>& paramTypes_,bool wantDefine){
    if(!this->InGlobal()) { return -1; }
    else{
        std::unordered_map<std::string,int>::iterator funcInst=funcMap_.find(name_);
        if(funcInst==funcMap_.end()){
            Symbol newFunc;
            newFunc.name_=name_;
            newFunc.type_=retType_;
            newFunc.paramTypes_=paramTypes_;
            newFunc.kind_=SymKind::FUNC;
            newFunc.defined_=wantDefine;
            int insertResID=AllocSymInst(newFunc);
            this->funcMap_.emplace(name_,insertResID);
            return insertResID;
        }
        else{
            Symbol existFunc=symAllTab_[funcInst->second];
            if(!wantDefine){  /*已经存在的函数，但只是声明 */
                if(existFunc.kind_==SymKind::BUILD_IN){
                    throw std::logic_error("can not redeclare BUILD_IN function"+name_);
                }
                if(existFunc.type_!=retType_){
                    throw std::logic_error("conflict return type of "+name_);
                }
                return existFunc.symID_;
            }
            else if(!symAllTab_[funcInst->second].defined_){
                symAllTab_[funcInst->second].defined_=true;
                return symAllTab_[funcInst->second].symID_;
            }
            else{  /*还想定义，无论如何都报错*/
                throw std::logic_error("can not redefined "+name_);
                exit(1);
            }
        }
    }
}

void NumberNode::ScopeParse(ScopeManager& manager_) {}
void CharNode::ScopeParse(ScopeManager& manager_) {}
void StringNode::ScopeParse(ScopeManager& manager_) {}

void VarDeclareNode::ScopeParse(ScopeManager& manager_){
    int varID=manager_.InsertVar(name_,type_,ptrLevel_);
    if(varID!=-1){
        symID_=varID;
    }
    else{
        throw std::logic_error("failed to declare variable: " + name_);
    }

    if(value_){
        value_->ScopeParse(manager_);
    }
}

void VarReferNode::ScopeParse(ScopeManager& manager_){
    symID_=manager_.SearchVar(name_);  //把符号ID绑定好
    if(symID_== -1){
        throw std::logic_error("symbol not found: "+name_);
    }
}

void UnaryOPNode::ScopeParse(ScopeManager& manager_){
    expr_->ScopeParse(manager_);
}

void BinaryOPNode::ScopeParse(ScopeManager& manager_){
    left_->ScopeParse(manager_);
    right_->ScopeParse(manager_);
}

void AssignStmtNode::ScopeParse(ScopeManager& manager_){
    leftValue_->ScopeParse(manager_);
    rightValue_->ScopeParse(manager_);
}

void StmtLists::ScopeParse(ScopeManager& manager_){
    for(int x=0;x<stmts_.size();x++){
        stmts_[x]->ScopeParse(manager_);
    }
}

void LegalExprStmtNode::ScopeParse(ScopeManager& manager_){
    indepExpr_->ScopeParse(manager_);
}

void BlockNode::ScopeParseOnly(ScopeManager& scopeMgr_){
    for(int x=0;x<stmts_.size();x++){
        stmts_[x]->ScopeParse(scopeMgr_);
        if(stmts_[x]->PromiseReturn() && x!= stmts_.size()-1){
            throw std::logic_error("still have code after return statement");
        }
    }
}

void BlockNode::ScopeParse(ScopeManager& scopeMgr_){
    ScopeDaemon scopeD(scopeMgr_,ScopeKind::BLOCK);
    ScopeParseOnly(scopeMgr_);
}

void IfElseNode::ScopeParse(ScopeManager& scopeMgr_){
    condition_->ScopeParse(scopeMgr_);
    ifBlock_->ScopeParse(scopeMgr_);
    if(elseBlock_){
        elseBlock_->ScopeParse(scopeMgr_);
    }
}

void FuncDefineNode::ScopeParse(ScopeManager& scopeMgr_){
    int paramCnt=paramList_.size();
    std::vector<Types> paramTypes(paramCnt);
    for(int i=0;i<paramCnt;i++){
        paramTypes[i]=paramList_[i].first;
    }
    this->symID_=scopeMgr_.InsertFunc(funcName_,retType_,paramTypes,funcBody_!=NULL);
    if(funcBody_==NULL) {return;}
    else{
        ScopeDaemon scopeD(scopeMgr_,ScopeKind::FUNC);
        paramSymID_.resize(paramCnt);
        for(int i=0;i<paramCnt;i++){
            paramSymID_[i]=scopeMgr_.InsertParam(paramList_[i].second,paramList_[i].first,0);  //目前没有记录参数的指针级别，先用0过渡一下
        }
        funcBody_->ScopeParseOnly(scopeMgr_);
    }
}

void FuncCallNode::ScopeParse(ScopeManager& scopeMgr_){
    symID_=scopeMgr_.SearchFunc(funcName_);
    if(symID_==-1){
        throw std::logic_error("function not found: "+funcName_);
    }
    for(int i=0;i<paramList_.size();i++){
        paramList_[i]->ScopeParse(scopeMgr_);
    }
    /*现在还缺少函数调用时的检查：参数数量、参数类型等*/
}

void ReturnNode::ScopeParse(ScopeManager& scopeMgr_){
    if(!scopeMgr_.InFunction()){
        throw std::logic_error("return statement must be in function scope");
    }
    if(retValue_){
        retValue_->ScopeParse(scopeMgr_);
    }
}