# Implementation of the keccak algorithm

# Installation

```
42sh$ git clone git@github.com:zeus2705/keccac.git
```

#Compilation

To compile the binary :

```
42sh$ make keccak
```

To compile a static lib :

```
42sh$ make libkeccak
```

To clean temporary file and compiled file use :

```
42sh$ make clean
```

To start the test suite use :

```
42sh$ make test
```
# Usage


Basic information about the binary
```
42sh$ ./keccak 
Option:
        -h display this message
        -v explain what is being done
        -224 : use keccak224
        -256 : use keccak256
        -384 : use keccak384
        -512 : use keccak512
Usage : 
        keccak FILE1 FILE2 ... FILEN
        keccak STRING1 STRING2 ... STRINGN
Output :
        The program will return a hash in hex format following the keccak algorithm policies check 
        https://keccak.team/keccak_specs_summary.html for more information
```

**Keccak224** will provide a hash of **28** bits of a file or a string.
```
42sh$ ./keccak -v -224 test
keccak224('test') : 3be30a9ff64f34a5861116c5198987ad780165f8366e67aff4760b5e
```

**Keccak256** will provide a hash of **32** bits of a file or a string, this is the **default option** for the binary.
```
42sh$ ./keccak -v test
keccak256('test') : 9c22ff5f21f0b81b113e63f7db6da94fedef11b2119b4088b89664fb9a3cb658
```

**Keccak384** will provide a hash of **48** bits of a file or a string.

```
42sh$ ./keccak -v -384 test
keccak384('test') : 53d0ba137307d4c2f9b6674c83edbd58b70c0f4340133ed0adc6fba1d2478a6a03b7788229e775d2de8ae8c0759d0527
```

**Keccak512** will provide a hash of **64** bits of a file or a string.

```
42sh$ ./keccak -v -384 test
./keccak -512 test
keccak512('test') : 1e2e9fc2002b002d75198b7503210c05a1baac4560916a3c6d93bcce3a50d7f00fd395bf1647b9abb8d1afcc9c76c289b0c9383ba386a956da4b38934417789e
```

To checksum a file just put a valid path
```
42sh$ ./keccak -v Makefile 
keccak256(Makefile) : 97a55595089c600106defaa208802f82f3c57daf58573554c695ae7269346607
```

You can also just get the hash by removing the verbose (-v)
```
42sh$ ./keccak test
9c22ff5f21f0b81b113e63f7db6da94fedef11b2119b4088b89664fb9a3cb658
```

To use the static lib get the libkeccac.a and  libkeccac.h
```
42sh$ ls
libkeccak.a  libkeccak.h  main.c
42sh$ cat main.c
#include <stdint.h>
#include <string.h>
#include "libkeccak.h"

int main(){
    char *test = "test";
    struct hash *h =  keccak(keccak256,(uint8_t *)test, strlen(test));
    print_hash(h);
    free_hash(h);
}
42sh$ gcc main.c -L. -lkeccak  -o main
42sh$ ./main 
9c22ff5f21f0b81b113e63f7db6da94fedef11b2119b4088b89664fb9a3cb658
```