CROSS_COMPILE = riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld

CFLAGS = -march=rv64i -mabi=lp64 -static -mcmodel=medany \
         -fvisibility=hidden -nostdlib -nostartfiles -T link.ld \
         -include define.h

TARGET = hello
SRCS = hello.c crt0.S

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@
	$(CROSS_COMPILE)objcopy -O binary $@ $(TARGET).bin
	$(CROSS_COMPILE)objdump -D $@ > $(TARGET).dump

clean:
	rm -f $(TARGET) $(TARGET).bin $(TARGET).dump