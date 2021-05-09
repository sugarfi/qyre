.PHONY: clean os.img dbfmt
DEFAULT_GOAL: os.img

C_SRC := $(shell find src -type f -name '*.c')
C_OBJ := $(C_SRC:.c=.o)

CFLAGS = -Wall -fpic -ffreestanding -fno-stack-protector -nostdinc -nostdlib -I src -mno-red-zone -c
LDFLAGS = -nostdlib -nostartfiles -T linker.ld

%.o: %.c
	@echo "Compile $<"
	@gcc $(CFLAGS) -o $@ $<

kernel.o: $(C_OBJ)
	@echo "Link kernel.o"
	@ld $(LDFLAGS) -o boot/kernel.o $(C_OBJ) 
	@strip -s boot/kernel.o

dbfmt:
	@echo "Compile dbfmt"
	@make -C dbfmt

diskdb.bin:
	@echo "Create disk database file"
	@dd if=/dev/zero of=diskdb.bin bs=1M count=64
	@echo "Format disk database file"
	@dbfmt/dbfmt diskdb.bin hi.txt

os.img: clean kernel.o diskdb.bin dbfmt
	@echo "Generate os.img"
	@mkbootimg qyre.json os.img

clean:
	@echo "Remove binaries"
	@rm -rf boot/kernel.o diskdb.bin $(C_OBJ)
