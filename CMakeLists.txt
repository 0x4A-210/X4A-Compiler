# 最低CMake版本要求
cmake_minimum_required(VERSION 3.16)
project(X4A LANGUAGES CXX)

# ==============================================
# 1. 强制Debug编译，保证-g调试信息
# ==============================================
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# ==============================================
# 2. 查找并配置LLVM
# ==============================================
find_package(LLVM REQUIRED CONFIG)
message(STATUS "[√] 找到LLVM版本: ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

# ==============================================
# 3. 查找Bison和Flex工具
# ==============================================
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# ==============================================
# 4. 设置可执行文件输出目录（根目录下的bin）
# ==============================================
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
# ==============================================
# 5. 处理Bison语法分析器
# ==============================================
BISON_TARGET(
    XAC_Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bison/yacc.y
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bison/yacc.cpp
    DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/bison/yacc.hpp
    COMPILE_FLAGS "-d"
)

# ==============================================
# 6. 处理Flex词法分析器
# ==============================================
FLEX_TARGET(
    XAC_Scanner
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flex/lexical.l
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flex/lexical.cpp
)

# ==============================================
# 7. 关联Flex和Bison依赖
# ==============================================
ADD_FLEX_BISON_DEPENDENCY(XAC_Scanner XAC_Parser)

# ==============================================
# 8. 收集源文件
# ==============================================
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AST/Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Generator/IRGenerate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Runtime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/StdLib.cpp
    ${BISON_XAC_Parser_OUTPUTS}
    ${FLEX_XAC_Scanner_OUTPUTS}
)

# ==============================================
# 9. 生成可执行文件
# ==============================================
add_executable(XAC ${SOURCE_FILES})

# ==============================================
# 10. 配置头文件包含目录
# ==============================================
target_include_directories(XAC PRIVATE
    ${PROJECT_SOURCE_DIR}/src/AST
    ${PROJECT_SOURCE_DIR}/src/bison
    ${PROJECT_SOURCE_DIR}/src/Generator
    ${PROJECT_SOURCE_DIR}/src/Tools
    ${LLVM_INCLUDE_DIRS}
)

# ==============================================
# 11. 链接LLVM core库
# ==============================================
llvm_map_components_to_libnames(LLVM_LIBS core support target mc nativecodegen)
target_link_libraries(XAC PRIVATE ${LLVM_LIBS})

# ==============================================
# 12. 配置C++标准
# ==============================================
target_compile_features(XAC PRIVATE cxx_std_17)
target_compile_options(XAC PRIVATE -g)