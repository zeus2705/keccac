CFLAGS=-Wall -W -ansi -Werror -std=c99 -Ofast
OBJ=sha3-256.c


SHA3-256: $(OBJ)
	clear
	echo "Building the keccak binary"
	$(CC) -o $@ $(OBJ) $(CFLAGS)

man:
	pandoc sha3-256-man.md -s -t man > sha3-256.man

test: $(OBJFAST)
	make clean;
	make SHA3-256;
	Test/test.sh;

.PHONY: clean
clean:
	rm SHA3-256 sha3-256.man 2>/dev/null || true
