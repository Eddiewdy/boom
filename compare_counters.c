#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "define.h"

// 定义uint64_t类型（修复define.h中的错误）
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

// 定义tempStackMem数组（修复define.h中的错误）
uint64_t tempStackMem[4096];

/**
 * 性能计数器比较程序
 * 
 * 这个程序展示了如何读取和比较RISC-V处理器的不同性能计数器
 * 包括自定义计数器和标准CSR计数器
 */

// 执行计算密集型操作
void compute_intensive() {
    volatile double result = 0.0;
    for (int i = 0; i < 10000; i++) {
        result += i * 1.1;
    }
}

// 执行内存密集型操作
void memory_intensive() {
    volatile int array[1000];
    for (int i = 0; i < 1000; i++) {
        array[i] = i;
    }
    for (int i = 0; i < 1000; i++) {
        array[i] += array[(i + 500) % 1000];
    }
}

// 测量并打印性能计数器
void measure_and_print(const char* test_name) {
    uint64_t counter_values[8];
    uint64_t cycle_before, cycle_after;
    uint64_t instret_before, instret_after;
    
    // 读取CSR计数器（开始）
    cycle_before = read_csr_cycle();
    instret_before = read_csr_instret();
    
    // 重置自定义计数器
    RESET_COUNTER;
    
    // 执行测试函数
    if (strcmp(test_name, "计算密集型") == 0) {
        compute_intensive();
    } else if (strcmp(test_name, "内存密集型") == 0) {
        memory_intensive();
    }
    
    // 读取CSR计数器（结束）
    cycle_after = read_csr_cycle();
    instret_after = read_csr_instret();
    
    // 读取自定义计数器
    ReadCounter8(counter_values, 0);
    
    // 打印结果
    printf("\n===== %s测试 =====\n", test_name);
    printf("第一个性能计数器值: %llu\n", counter_values[0]);
    printf("周期计数器差值: %llu\n", cycle_after - cycle_before);
    printf("指令计数器差值: %llu\n", instret_after - instret_before);
    printf("每指令周期数(CPI): %.2f\n", 
           (double)(cycle_after - cycle_before) / (instret_after - instret_before));
}

int main() {
    // 配置性能计数器
    SetSampleBaseInfo(1, (uint64_t)&main);
    
    // 设置事件选择器为0（第一个计数器）
    SetSampleCtrlReg(100, 10, 0);
    
    // 设置计数器特权级别（0=用户模式）
    SetCounterLevel("0");
    
    // 启用性能计数器
    SetPfcEnable(1);
    
    // 测量计算密集型操作
    measure_and_print("计算密集型");
    
    // 测量内存密集型操作
    measure_and_print("内存密集型");
    
    return 0;
} 