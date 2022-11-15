#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/*bitwise rotation to left from stackoverflow https://stackoverflow.com/questions/52608994/proper-way-for-declaring-binary-rotation-preprocessor-macro*/
#define ROTL(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#define BUFF_SIZE 8160
#define R 1088
#define SIZE_BLOCK 136
#define PACKET_IN_SPONGE_LEN 17

const uint8_t endbyte_d = 0x06  ;

const uint64_t RoundConstant[24] = {
    0x0000000000000001,
    0x0000000000008082,
    0x800000000000808A,
    0x8000000080008000,
    0x000000000000808B,
    0x0000000080000001,
    0x8000000080008081,
    0x8000000000008009,
    0x000000000000008A,
    0x0000000000000088,
    0x0000000080008009,
    0x000000008000000A,
    0x000000008000808B,
    0x800000000000008B,
    0x8000000000008089,
    0x8000000000008003,
    0x8000000000008002,
    0x8000000000000080,
    0x000000000000800A,
    0x800000008000000A,
    0x8000000080008081,
    0x8000000000008080,
    0x0000000080000001,
    0x8000000080008008,
};


const uint8_t ROTOFF[25] = {
    0, 1, 62, 28, 27,
    36, 44, 6, 55, 20,
    3, 10 , 43, 25 ,39,
    41, 45, 15 ,21, 8,
    18, 2, 61, 56, 14,
};



void keccaff1600(uint64_t *lanes){
     //temporary state permutation holder cf. Pseudo-code description of the permutations
    uint64_t C[5];
    uint64_t D[5];
    uint64_t B[25];

    for (uint16_t rounds = 0; rounds < 24; rounds++){
        //A == lanes

        int i = 0;
        int j = 0;
        int index = 0;
       
       

        /*
        # θ step
        C[x] = A[x,0] xor A[x,1] xor A[x,2] xor A[x,3] xor A[x,4],   for x in 0…4
        D[x] = C[x-1] xor rot(C[x+1],1),                             for x in 0…4
        A[x,y] = A[x,y] xor D[x],                           for (x,y) in (0…4,0…4)
        */
        for (; i < 5; i++)
            C[i] = lanes[i] ^ lanes[i + 5] ^ lanes[i + 10] ^ lanes[i + 15] ^ lanes[i + 20];
        for (i = 0; i < 5; i++){
            D[i] = C[(i + 4) % 5] ^ ROTL(C[(i + 1 )% 5], 1);
            for (int j = 0; j < 5; j++)
                lanes[j * 5 + i] ^= D[i];
        }

        //rho
        /* # ρ and π steps
        B[y,2*x+3*y] = rot(A[x,y], r[x,y]),                 for (x,y) in (0…4,0…4)*/

        for (i = 0; i < 5; i++){
            for (j = 0; j < 5; j++){
                index = i * 5 + j;
                lanes[index] = ROTL(lanes[index],ROTOFF[index]);
            }
        }

        //pi
        for (i = 0; i < 5; i++){
            for (j = 0; j < 5; j++){
                index = i * 5 + j;
                B[index] = lanes[index];
            }
        }
        for (i = 0; i < 5; i++){
            for (j = 0; j < 5; j++){
                int u = (0 * j + 1 * i) % 5;
                int v = (2 * j + 3 * i) % 5;
                lanes[v *5 +u] = B[5 * i + j];
            }
        }

        //x bizarre
        /*
        # χ step
        A[x,y] = B[x,y] xor ((not B[x+1,y]) and B[x+2,y]),  for (x,y) in (0…4,0…4)
        */
        for (i = 0; i < 5; i++){
                for (j = 0; j < 5; j++){
                    index = i * 5 + j;
                    C[j] = lanes[index] ^ ((~lanes[i * 5 + ((j +1) % 5)]) & lanes[i * 5 + ((j + 2) % 5)]);
                }
                for (j = 0; j < 5; j++){
                    index = i * 5 + j;
                    lanes[index] = C[j];
                }
        }
        
        // petit l
        /*
        # ι step
        A[0,0] = A[0,0] xor RC
        */
        lanes[0] ^= RoundConstant[rounds];
    }
}

void absorb(uint64_t  *lanes, uint8_t *buff ,uint8_t nb_block){
    //for each block Pi in P
    for (uint8_t i = 0; i < nb_block; i++) {
        //S[x,y] = S[x,y] xor Pi[x+5*y],          for (x,y) such that x+5*y < r/w
        uint64_t* block = (uint64_t *)buff + i * PACKET_IN_SPONGE_LEN;
        for (int j = 0; j < PACKET_IN_SPONGE_LEN; j++) {
            lanes[j] ^= block[j];
        }
        //S = Keccak-f[r+c](S)
        keccaff1600(lanes);
    }
}


void fast_keccak(int f){
    //***** Initialization *******//
    //buffer where the file is read
    uint8_t buff[BUFF_SIZE];
    //Permutation State array (A)
    uint64_t lanes[25] = {0};
    


    //***** Absorbing phase *******//
    uint16_t buffsize = read(f,buff, BUFF_SIZE);
    while (buffsize == BUFF_SIZE){
        //Absorbing all the buffer (60 blocks)
        absorb(lanes,buff,60);
        //refill the buffer
        buffsize = read(f,buff, BUFF_SIZE);
    }
    //buffer not full
    //calculating the number of full block in the buffer
    uint8_t nb_blocks = buffsize / SIZE_BLOCK;
    //Where is the last index of the last incomplete in the buffer
    size_t block_len = (nb_blocks + 1) * SIZE_BLOCK;

    //***** Padding *******//
    //filling the incomplete block of 0
    for (size_t i = buffsize; i < block_len; i++)
        buff[i] = 0;

    //Padding Byte
    //Mbytes || d || 0x00 || … || 0x00
    buff[buffsize] ^= endbyte_d;
    //P xor (0x00 || … || 0x00 || 0x80)
    buff[block_len - 1] = 0x80;
    
    //absorb all the blocks in the buffer
    absorb(lanes,buff,nb_blocks + 1);
    //***** End of the absorbing phase *******//


    //***** Squeezing *******//
    uint8_t *hash = (uint8_t *)lanes;
    for (size_t i = 0; i < 32; i++)
        printf("%.2x",hash[i]);
    printf("\n");
    

}

int main(int argc, char** argv){
    //Read the stdin
    if (argc == 1){
        fast_keccak(0);
        return 0;
    }
    int fd =0;
    //Read every file put in arguments
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help")  == 0){
            printf("Option:\n\t-h, --help display this message\nUsage:\n\tSHA3-256 FILE1 FILE2 ... FILEN\n\tSHA3-256 < FILE\nOutput:\n\tThe program will return a hash in hex format following the SHA3-256 algorithm policies check https://keccak.team/keccak_specs_summary.html for more information\n");
            return 0;
        }
        if (strcmp(argv[i], "-") == 0){
            fast_keccak(0);
            continue;
        }
        fd = open(argv[i], O_RDONLY);
        if (fd > 0) {
            printf("sha3-256(%s): ",argv[i]);
            fast_keccak(fd);
            close(fd);
        }
        else
            printf("Could not open the file %s\n",argv[i]);
    }
    return 0;
}