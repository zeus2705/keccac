# Implementation of the SHA3-256 algorithm based on Keccak

# Compilation

### To compile the binary:

```
42sh$ make SHA3-256
```

### Generate the `man` with `pandoc`:

[`pandoc`](https://github.com/jgm/pandoc) command must be installed: 

```
42sh$ make man
```

Launch the local `man`:
```
42sh$ man -l sha3-256.man
```

### To clean temporary file and compiled file use:
```
42sh$ make clean
```

### To start the test suite use:

```
42sh$ make test
```
# Usage

### Basic information about the binary
```
42sh$ ./SHA3-256 --help
Option:
	-h, --help display this message
Usage:
	SHA3-256 FILE1 FILE2 ... FILEN
	SHA3-256 < FILE
Output:
	The program will return a hash in hex format following the SHA3-256 algorithm policies check https://keccak.team/keccak_specs_summary.html for more information
```

### Example

Checksum a file:

```
42sh$ ./SHA3-256 Test/testfile/small 
sha3-256(Test/testfile/small): c65bb706eed8bf7bef71dc0a97f510797f384d3fd6cd8cd6a04a4fa8ef410fa1
```

Alternative method:
```
42sh$ ./SHA3-256 < Test/testfile/small
c65bb706eed8bf7bef71dc0a97f510797f384d3fd6cd8cd6a04a4fa8ef410fa1
```

Checksum a stream:
```
42sh$ ./SHA3-256 -
OpenBSD gets updated as often as its users bathe << You don't need updates when you make things perfect in the first try.
f1b3a10375c8d81826185c763cdd7910ac06990c273e4909348f229d27a62e31
```
