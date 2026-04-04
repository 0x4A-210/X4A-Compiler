cd /home/k40/LLVM/build
if [[ "$1" == "all" ]]; then
    cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_INCLUDE_TESTS=ON -DLLVM_TARGETS_TO_BUILD="X86" -G "Unix Makefiles" ../llvm
    make
elif [[ "$1" == "patch" ]]; then
    cmake ../llvm
    make
else
    echo "Usage: $0 [all|patch]"
fi