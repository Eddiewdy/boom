#include <stdio.h>
#include <stdint.h>
#include "define.h"

// 定义uint64_t类型（修复define.h中的错误）
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

// 定义tempStackMem数组（修复define.h中的错误）
uint64_t tempStackMem[4096];

/**
 * 读取第一个性能计数器的简单示例
 * 
 * 这个程序展示了如何读取RISC-V处理器的第一个性能计数器
 * 基于define.h中定义的宏和core.scala中的实现
 */
int main() {
    uint64_t counter_value = 0;
    
    // 1. 重置计数器
    RESET_COUNTER;
    
    // 2. 执行一些指令，生成性能事件
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    
    // 3. 读取第一个性能计数器（从0开始）
    // 使用内联汇编直接读取第一个计数器到变量
    asm volatile(
        "addi x0, %0, 128+0+0 \n\t"  // 读取第一个计数器（索引0）
        : "=r"(counter_value)
        :
    );
    
    // 4. 打印第一个计数器的值
    printf("第一个性能计数器的值: %llu\n", counter_value);
    
    return 0;
} 