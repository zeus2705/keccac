#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "keccak.h"


char *keccakmode_str[4] = {"keccak224","keccak256","keccak384","keccak512"};

void print_help()
{
    printf("Option:\n\t-h display this message\n\t-v explain what is being done\n\t-224 : use keccak224\n\t-256 : use keccak256 (default)\n\t-384 : use keccak384\n\t-512 : use keccak512\nUsage : \n\tkeccak FILE1 FILE2 ... FILEN\n\tkeccak STRING1 STRING2 ... STRINGN\nOutput :\n\tThe program will return a hash in hex format following the keccak algorithm policies check https://keccak.team/keccak_specs_summary.html for more information\n");
}

int main(int argc, char** argv)
{
    int v = 0;
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
        else if (strcmp(argv[i], "-v") == 0)
            v = 1;
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
                if (v)
                    printf("%s(%s) : ",keccakmode_str[mode],argv[i]);
                print_hash(h);
                free_hash(h);
                close(file);
            }
            else{
                struct hash *h =  keccak(mode,(uint8_t *)argv[i], strlen(argv[i]));
                if (v)
                    printf("%s(\'%s\') : ",keccakmode_str[mode],argv[i]);
                print_hash(h);
                free_hash(h);
            }
        }
    }
}