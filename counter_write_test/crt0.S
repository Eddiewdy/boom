.section .text.init
.global _start
.global myputchar
.global myputs

# 数据段：缓冲区和长度
.section .data
.align 3
buffer:
    .zero 64    # 64字节的缓冲区
buflen:
    .word 0     # 缓冲区当前长度

.section .text.init
_start:
    # 初始化栈指针
    la sp, _stack_top

    # 清零bss段
    la t0, _bss_start
    la t1, _bss_end
bss_clear:
    sw zero, (t0)
    addi t0, t0, 4
    blt t0, t1, bss_clear

    # 清零tohost
    la t0, tohost
    sw zero, (t0)
    sw zero, 4(t0)

    # 跳转到main函数
    call main

1:  j 1b      # 无限循环

# 打印单个字符的函数
myputchar:
    # a0 包含要打印的字符
    
    # 加载当前缓冲区长度
    la t0, buflen
    lw t1, 0(t0)
    
    # 获取缓冲区地址并加上偏移
    la t2, buffer
    add t2, t2, t1
    
    # 增加缓冲区长度
    addi t3, t1, 1
    sw t3, 0(t0)
    
    # 存储字符到缓冲区
    sb a0, 0(t2)
    
    # 检查是否需要刷新缓冲区（遇到换行符或缓冲区满）
    li t4, 10        # '\n'
    beq a0, t4, flush
    li t4, 64        # 缓冲区大小
    beq t3, t4, flush
    
    li a0, 0
    ret

# 刷新缓冲区的函数
flush:
    # 保存寄存器
    addi sp, sp, -128
    sd ra, 0(sp)
    
    # 准备发送到主机的数据结构
    addi t0, sp, 63
    andi t0, t0, -64   # 64字节对齐
    
    # 设置长度
    li t1, 64
    sd t1, 0(t0)      # 大小
    li t1, 1
    sd t1, 8(t0)      # 命令类型（1表示打印）
    
    # 设置缓冲区地址
    la t1, buffer
    sd t1, 16(t0)
    
    # 设置实际长度
    la t1, buflen
    lw t1, 0(t1)
    sd t1, 24(t0)
    
    # 内存屏障
    fence
    
    # 发送到主机
    la t1, tohost
    sd t0, 0(t1)
    
    # 等待主机响应
1:  la t1, fromhost
    ld t2, 0(t1)
    beqz t2, 1b
    
    # 清除fromhost
    la t1, fromhost
    sd zero, 0(t1)
    
    # 内存屏障
    fence
    
    # 重置缓冲区长度
    la t0, buflen
    sw zero, 0(t0)
    
    # 恢复寄存器
    ld ra, 0(sp)
    addi sp, sp, 128
    
    li a0, 0
    ret

# 打印字符串的函数
myputs:
    # a0 包含字符串地址
    addi sp, sp, -16
    sd ra, 0(sp)
    
1:  lbu t0, 0(a0)     # 加载一个字节
    beqz t0, 2f        # 如果是0，结束
    mv a1, a0          # 保存当前地址
    mv a2, t0          # 保存当前字符
    mv a0, t0          # 移动字符到a0作为参数
    call myputchar       # 打印字符
    mv a0, a1          # 恢复地址
    addi a0, a0, 1     # 移动到下一个字符
    j 1b
    
2:  ld ra, 0(sp)
    addi sp, sp, 16
    ret