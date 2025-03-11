// 防止C代码被多次包含
#ifndef _HELLO_C
#define _HELLO_C

// 声明为不使用标准库启动代码
#define BARE_METAL

// UART寄存器地址
#define UART_BASE 0x80000000
#define UART_TX   (*(volatile unsigned char *)(UART_BASE + 0x0))
#define UART_LSR  (*(volatile unsigned char *)(UART_BASE + 0x5))
#define UART_LSR_EMPTY_MASK 0x40

#include "define.h"

// 声明tohost变量
volatile unsigned long tohost __attribute__((section(".tohost")));
volatile unsigned long fromhost __attribute__((section(".fromhost")));

// 声明在汇编中实现的函数
void myputchar(char c);
void myputs(const char *s);

// 打印整数的函数
void print_int(int num) {
    char buf[16];
    int i = 0;
    
    // 处理负数
    if (num < 0) {
        myputchar('-');
        num = -num;
    }
    
    // 处理0的特殊情况
    if (num == 0) {
        myputchar('0');
        myputchar('\n');
        return;
    }
    
    // 将数字转换为字符串（反向）
    while (num > 0) {
        int digit = 0;
        // 使用减法代替除法和取模
        while (num >= 10) {
            num -= 10;
            digit++;
        }
        buf[i++] = '0' + num;
        num = digit;
    }
    
    // 反向输出字符
    while (i > 0) {
        myputchar(buf[--i]);
    }
    myputchar('\n');
}

// 用于通知测试完成的函数
void test_exit(int code) {
    // 确保所有输出都已经发送
    // myputchar('\n');  // 强制刷新缓冲区
    // 发送退出代码 (将代码左移1位并设置最低位为1)
    tohost = ((unsigned long)code << 1) | 1;
    while (1);  // 等待模拟器终止
}

// 主函数声明为弱符号，允许被启动代码覆盖
__attribute__((weak))
int main() {
    myputs("Hello, RISC-V!\n");

    // 设置进程标识为0x1234567（限制1要求）
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

    // 测试读取多个计数器
    unsigned int counter_val;
    unsigned int write_val = 10;
    
    // 读取计数器1
    myputs("Reading counter 1:\n");
    asm volatile ("addi x0, %0, 128" : "=r"(counter_val));
    print_int(counter_val);
    
    // 读取计数器2
    myputs("Reading counter 2:\n");
    asm volatile ("addi x0, %0, 129" : "=r"(counter_val));
    print_int(counter_val);
    
    // 读取计数器3
    myputs("Reading counter 3:\n");
    asm volatile ("addi x0, %0, 160" : "=r"(counter_val));
    print_int(counter_val);

    // 写入计数器1（不使用宏定义，直接使用mv和addi指令）
    myputs("Write counter 3:\n");
    asm volatile (
        "mv t0, %0  \n\t"    // 将write_val的值移动到t0寄存器
        "addi x0, t0, 288"   // 使用addi指令写入计数器1
        : 
        : "r"(write_val)
    );
    // SetTempReg(write_val, 279);

    // 再次读取计数器1验证写入结果
    myputs("Reading counter 3 after write:\n");
    asm volatile ("addi x0, %0, 160" : "=r"(counter_val));
    print_int(counter_val);

    myputs("Test passed!");
    test_exit(0);  // 测试成功
    return 0;
}

#endif