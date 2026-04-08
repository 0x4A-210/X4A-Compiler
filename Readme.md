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
```bash
./bin/XAC ./test/main.xa
```
## TODO
已经可以完美生成目标文件（.o），接下来考虑支持函数调用语句、分支循环控制流，并最终实现输出hello world
# 语法规定
## 支持的数据类型：
- char类型，同C
- Num64数值，即QWORD，64位整数
- Num32数值，即DWORD，32位整数
- Num16数值，即WORD，16位整数
- Num8数值，即BYTE，8位整数
- Point，即指针，64位
## 终止符
同C/C++，每条语句以;结尾