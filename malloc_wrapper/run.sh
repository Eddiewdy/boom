#!/bin/bash

# 设置RISC-V交叉编译器
RISCV_GCC=riscv64-unknown-linux-gnu-gcc
RISCV_GXX=riscv64-unknown-linux-gnu-g++

# 编译测试程序
$RISCV_GXX test.cpp -o test

# 编译malloc包装器
$RISCV_GCC -shared -fPIC -o mymalloc.so malloc_wrapper.c -ldl

# 运行程序（需要在RISC-V环境或模拟器中执行）
# LD_PRELOAD=$(pwd)/mymalloc.so ./test