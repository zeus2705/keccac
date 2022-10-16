#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "keccak.h"


char *keccakmode_str[4] = {"keccak224","keccak256","keccak384","keccak512"};

void print_help()
{
    printf("Option:\n\t-224 : use keccak224\n\t-256 : use keccak256\n\t-384 : use keccak384\n\t-512 : use keccak512\nUsage : \n\tkeccak FILE1 FILE2 ... FILEN\n\ttkeccak STRING1 STRING2 ... STRINGN\n");
}

int main(int argc, char** argv)
{
    if (argc == 1){
        print_help();
        return 0;
    }
    enum keccakmode mode = keccak256;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-h") == 0){
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-224") == 0)
            mode = keccak224;
        else if (strcmp(argv[i], "-256") == 0)
            mode = keccak256;
        else if (strcmp(argv[i], "-384") == 0)
            mode = keccak384;
        else if (strcmp(argv[i], "-512") == 0)
            mode = keccak512;
        else{
            int file = open(argv[i], O_RDONLY);
            if (file > 0) {
                struct hash *h = keccak_file(mode, file);
                printf("%s(%s) : ",keccakmode_str[mode],argv[i]);
                print_hash(h);
                free_hash(h);
                close(file);
            }
            else{
                struct hash *h =  keccak(mode,(uint8_t *)argv[i], strlen(argv[i]));
                printf("%s(\'%s\') : ",keccakmode_str[mode],argv[i]);
                print_hash(h);
                free_hash(h);
            }
        }
    }
}