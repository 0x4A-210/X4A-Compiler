cd ./src
bison -d -o ./bison/yacc.cpp ./bison/yacc.y
flex -o ./flex/lexical.cpp ./flex/lexical.l
g++ -g -o ../bin/XAC ./bison/yacc.cpp ./flex/lexical.cpp main.cpp Node.cpp `llvm-config --cxxflags --ldflags --libs core`