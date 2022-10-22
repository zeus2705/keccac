CC=gcc
CFLAGS=-Wall -W -ansi -Werror -std=c99 -Ofast
OBJFAST=src/fast_keccak.c
OBJBIN=src/hash.o src/keccak.o src/main.o
OBJLIB=src/hash.o src/keccak.o 


%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

keccak: $(OBJBIN)
	clear
	echo "Building the keccak binary"
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x $^

libkeccak: $(OBJLIB)
	clear
	echo "Building the keccak lib"
	ar -cvq $@.a $^
	ranlib $^.a
	cp src/libkeccak.h ./

SHA3-256: $(OBJFAST)
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
	rm -rf src/*.o keccak libkeccak.a libkeccak.h SHA3-256