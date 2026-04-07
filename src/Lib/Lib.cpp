#include<cstdio>
#include<iostream>
#include<fstream>
#include"../Generator/IRGenerate.h"
#include"../AST/Node.h"
extern int yyparse();
extern StmtLists program;
extern FILE* yyin;
FILE* ParseCLI(int cliCount,char* argv[]){
    if(cliCount==2){
        FILE* src_file=fopen(argv[1],"r");
        if(!src_file){
            std::cout<<"source file error"<<std::endl;
            exit(1);
        }
        else{
            return src_file;
        }
    }
    else{
        std::cout<<"argc error"<<std::endl;
        exit(1);
    }
}

void X4A_Run(int cliCount,char* argv[]){
    yyin=ParseCLI(cliCount,argv);
    int res=yyparse();
    if(!res){
        std::cout<<"Parsing success!"<<std::endl;
        X4A_Ctx context;
        context.llvmContext_=std::make_unique<llvm::LLVMContext>();
        context.llvmModule_=std::make_unique<llvm::Module>("X4A_Module",*context.llvmContext_);
        context.llvmBuilder_=std::make_unique<llvm::IRBuilder<>>(*context.llvmContext_);

        llvm::Function* mainFunc = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(*context.llvmContext_),false),llvm::Function::ExternalLinkage,"main",context.llvmModule_.get());
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context.llvmContext_,"entry",mainFunc);
        context.llvmBuilder_->SetInsertPoint(entry);
        program.IRGenerate(context);
        context.llvmBuilder_->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context.llvmContext_), 0));
        context.llvmModule_->print(llvm::outs(), NULL);
        fclose(yyin);
    }
    else{
        std::cout<<"Parsing failed!"<<std::endl;
        fclose(yyin);
        exit(1);
    }
}