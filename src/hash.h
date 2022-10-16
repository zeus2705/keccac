#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct hash{
    uint8_t *bytearr;
    uint8_t size;
};

struct hash *new_hash(uint8_t size);
void free_hash();
void print_hash(struct hash *todisp);
void hash_string(char *dst, struct hash *todisp);