#include <stdio.h>
#include <stdlib.h>

// 添加读取计数器的函数
static inline unsigned long read_counter(unsigned int counter_idx) {
    unsigned long value = 0;
    
    // 使用switch语句为每个计数器索引提供正确的立即数
    switch (counter_idx) {
        case 32: asm volatile ("addi x0, %0, 160" : "=r"(value)); break;
        case 33: asm volatile ("addi x0, %0, 161" : "=r"(value)); break;
        case 34: asm volatile ("addi x0, %0, 162" : "=r"(value)); break;
        case 35: asm volatile ("addi x0, %0, 163" : "=r"(value)); break;
        case 36: asm volatile ("addi x0, %0, 164" : "=r"(value)); break;
        case 37: asm volatile ("addi x0, %0, 165" : "=r"(value)); break;
        case 38: asm volatile ("addi x0, %0, 166" : "=r"(value)); break;
        case 39: asm volatile ("addi x0, %0, 167" : "=r"(value)); break;
        case 40: asm volatile ("addi x0, %0, 168" : "=r"(value)); break;
        case 41: asm volatile ("addi x0, %0, 169" : "=r"(value)); break;
        case 42: asm volatile ("addi x0, %0, 170" : "=r"(value)); break;
        case 43: asm volatile ("addi x0, %0, 171" : "=r"(value)); break;
        case 44: asm volatile ("addi x0, %0, 172" : "=r"(value)); break;
        case 45: asm volatile ("addi x0, %0, 173" : "=r"(value)); break;
        case 46: asm volatile ("addi x0, %0, 174" : "=r"(value)); break;
        case 47: asm volatile ("addi x0, %0, 175" : "=r"(value)); break;
        case 48: asm volatile ("addi x0, %0, 176" : "=r"(value)); break;
        case 49: asm volatile ("addi x0, %0, 177" : "=r"(value)); break;
        case 50: asm volatile ("addi x0, %0, 178" : "=r"(value)); break;
        case 51: asm volatile ("addi x0, %0, 179" : "=r"(value)); break;
        case 52: asm volatile ("addi x0, %0, 180" : "=r"(value)); break;
        case 53: asm volatile ("addi x0, %0, 181" : "=r"(value)); break;
        case 54: asm volatile ("addi x0, %0, 182" : "=r"(value)); break;
        case 55: asm volatile ("addi x0, %0, 183" : "=r"(value)); break;
        case 56: asm volatile ("addi x0, %0, 184" : "=r"(value)); break;
        case 57: asm volatile ("addi x0, %0, 185" : "=r"(value)); break;
        case 58: asm volatile ("addi x0, %0, 186" : "=r"(value)); break;
        case 59: asm volatile ("addi x0, %0, 187" : "=r"(value)); break;
        case 60: asm volatile ("addi x0, %0, 188" : "=r"(value)); break;
        case 61: asm volatile ("addi x0, %0, 189" : "=r"(value)); break;
        case 62: asm volatile ("addi x0, %0, 190" : "=r"(value)); break;
        case 63: asm volatile ("addi x0, %0, 191" : "=r"(value)); break;
        default: 
            printf("Error: Counter index %d out of range (32-63)\n", counter_idx);
            break;
    }
    
    return value;
}

int main() {
    // 设置进程标识为0x1234567
    unsigned int proc_tag = 0x1234567;
    asm volatile ("addi x0, %0, 1" : : "r"(proc_tag));

    // 设置统计所有级别的事件
    unsigned int priv_level = 3;  // 3表示统计所有级别
    asm volatile ("addi x0, %0, 6" : : "r"(priv_level));

    // 启用计数器
    unsigned int enable = 1;
    asm volatile ("addi x0, %0, 7" : : "r"(enable));

    // 重置所有计数器
    asm volatile ("addi x0, x0, 127");

    printf("Allocating memory...\n");
    
    // 分配大于1MB的内存，以触发我们在malloc_wrapper中设置的阈值
    size_t large_size = 2 * 1024 * 1024; // 2MB
    printf("Allocating %zu bytes of memory...\n", large_size);
    int *arr = (int *)malloc(large_size);
    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // 在malloc之后打印32-64的计数器值
    printf("\n--- 计数器状态 (malloc之后) ---\n");
    for (int i = 32; i < 64; i++) {
        unsigned long counter_value = read_counter(i);
        printf("计数器[%d] = 0x%lx\n", i, counter_value);
    }
    printf("----------------------------\n\n");

    for (int i = 0; i < 10; i++) {
        arr[i] = i;
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    printf("Memory freed.\n");

    // 在free之后打印32-64的计数器值
    printf("\n--- 计数器状态 (free之后) ---\n");
    for (int i = 32; i < 64; i++) {
        unsigned long counter_value = read_counter(i);
        printf("计数器[%d] = 0x%lx\n", i, counter_value);
    }
    printf("----------------------------\n");

    return 0;
}
