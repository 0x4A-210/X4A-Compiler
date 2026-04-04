#include"Node.h"
#include<cstdio>
#include<iostream>
#include<fstream>
extern int yyparse();
extern StmtListNode program;
extern FILE* yyin;
int main(int argc,char* argv[]){
    if(argc==1){
        std::cout<<"interactive mode"<<std::endl;
        yyin =stdin;
        int res=yyparse();
        if(!res){
            program.ShowASTNode();
            return 0;
        }
        else{
            std::cout<<"AST Parse Error"<<std::endl;
            return 1;
        }
    }
    else if(argc==2){
        std::cout<<"Read file mode"<<std::endl;
        FILE* src_file=fopen(argv[1],"r");
        if(!src_file){
            std::cout<<"source file error"<<std::endl;
            return 1;
        }
        else{
            yyin=src_file;
        }
        int res=yyparse();
        if(!res){
            program.ShowASTNode();
            return 0;
        }
        else{
            std::cout<<"AST Parse Error"<<std::endl;
            return 1;
        }
    }
    else{
        std::cout<<"argc error"<<std::endl;
        return 1;
    }
}