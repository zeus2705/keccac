#include "hash.h"

struct hash *new_hash(uint8_t size){
    struct hash *toreturn = malloc(sizeof(struct hash));
    if (!toreturn)
        return NULL;
    toreturn->bytearr = malloc(size);
    if (!toreturn->bytearr){
        free(toreturn);
        return NULL;
    }
    toreturn->size = size;
    return toreturn;
}
void free_hash(struct hash *tofree){
    free(tofree->bytearr);
    free(tofree);
}
void print_hash(struct hash *todisp){
    for (uint8_t i = 0; i < todisp->size; i++)
        printf("%.2x",todisp->bytearr[i]);
    printf("\n");
}
void hash_string(char *dst, struct hash *todisp){
    for (uint8_t i = 0; i < todisp->size; i++)
        sprintf(dst + i,"%.2x",todisp->bytearr[i]);
}