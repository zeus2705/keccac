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
#define ROW 17

const uint8_t endbyte_d = 0x01;

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
    for (uint16_t rounds = 0; rounds < 24; rounds++){
        //A == lanes

        int i = 0;
        int j = 0;
        int index = 0;
        //temporary lane holder cf Pseudo-code description of the permutations
        uint64_t C[5] = {0};
        uint64_t D[5] = {0};
        uint64_t B[25] = {0};

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

        //x bizzare
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

void absorb(uint64_t  *lanes, uint8_t *buff ,int nb_block){
    for (int i = 0; i < nb_block; i++) {
        uint64_t* block = (uint64_t *)buff + i * ROW;
        for (int j = 0; j < ROW; j++) {
            lanes[j] ^= block[j];
        }
        keccaff1600(lanes);
    }
}


void fast_keccak(int f){
    uint8_t hash[32] ={0};
    uint8_t buff[BUFF_SIZE] = {0};
    uint64_t lanes[25] = {0};
    uint16_t buffsize = read(f,buff, BUFF_SIZE);
    while (buffsize == BUFF_SIZE){
        absorb(lanes,buff,60);
        buffsize = read(f,buff, BUFF_SIZE);
    }
    uint8_t nb_blocks = buffsize / SIZE_BLOCK;
    size_t block_len = (nb_blocks + 1) * SIZE_BLOCK;
    buff[buffsize] ^= endbyte_d;
    buff[block_len - 1] ^= 0x80;
    absorb(lanes,buff,nb_blocks + 1);
    memcpy(hash, lanes, 32);
    for (size_t i = 0; i < 32; i++)
        printf("%.2x",hash[i]);
    printf("\n");
    

}

int main(){
    int file = open("limit", O_RDONLY);
    fast_keccak(file);
    close(file);
    return 0;
}