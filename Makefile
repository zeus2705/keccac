CC=gcc
CFLAGS=-Wall -W -ansi -Werror -std=c99
OBJBIN=src/hash.o src/keccak.o src/main.o
OBJLIB=src/hash.o src/keccak.o 


%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

keccak: $(OBJBIN)
	clear
	echo "Building the keccak binary"
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x keccak

libkeccak: $(OBJLIB)
	clear
	echo "Building the keccak lib"
	ar -cvq $@.a $^
	ranlib libkeccak.a
	cp src/libkeccak.h ./

test:
	make clean;
	make keccak;
	Test/test.sh;

.PHONY : clean
clean :
	rm -rf src/*.o keccak libkeccak.a libkeccak.h