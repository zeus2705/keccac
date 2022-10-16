#pragma once
#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "hash.h"


enum keccakmode{
    keccak224,
    keccak256,
    keccak384,
    keccak512,
};


struct hash *keccak(enum keccakmode mod,uint8_t *input,uint64_t sizeinput);
struct hash *keccak_file(enum keccakmode mod,int reaf);