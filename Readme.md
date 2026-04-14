# 简介
一个基于LLVM设计的，学习用的类C语言及Toy Compiler
# 快速使用
首先克隆本项目
## 安装LLVM
```bash
git clone https://github.com/llvm/llvm-project.git
```
将本项目中Make文件夹中的LLVM-Build.sh拷贝到llvm-project，执行：
```bash
mkdir build && cd build
../LLVM-Build.sh
```
## 编译本项目
在本项目根目录下执行
```bash
./build.sh
```
## 使用
目前只能生成.o目标文件，链接请自行使用链接器完成
```bash
./bin/XAC ./test/main.xa ./bin/main.o
```
## TODO
- 目前可实现函数声明（声明同时定义，或者仅声明）、函数调用、分支控制、变量声明（声明同时赋值，或者仅赋值）、链接C的printf来输出、变量赋值、变量引用、加减乘除、大于小于等于，且数据类型支持：char、64、32、16、8位的整数以及字符串；<br>
- 下一步计划实现指针类型，并链接C的scanf完成输入功能
# 语法规定
## 支持的数据类型：
- char类型，同C
- Num64数值，即QWORD，64位整数
- Num32数值，即DWORD，32位整数
- Num16数值，即WORD，16位整数
- Num8数值，即BYTE，8位整数
- String，字符串
## 终止符
同C/C++，每条语句以;结尾