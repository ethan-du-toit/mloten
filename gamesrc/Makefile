NAME = ./game.c
PF   = ./
OUT  = $(PF)/../bin/rvgame.bin
OPT  = -O3 -flto
EXLINKS = -L. -l:crt1.o
PX   = riscv64-unknown-elf-
all:
	$(PX)gcc $(OPT) -pedantic -Wall -Wextra -T$(PF)mloten.ld -Wl,--build-id=none -nostdlib -nostartfiles $(EXLINKS) -march=rv32im -mabi=ilp32 $(NAME) -o $(PF)bin/__temp__
	$(PX)objcopy -O binary $(PF)bin/__temp__ $(OUT)
