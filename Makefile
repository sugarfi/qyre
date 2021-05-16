.PHONY: clean os.img dbfmt
DEFAULT_GOAL: os.img

C_SRC := $(shell find src -type f -name '*.c')
C_OBJ := $(C_SRC:.c=.o)

CFLAGS = -Wall -Wextra -O3 -pipe -I src -c -ffreestanding -fno-stack-protector -fno-pic -fpie -mgeneral-regs-only -mno-red-zone -g
LDFLAGS = -nostdlib -fno-pic -fpie -Wl,-static,-pie,--no-dynamic-linker,-ztext -z max-page-size=0x1000 -T linker.ld

%.o: %.c
	@echo "Compile $<"
	@gcc $(CFLAGS) -o $@ $<

kernel.o: $(C_OBJ)
	@echo "Link kernel.o"
	@gcc $(LDFLAGS) -o boot/kernel.o $(C_OBJ) 

dbfmt:
	@echo "Compile dbfmt"
	@make -C dbfmt

diskdb.bin: dbfmt
	@echo "Create disk database file"
	@dd if=/dev/zero of=diskdb.bin bs=1M count=64
	@echo "Format disk database file"
	@dbfmt/dbfmt diskdb.bin hi.txt

os.img: clean kernel.o diskdb.bin
	@echo "Generate os.img"
	@dd if=/dev/zero of=os.img bs=1M count=80
	@parted -s os.img mklabel msdos
	@parted -s os.img mkpart primary 1 16M
	@parted -s os.img mkpart primary 16M 100%
	@echfs-utils -m -p0 os.img quick-format 8192
	@echfs-utils -m -p0 os.img import boot/limine.sys limine.sys
	@echfs-utils -m -p0 os.img import boot/limine.cfg limine.cfg
	@echfs-utils -m -p0 os.img import boot/kernel.o kernel.o
	@dd if=diskdb.bin of=os.img seek=30720 conv=notrunc
	@limine-install os.img

clean:
	@echo "Remove binaries"
	@rm -rf boot/kernel.o diskdb.bin $(C_OBJ)
