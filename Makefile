CC=gcc
CFLAGS=-Wall -W -ansi -Werror -std=c99
SRC=src/hash.c src/keccak.c
MAIN=src/main.c
OBJMAIN=Build/main.o
OBJ=Build/hash.o Build/keccak.o 

keccak:
	clear
	echo "Building the keccak binary"
	mkdir Build || true
	$(CC) -c $(SRC) $(MAIN) $(CFLAGS)
	mv *.o Build/ || true
	$(CC) -o keccak $(OBJ) $(OBJMAIN) $(CFLAGS)
	chmod +x keccak

keccaklib:
	clear
	echo "Building the keccak lib"
	$(CC) -c $(SRC) $(CFLAGS)
	mv *.o Build/ || true
	ar -cvq keccak.a $(OBJ)

test:
	make clean;
	make keccak;
	Test/test.sh;

.PHONY : clean
clean :
	rm -rf Build/* keccak keccak.a