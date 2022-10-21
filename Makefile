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
	chmod +x keccak

libkeccak: $(OBJLIB)
	clear
	echo "Building the keccak lib"
	ar -cvq $@.a $^
	ranlib libkeccak.a
	cp src/libkeccak.h ./

keccak256: $(OBJFAST)
	clear
	echo "Building the keccak binary"
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x keccak256

test: $(OBJBIN)
	make clean;
	make keccak;
	Test/test.sh;

.PHONY : clean
clean :
	rm -rf src/*.o keccak libkeccak.a libkeccak.h keccak256