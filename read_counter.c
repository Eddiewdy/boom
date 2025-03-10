#include <stdio.h>
#include <stdint.h>
#include "define.h"

// 定义uint64_t类型（修复define.h中的错误）
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

// 定义tempStackMem数组（修复define.h中的错误）
uint64_t tempStackMem[4096];

// 示例函数，用于测试性能计数器
void test_function() {
    // 一些计算操作，用于生成性能事件
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
}

int main() {
    uint64_t counter_values[16];
    uint64_t tag, exit_addr, max_events, now_events, event_sel, max_priv;
    uint64_t sample_happen;
    
    // 1. 设置性能计数器的基本信息
    SetSampleBaseInfo(1, (uint64_t)&main); // 设置进程标签和采样函数地址
    
    // 2. 设置性能计数器的控制寄存器
    // 参数：最大事件数，预热指令数，事件选择器（0表示第一个计数器）
    SetSampleCtrlReg(100, 10, 0);
    
    // 3. 设置计数器特权级别（0=用户模式）
    SetCounterLevel("0");
    
    // 4. 重置计数器
    RESET_COUNTER;
    
    // 5. 获取当前设置信息
    GetInformation(tag, exit_addr, max_events, now_events, event_sel, max_priv);
    printf("性能计数器配置信息：\n");
    printf("  进程标签: %llu\n", tag);
    printf("  采样函数地址: 0x%llx\n", exit_addr);
    printf("  最大事件数: %llu\n", max_events);
    printf("  当前事件数: %llu\n", now_events);
    printf("  事件选择器: %llu\n", event_sel);
    printf("  最大特权级别: %llu\n", max_priv);
    
    // 6. 执行测试函数
    test_function();
    
    // 7. 读取第一个性能计数器（使用ReadCounter8宏）
    ReadCounter8(counter_values, 0);
    printf("\n使用ReadCounter8读取性能计数器：\n");
    for (int i = 0; i < 8; i++) {
        printf("  计数器[%d]: %llu\n", i, counter_values[i]);
    }
    
    // 8. 读取更多计数器（使用ReadCounter16宏）
    ReadCounter16(counter_values, 0);
    printf("\n使用ReadCounter16读取性能计数器：\n");
    for (int i = 0; i < 16; i++) {
        printf("  计数器[%d]: %llu\n", i, counter_values[i]);
    }
    
    // 9. 使用CSR读取cycle和instret计数器
    uint64_t cycle_count = read_csr_cycle();
    uint64_t instret_count = read_csr_instret();
    printf("\n使用CSR读取计数器：\n");
    printf("  周期计数器: %llu\n", cycle_count);
    printf("  指令计数器: %llu\n", instret_count);
    
    // 10. 检查采样是否发生
    GetSampleHappen(sample_happen);
    printf("\n采样发生状态: %s\n", sample_happen ? "是" : "否");
    
    return 0;
} 