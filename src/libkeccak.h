#pragma once
#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

struct hash{
    uint8_t *bytearr;
    uint8_t size;
};

enum keccakmode{
    keccak224,
    keccak256,
    keccak384,
    keccak512,
};

struct hash *new_hash(uint8_t size);
void free_hash();
void print_hash(struct hash *todisp);
void hash_string(char *dst, struct hash *todisp);




struct hash *keccak(enum keccakmode mod,uint8_t *input,uint64_t sizeinput);
struct hash *keccak_file(enum keccakmode mod,int reaf);