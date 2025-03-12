#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// 定义内存分配阈值，超过此阈值的内存分配将被记录到性能计数器
#define MEMORY_THRESHOLD (1024) // 1KB

// 用于写入性能计数器的函数
static inline void write_counter(unsigned int counter_idx, unsigned long value) {
// #ifdef __riscv
    // RISC-V架构下的实现
    // 根据计数器索引选择不同的指令
    switch (counter_idx) {
        case 32:
            asm volatile ("addi x0, %0, 288" : : "r"(value)); break;
        case 33:
            asm volatile ("addi x0, %0, 289" : : "r"(value)); break;
        case 34:
            asm volatile ("addi x0, %0, 290" : : "r"(value)); break;
        case 35:
            asm volatile ("addi x0, %0, 291" : : "r"(value)); break;
        case 36:
            asm volatile ("addi x0, %0, 292" : : "r"(value)); break;
        case 37:
            asm volatile ("addi x0, %0, 293" : : "r"(value)); break;
        case 38:
            asm volatile ("addi x0, %0, 294" : : "r"(value)); break;
        case 39:
            asm volatile ("addi x0, %0, 295" : : "r"(value)); break;
        case 40:
            asm volatile ("addi x0, %0, 296" : : "r"(value)); break;
        case 41:
            asm volatile ("addi x0, %0, 297" : : "r"(value)); break;
        case 42:
            asm volatile ("addi x0, %0, 298" : : "r"(value)); break;
        case 43:
            asm volatile ("addi x0, %0, 299" : : "r"(value)); break;
        case 44:
            asm volatile ("addi x0, %0, 300" : : "r"(value)); break;
        case 45:
            asm volatile ("addi x0, %0, 301" : : "r"(value)); break;
        case 46:
            asm volatile ("addi x0, %0, 302" : : "r"(value)); break;
        case 47:
            asm volatile ("addi x0, %0, 303" : : "r"(value)); break;
        case 48:
            asm volatile ("addi x0, %0, 304" : : "r"(value)); break;
        case 49:
            asm volatile ("addi x0, %0, 305" : : "r"(value)); break;
        case 50:
            asm volatile ("addi x0, %0, 306" : : "r"(value)); break;
        case 51:
            asm volatile ("addi x0, %0, 307" : : "r"(value)); break;
        case 52:
            asm volatile ("addi x0, %0, 308" : : "r"(value)); break;
        case 53:
            asm volatile ("addi x0, %0, 309" : : "r"(value)); break;
        case 54:
            asm volatile ("addi x0, %0, 310" : : "r"(value)); break;
        case 55:
            asm volatile ("addi x0, %0, 311" : : "r"(value)); break;
        case 56:
            asm volatile ("addi x0, %0, 312" : : "r"(value)); break;
        case 57:
            asm volatile ("addi x0, %0, 313" : : "r"(value)); break;
        case 58:
            asm volatile ("addi x0, %0, 314" : : "r"(value)); break;
        case 59:
            asm volatile ("addi x0, %0, 315" : : "r"(value)); break;
        case 60:
            asm volatile ("addi x0, %0, 316" : : "r"(value)); break;
        case 61:
            asm volatile ("addi x0, %0, 317" : : "r"(value)); break;
        case 62:
            asm volatile ("addi x0, %0, 318" : : "r"(value)); break;
        case 63:
            asm volatile ("addi x0, %0, 319" : : "r"(value)); break;
        default:
            fprintf(stderr, "Error: Counter index %d out of range (32-63)\n", counter_idx);
            break;
    }
// #else
    // 在非RISC-V架构下，仅打印信息但不执行实际操作
    // fprintf(stderr, "[Simulated] Writing value 0x%lx to counter %d\n", value, counter_idx);
// #endif
}

// 用于读取性能计数器的函数
static inline unsigned long read_counter(unsigned int counter_idx) {
    unsigned long value = 0;
// #ifdef __riscv
    // RISC-V架构下的实现
    // 根据计数器索引选择不同的指令
    switch (counter_idx) {
        case 32:
            asm volatile ("addi x0, %0, 160" : "=r"(value)); break;
        case 33:
            asm volatile ("addi x0, %0, 161" : "=r"(value)); break;
        case 34:
            asm volatile ("addi x0, %0, 162" : "=r"(value)); break;
        case 35:
            asm volatile ("addi x0, %0, 163" : "=r"(value)); break;
        case 36:
            asm volatile ("addi x0, %0, 164" : "=r"(value)); break;
        case 37:
            asm volatile ("addi x0, %0, 165" : "=r"(value)); break;
        case 38:
            asm volatile ("addi x0, %0, 166" : "=r"(value)); break;
        case 39:
            asm volatile ("addi x0, %0, 167" : "=r"(value)); break;
        case 40:
            asm volatile ("addi x0, %0, 168" : "=r"(value)); break;
        case 41:
            asm volatile ("addi x0, %0, 169" : "=r"(value)); break;
        case 42:
            asm volatile ("addi x0, %0, 170" : "=r"(value)); break;
        case 43:
            asm volatile ("addi x0, %0, 171" : "=r"(value)); break;
        case 44:
            asm volatile ("addi x0, %0, 172" : "=r"(value)); break;
        case 45:
            asm volatile ("addi x0, %0, 173" : "=r"(value)); break;
        case 46:
            asm volatile ("addi x0, %0, 174" : "=r"(value)); break;
        case 47:
            asm volatile ("addi x0, %0, 175" : "=r"(value)); break;
        case 48:
            asm volatile ("addi x0, %0, 176" : "=r"(value)); break;
        case 49:
            asm volatile ("addi x0, %0, 177" : "=r"(value)); break;
        case 50:
            asm volatile ("addi x0, %0, 178" : "=r"(value)); break;
        case 51:
            asm volatile ("addi x0, %0, 179" : "=r"(value)); break;
        case 52:
            asm volatile ("addi x0, %0, 180" : "=r"(value)); break;
        case 53:
            asm volatile ("addi x0, %0, 181" : "=r"(value)); break;
        case 54:
            asm volatile ("addi x0, %0, 182" : "=r"(value)); break;
        case 55:
            asm volatile ("addi x0, %0, 183" : "=r"(value)); break;
        case 56:
            asm volatile ("addi x0, %0, 184" : "=r"(value)); break;
        case 57:
            asm volatile ("addi x0, %0, 185" : "=r"(value)); break;
        case 58:
            asm volatile ("addi x0, %0, 186" : "=r"(value)); break;
        case 59:
            asm volatile ("addi x0, %0, 187" : "=r"(value)); break;
        case 60:
            asm volatile ("addi x0, %0, 188" : "=r"(value)); break;
        case 61:
            asm volatile ("addi x0, %0, 189" : "=r"(value)); break;
        case 62:
            asm volatile ("addi x0, %0, 190" : "=r"(value)); break;
        case 63:
            asm volatile ("addi x0, %0, 191" : "=r"(value)); break;
        default:
            fprintf(stderr, "Error: Counter index %d out of range (32-63)\n", counter_idx);
            break;
    }
// #else
    // 在非RISC-V架构下，返回模拟值
    // 为了测试，我们假设所有计数器初始值为0
// #endif
    return value;
}

// 在后32个性能计数器中查找空闲的计数器
// 返回找到的第一对空闲计数器的索引，如果没有找到则返回-1
static int find_free_counter_pair() {
    // 从第32个计数器开始查找
    for (int i = 32; i < 64; i += 2) {
        // fprintf(stderr, "find_free_counter_pair read_counter(%d) = %lx\n", i, read_counter(i));
        if (read_counter(i) == 0 && read_counter(i+1) == 0) {
            return i;
        }
    }
    return -1;
}

// 在后32个性能计数器中查找指定地址的计数器对
// 返回找到的计数器索引，如果没有找到则返回-1
static int find_address_counter(void *ptr) {
    // 从第32个计数器开始查找
    for (int i = 32; i < 64; i += 2) {
        // fprintf(stderr, "find_address_counter read_counter(%d) = %lx\n", i, read_counter(i));
        if (read_counter(i) == (unsigned long)ptr) {
            return i;
        }
    }
    return -1;
}

void *malloc(size_t size) {
    static void *(*real_malloc)(size_t) = NULL;
    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            fprintf(stderr, "Error: dlsym failed to find malloc\n");
            exit(1);
        }
    }

    // 防止在 printf 内部调用 malloc 造成递归
    void *ptr = real_malloc(size);
    fprintf(stderr, "[Intercepted] malloc(%zu) = %p\n", size, ptr);
    
    // 如果分配的内存大小超过阈值，则尝试记录到性能计数器
    if (size > MEMORY_THRESHOLD) {
        int counter_idx = find_free_counter_pair();
        if (counter_idx != -1) {
            // 将地址存入第一个计数器，大小存入第二个计数器
            write_counter(counter_idx, (unsigned long)ptr);
            write_counter(counter_idx + 1, (unsigned long)size);
            fprintf(stderr, "[Recorded] Large allocation: %p (size: %zu) in counters %d and %d\n", 
                    ptr, size, counter_idx, counter_idx + 1);
        } else {
            fprintf(stderr, "[Warning] No free counter pairs available for recording large allocation\n");
        }
    }
    
    return ptr;
}

void free(void *ptr) {
    static void (*real_free)(void *) = NULL;
    if (!real_free) {
        real_free = dlsym(RTLD_NEXT, "free");
        if (!real_free) {
            fprintf(stderr, "Error: dlsym failed to find free\n");
            exit(1);
        }
    }
    
    // 检查是否需要清除性能计数器
    if (ptr != NULL) {
        int counter_idx = find_address_counter(ptr);
        if (counter_idx != -1) {
            // 清除计数器对
            fprintf(stderr, "[Clearing] Counters %d and %d for address %p\n", 
                    counter_idx, counter_idx + 1, ptr);
            write_counter(counter_idx, 0);
            write_counter(counter_idx + 1, 0);
        }
    }
    
    fprintf(stderr, "[Intercepted] free(%p)\n", ptr);
    real_free(ptr);
}
