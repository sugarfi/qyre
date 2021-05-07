.PHONY: clean os.img
DEFAULT_GOAL: os.img

C_SRC := $(shell find src -type f -name '*.c')
C_OBJ := $(C_SRC:.c=.o)

CFLAGS = -ffreestanding -fno-stack-protector -c -nostdlib -nostdinc -mno-red-zone -fpic -Wall -I src
LDFLAGS = -T linker.ld -nostdlib -nostartfiles

%.o: %.c
	@echo "Compile $<"
	@gcc $(CFLAGS) -o $@ $<

kernel.o: $(C_OBJ)
	@echo "Link kernel.o"
	@ld $(LDFLAGS) -o boot/kernel.o $(C_OBJ)

diskdb.bin:
	@echo "Create disk database file"
	@dd if=/dev/zero of=diskdb.bin bs=1M count=64

os.img: clean kernel.o diskdb.bin
	@echo "Generate os.img"
	@mkbootimg qyre.json os.img

clean:
	@echo "Remove binaries"
	@rm -rf boot/kernel.o diskdb.bin $(C_OBJ)
