#include "keccak.h"

/*bitwise rotation to left from stackoverflow https://stackoverflow.com/questions/52608994/proper-way-for-declaring-binary-rotation-preprocessor-macro*/
#define ROTL(X, N)  ((N) == 0 ? (X) : (((X) << (N)) | ((X) >> (8 * sizeof(X) - (N)))))


const uint16_t keccakparam_bitrate[4] = { 1152, 1088, 832, 576};

const uint8_t keccakparam_hashsize[4] = { 28, 32, 48, 64};

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

void Round(uint64_t *lanes, uint16_t roundnb){

    //A == lanes

    int i = 0;
    int j = 0;
    int index = 0;
    //temporary lane holder cf Pseudo-code description of the permutations
    uint64_t C[5];
    uint64_t D[5];
    uint64_t B[25];

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
    lanes[0] ^= RoundConstant[roundnb];
}

void keccaff1600(uint64_t *lanes){
    for (uint16_t i = 0; i < 24; i++){
        Round(lanes, i);
    }
}

void keccac_sponge_absorb(uint16_t r, uint16_t sizeinput, uint8_t *paddedinput, uint64_t *lanes){
    /* 
    # Absorbing phase
    for each block Pi in P
    S[x,y] = S[x,y] xor Pi[x+5*y],          for (x,y) such that x+5*y < r/w
    S = Keccak-f[r+c](S)
    */
    int roverw = r/64;
    for (int i = 0; i < (sizeinput / (r / 8)); i++) {
        uint64_t* block = (uint64_t*)paddedinput + i * roverw;
        for (int j = 0; j < roverw; j++) {
            lanes[j] ^=  block[j];
        }
        keccaff1600(lanes);
    }
}

void keccac_sponge_squeeze(struct hash *result,uint64_t *lanes){
        memcpy(result->bytearr, lanes, result->size);
}

uint64_t padding_input(uint16_t r, uint64_t sizeinput,uint8_t *input,uint8_t *paddedinput){

    int size_block = r/8;
    int blocks = sizeinput / size_block;
    size_t block_len = (blocks + 1) * size_block;
    /* d = 2^|Mbits| + sum for i=0..|Mbits|-1 of 2^i*Mbits[i]
    P = Mbytes || d || 0x00 || … || 0x00
    P = P xor (0x00 || … || 0x00 || 0x80)*/
    memcpy(paddedinput,input,sizeinput);
    /* add padding bytes */
    paddedinput[sizeinput] ^= endbyte_d;
    paddedinput[block_len - 1] ^= 0x80;

    return block_len;
}


#define BUFF_SIZE 8160

struct hash *keccak(enum keccakmode mod, uint8_t *input, uint64_t sizeinput){
    struct hash *result = new_hash(keccakparam_hashsize[mod]);
    uint8_t buff[BUFF_SIZE] = {0};
    if (!result)
        errx(1,"No enoughs memory");
    uint16_t r =  keccakparam_bitrate[mod];
    uint64_t lanes[25] = {0};
    int nbabs = 0;
    while (sizeinput >= BUFF_SIZE){
        memcpy(buff,(input + BUFF_SIZE * nbabs),BUFF_SIZE);
        sizeinput -= BUFF_SIZE;
        nbabs++;
        keccac_sponge_absorb(r,BUFF_SIZE,buff,lanes);
    }
    uint8_t paddedinput[BUFF_SIZE] = {0};
    sizeinput = padding_input(r,sizeinput,(input + BUFF_SIZE * nbabs),paddedinput);
    keccac_sponge_absorb(r,sizeinput,paddedinput,lanes);
    keccac_sponge_squeeze(result,lanes);
    return result;
}

struct hash *keccak_file(enum keccakmode mod,int f){
    struct hash *result = new_hash(keccakparam_hashsize[mod]);
    uint8_t buff[BUFF_SIZE] = {0};
    if (!result)
        errx(1,"No enoughs memory");
    uint16_t r =  keccakparam_bitrate[mod];
    uint64_t lanes[25] = {0};
    uint64_t buffsize = read(f,buff, BUFF_SIZE);
    while (buffsize == BUFF_SIZE){
        keccac_sponge_absorb(r,BUFF_SIZE,buff,lanes);
        buffsize = read(f,buff, BUFF_SIZE);
    }
    uint8_t paddedinput[BUFF_SIZE] = {0};
    buffsize = padding_input(r,buffsize,buff,paddedinput);
    keccac_sponge_absorb(r,buffsize,paddedinput,lanes);
    keccac_sponge_squeeze(result,lanes);
    return result;
}