#include<cstdio>
#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include"Generator/IRGenerate.h"
// Target 相关（核心）
#include"llvm/Support/TargetSelect.h"
#include"llvm/Target/TargetMachine.h"
#include"llvm/MC/TargetRegistry.h"

// 输出文件
#include"llvm/Support/FileSystem.h"
#include"llvm/Support/raw_ostream.h"

// PassManager
#include"llvm/IR/LegacyPassManager.h"

// Target triple
#include"llvm/TargetParser/Host.h"
#include"./AST/Node.h"
#include"./Tools/StdLib.h"
extern int yyparse();
extern StmtLists program;
extern FILE* yyin;
FILE* ParseCLI(int cliCount,char* argv[]){
    if(cliCount==3){
        FILE* srcFile=fopen(argv[1],"r");
        if(!srcFile){
            std::cout<<"source file error"<<std::endl;
            exit(1);
        }
        else{
            return srcFile;
        }
    }
    else{
        std::cout<<"argc error"<<std::endl;
        exit(1);
    }
}

void Assembler(const X4A_Ctx& context,const char* dstFile,bool& linkTag){
    std::string dstFileName(dstFile);
    if(dstFileName.substr(dstFileName.size()-2,2)!=".o"){
        dstFileName+=".o";
        linkTag=true;
    }
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::Triple archInfo(llvm::sys::getDefaultTargetTriple());
    context.llvmModule_->setTargetTriple(archInfo);
    std::string error;
    const llvm::Target* target=llvm::TargetRegistry::lookupTarget(archInfo,error);
    if(!target){
        std::cout<<"Target error: "<<error<<std::endl;
        exit(1); 
    }
    std::string cpu("generic");
    std::string features("");
    llvm::TargetOptions opt;

    llvm::TargetMachine *targetMachine =target->createTargetMachine(archInfo, cpu, features, opt, llvm::Reloc::PIC_);

    // 4. 设置 DataLayout
    context.llvmModule_->setDataLayout(targetMachine->createDataLayout());

    // 5. 输出文件
    std::error_code err_code;
    llvm::raw_fd_ostream dest(dstFileName.c_str(), err_code, llvm::sys::fs::OF_None);

    if (err_code) {
        llvm::errs() << "Could not open file: " << err_code.message();
        return;
    }

    llvm::legacy::PassManager llvmPass;

    if (targetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        llvm::errs() << "TargetMachine can't emit object file";
        return;
    }

    // 6. 运行 Pass
    llvmPass.run(*context.llvmModule_);

    dest.flush();
}

void Linker(const char* dstFile){
    std::string shellCmd="clang "+std::string(dstFile)+".o"+" -o "+std::string(dstFile);
    system(shellCmd.c_str());
    return;
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

        //创建主函数
        llvm::Function* mainFunc = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(*context.llvmContext_),false),llvm::Function::ExternalLinkage,"main",context.llvmModule_.get());
        llvm::BasicBlock* mainEntry = llvm::BasicBlock::Create(*context.llvmContext_,"entry",mainFunc);
        context.llvmBuilder_->SetInsertPoint(mainEntry);
        program.IRGenerate(context); //生成IR
        //创建返回指令
        context.llvmBuilder_->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context.llvmContext_), 0));
        context.llvmModule_->print(llvm::outs(), NULL);  //打印输出IR，用于前期调试
        bool linkTag=false;
        Assembler(context, argv[2],linkTag); //生成目标文件
        if(linkTag){
            Linker(argv[2]);
        }
        fclose(yyin);
    }
    else{
        fclose(yyin);
        exit(1);
    }
}