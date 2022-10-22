CC=gcc
CFLAGS=-Wall -W -ansi -Werror -std=c99 -Ofast
OBJ=sha3-256.c


%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

SHA3-256: $(OBJ)
	clear
	echo "Building the keccak binary"
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x $^

test: $(OBJFAST)
	make clean;
	make SHA3-256;
	Test/test.sh;

.PHONY : clean
clean :
	rm SHA3-256 || true