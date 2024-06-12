/*
main.c

    Data Encryption Standard(DES)
*/
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>


// typedef for universal coding.
typedef unsigned char BYTE;
typedef unsigned long int DWORD;
typedef unsigned long long int QWORD;


// Show Internal Values: P-Box, S-Box, Etc.. 
#define DEBUG_SHOW_INTERNAL_VALUES 0


#define MODE_ENCRYPTION 0
#define MODE_DECRYPTION 1


// S-Boxes:
BYTE sbox1[4][16] = {
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
    {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
    {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
    {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
};
BYTE sbox2[4][16] = {
    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
    {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
    {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
    {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
};
BYTE sbox3[4][16] = {
    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
    {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
    {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
    {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
};
BYTE sbox4[4][16] = {
    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
    {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
    {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
    {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
};
BYTE sbox5[4][16] = {
    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
    {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
    {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
    {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
};
BYTE sbox6[4][16] = {
    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
    {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
    {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
    {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
};
BYTE sbox7[4][16] = {
    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
    {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
    {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
    {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
};
BYTE sbox8[4][16] = {
    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
    {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
    {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
    {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
};


// Functions for Test
void TestPrintBin(QWORD val){
    for(BYTE i=0; i<64; i++){
        if((i%8 == 0) && (i != 0)){
            printf(" ");
        }
        printf("%01d", (val >> (63 - i)) & 0b1);

    }
}
void TestPrintHex(QWORD val){
    for(BYTE i=0; i<64; i+=8){
        if((i%16 == 0) && (i != 0)){
            printf(" ");
        }
        printf("%02x", (val >> (56 - i)) & 0xFF);
    }
}


// DES Functions:
void InitialPermutation(QWORD * OutputBlock, QWORD * InputBlock){
    BYTE IPtable[64] = {58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1,59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};
    for(BYTE i = 0; i<64; i++){
        *OutputBlock = (*OutputBlock) | ((((*InputBlock) >> (64 - IPtable[i])) & 0b1) << (63 - i));
    }
}
void FinalPermutation(QWORD * OutputBlock, QWORD * InputBlock){
    BYTE FPtable[64] = {40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};
    for(BYTE i = 0; i<64; i++){
        *OutputBlock = (*OutputBlock) | ((((*InputBlock) >> (64 - FPtable[i])) & 0b1) << (63 - i));
    }
}
void ExpansionPermutation(QWORD * Output48BitBlock, DWORD * Input32BitBlock){
    BYTE ExpansionPBox[48] = {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1}; // Expansion P-Box Table
    for(BYTE i=0; i<48; i++){
        *Output48BitBlock = (*Output48BitBlock) | (((QWORD)(((*Input32BitBlock) >> (32 - ExpansionPBox[i])) & 0b1)) << (47 - i)); // BIT OPERATION!!
    }
    // no neeeeed return
}
void StraightPermutation(DWORD * Output32BitBlock, DWORD * Input32BitBlock){
    BYTE StraightPBox[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25}; // Straight P-Box table
    for(BYTE i=0; i<32; i++){
        *Output32BitBlock = ((*Output32BitBlock) & ~((DWORD)0b1 << (31 - i))) | (((*Input32BitBlock >> (32 - StraightPBox[i])) & 0b1) << (31 - i)); // AGAIN BIT OPERATION!
    }
}
void Substitution(DWORD * Output32BitBlock, QWORD * Input48BitBlock){
    // SOME OF BIT OPERATION!!!!! xD
    *Output32BitBlock = sbox1[((*Input48BitBlock >> 46) & 0b10)| ((*Input48BitBlock >> 42) & 0b1)][(*Input48BitBlock >> 43) & 0b1111] << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox2[((*Input48BitBlock >> 40) & 0b10)| ((*Input48BitBlock >> 36) & 0b1)][(*Input48BitBlock >> 37) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox3[((*Input48BitBlock >> 34) & 0b10)| ((*Input48BitBlock >> 30) & 0b1)][(*Input48BitBlock >> 31) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox4[((*Input48BitBlock >> 28) & 0b10)| ((*Input48BitBlock >> 24) & 0b1)][(*Input48BitBlock >> 25) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox5[((*Input48BitBlock >> 22) & 0b10)| ((*Input48BitBlock >> 18) & 0b1)][(*Input48BitBlock >> 19) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox6[((*Input48BitBlock >> 16) & 0b10)| ((*Input48BitBlock >> 12) & 0b1)][(*Input48BitBlock >> 13) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox7[((*Input48BitBlock >> 10) & 0b10)| ((*Input48BitBlock >> 6) & 0b1)][(*Input48BitBlock >> 7) & 0b1111]) << 4;
    *Output32BitBlock = (*Output32BitBlock | sbox8[((*Input48BitBlock >> 4) & 0b10)| ((*Input48BitBlock >> 0) & 0b1)][(*Input48BitBlock >> 1) & 0b1111]);
}
void PC1(QWORD * Output56BitKey, QWORD * Input64BitKey){
    // Permuted Choice 1
    BYTE PC1Table[56] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
    for(BYTE i=0; i<56; i++){
        *Output56BitKey = (*Output56BitKey) | ((((*Input64BitKey) >> (64 - PC1Table[i])) & 0b1) << (55 - i));
    }

}
void PC2(QWORD * Output48BitRoundKey, QWORD * Input56BitKey){
    // Permuted Choice 2
    BYTE PC2Table[48] = {14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
    for(BYTE i=0; i<48; i++){
        *Output48BitRoundKey = (*Output48BitRoundKey) | ((((*Input56BitKey) >> (56 - PC2Table[i])) & 0b1) << (47 - i));
    }
}

void KeyExpansion(QWORD * Output48BitRoundKeys, QWORD * Initial64BitKey){
    // Key Expansion
    QWORD Key56Bit = 0;
    DWORD Left28BitKey = 0;
    DWORD Right28BitKey = 0;
    BYTE ShiftAmount[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

    PC1(&Key56Bit, Initial64BitKey); // PC1

    Left28BitKey = Key56Bit >> 28;
    Right28BitKey = Key56Bit & 0xfffffff;

    for(BYTE i=0; i<16; i++){
        Left28BitKey = ((Left28BitKey << ShiftAmount[i]) & 0xfffffff) | (Left28BitKey >> (28 - ShiftAmount[i])); // Circular Shift
        Right28BitKey = ((Right28BitKey << ShiftAmount[i]) & 0xfffffff) | (Right28BitKey >> (28 - ShiftAmount[i])); // Circular Shift
        Key56Bit = (((QWORD)Left28BitKey) << 28) | Right28BitKey; // Merge
        PC2(&(Output48BitRoundKeys[i]), &Key56Bit); // PC2
    }
}

void Encryption(QWORD * Output64BitBlock, QWORD * Input64BitBlock, QWORD * RoundKeys48Bit){
    QWORD Temp64BitBlock = 0;
    // TEST:
    if(DEBUG_SHOW_INTERNAL_VALUES){
        for(BYTE i=0; i<16; i++){
            TestPrintBin(RoundKeys48Bit[i]);
            printf("\n");
        }
    }


    // ##Calculation for Block
    InitialPermutation(&Temp64BitBlock, Input64BitBlock);
    // Test:
    if(DEBUG_SHOW_INTERNAL_VALUES){
        printf("Initial Perm   : ");
        TestPrintBin(Temp64BitBlock);
        printf("\n");
        printf("----------------------------------------------------------------------------------------\n");
    }

    // seperate Left/Right 32bit
    DWORD Left32Bit = 0;
    DWORD Right32Bit = 0;


    Left32Bit = (Temp64BitBlock >> 32) & 0xffffffff;
    Right32Bit = Temp64BitBlock & 0xffffffff;


    // go 16-round
    for(BYTE i=0; i<16; i++){
        QWORD Right48Bits = 0;
        DWORD TempRight32Bit = 0;
        DWORD TempRight32Bit2 = 0;
        DWORD Temp32Bit = 0;


        ExpansionPermutation(&Right48Bits, &Right32Bit); // Expansion P-Box
        // Test:
        if(DEBUG_SHOW_INTERNAL_VALUES){
            printf("Expansion Right: ");
            TestPrintBin(Right48Bits);
            printf("\n");
        }


        Right48Bits = Right48Bits ^ RoundKeys48Bit[i]; // XOR with round key
        // Test:
        if(DEBUG_SHOW_INTERNAL_VALUES){
            printf("XOR with Key   : ");
            TestPrintBin(Right48Bits);
            printf("\n");
        }


        Substitution(&TempRight32Bit, &Right48Bits); // S-Box        
        // Test:
        if(DEBUG_SHOW_INTERNAL_VALUES){
            printf("After S-Box    : ");
            TestPrintBin(TempRight32Bit);
            printf("\n");
        }


        StraightPermutation(&TempRight32Bit2, &TempRight32Bit); // Straight P-Box
        // Test:
        if(DEBUG_SHOW_INTERNAL_VALUES){
            printf("Straight P-Box : ");
            TestPrintBin(TempRight32Bit2);
            printf("\n");
        }


        Left32Bit = Left32Bit ^ TempRight32Bit2; // XOR with Left Block
        // Test:
        if(DEBUG_SHOW_INTERNAL_VALUES){
            printf("XOR with left  : ");
            TestPrintBin(Left32Bit);
            printf("\n");
            printf("----------------------------------------------------------------------------------------\n");
        }


        if(i != 15){
            // Swap Left-Right
            Temp32Bit = Right32Bit;
            Right32Bit = Left32Bit;
            Left32Bit = Temp32Bit;
        }else{
            Temp64BitBlock = ((QWORD)Left32Bit << 32) | Right32Bit;
        }
    }
    FinalPermutation(Output64BitBlock, &Temp64BitBlock);
}

void Encrypt(QWORD * Output64BitBlock, QWORD * Input64BitBlock, QWORD * Initial64BitKey){
    QWORD RoundKeys48Bit[16];
    memset(&RoundKeys48Bit, 0, 128); // 128 = (64 / 8) * 16 # init to 0
    KeyExpansion(RoundKeys48Bit, Initial64BitKey);
    Encryption(Output64BitBlock, Input64BitBlock, RoundKeys48Bit);
}

void Decrypt(QWORD * Output64BitBlock, QWORD * Input64BitBlock, QWORD * Initial64BitKey){
    QWORD TempRoundKeys48Bit[16];
    QWORD RoundKeys48Bit[16];
    memset(&TempRoundKeys48Bit, 0, 128); // 128 = (64 / 8) * 16 # init to 0
    memset(&RoundKeys48Bit, 0, 128); // 128 = (64 / 8) * 16 # init to 0
    KeyExpansion(TempRoundKeys48Bit, Initial64BitKey);


    // 라운드 키 순서 뒤집기:
    for(BYTE i=0; i<16; i++){
        RoundKeys48Bit[i] = TempRoundKeys48Bit[15 - i];
    }
    Encryption(Output64BitBlock, Input64BitBlock, RoundKeys48Bit);
}

int SelfModificationCheck(){
    /*
    심심해서?/처음에 만든 테스트 코드를 삭제하기에는 좀 아까워서 살릴 겸 넣는 기능
    백신프로그램에서 자가 진단 기능(변조 방지)이 있는 것 등, 어떤 프로그램은
    자신의 기능을 수행하기 전에, 사전에 입력된 값을 통해 자신이 정상적으로 동작하는지
    다시말해, 사전에 계산된 값을 내놓는지 확인함으로서, 무결성을 체크하는 것 같다.
    이를 흉내내보는 것이 목표라고 "주장"하며 처음 코드를 살려본다.

    반환값:
    0: 무결성 체크 성공=이상 없음
    1: 문제가 있음
    */
    // Encryption Test:
    QWORD Plain1 = 0x1122334455667788; // Dont change!
    QWORD Cipher1 = 0; // Dont change!
    QWORD Key1 = 0x752878397493cb70; // Dont change!


    printf("Encryption!\n");
    printf("Plain1         : ");
    TestPrintHex(Plain1);
    printf("\n");
    printf("Plain1         : ");
    TestPrintBin(Plain1);
    printf("\n");


    Encrypt(&Cipher1, &Plain1, &Key1);


    printf("Cipher1        : ");
    TestPrintBin(Cipher1);
    printf("\n");
    printf("Cipher1        : ");
    TestPrintHex(Cipher1);
    printf("\n");


    printf("----------------------------------------------------------------------------------------\n");
    // Decryption Test:
    QWORD Plain2 = 0; // Dont change!
    QWORD Cipher2 = 0xb5219ee81aa7499d; // Dont change!
    QWORD Key2 = 0x752878397493cb70; // Dont change!


    printf("Decryption!\n");
    printf("Cipher2        : ");
    TestPrintHex(Cipher2);
    printf("\n");
    printf("Cipher2        : ");
    TestPrintBin(Cipher2);
    printf("\n");


    Decrypt(&Plain2, &Cipher2, &Key2);


    printf("Plain2         : ");
    TestPrintBin(Plain2);
    printf("\n");
    printf("Plain2         : ");
    TestPrintHex(Plain2);
    printf("\n");


    // Validation:
    printf("----------------------------------------------------------------------------------------\n");
    printf("Plain1  == Plain2  : %d\n", (Plain1 == Plain2));
    printf("Cipher1 == Cipher2 : %d\n", (Cipher1 == Cipher2));
    if((Plain1 == Plain2) && (Cipher1 == Cipher2)){
        printf("Validation Success!\n");
        return 0; // No ERRORs in Code!
    }
    // Validation Fail!:
    printf("Validation Fail!\n");
    return 1;
}


int main(int argc, char* argv[]){
    // TEST:
    SelfModificationCheck(); // Self-Checking about THIS CODE(ex: S-Box values..)
    printf("----------------------------------------------------------------------------------------\n\n");


    // print Usage:
    if(argc < 5){
        printf("#########################################################\n");
        printf("USAGE:\n");
        printf("\n");
        printf("main.exe [input file path] [output file path] [Key] [0/1]\n");
        printf("Key : Must 64Bit Hex!\n");
        printf("0   : Encryption\n");
        printf("1   : Decryption\n");
        printf("#########################################################\n");
        return 0;
    }


    // Vars:
    BYTE TaskMode = MODE_ENCRYPTION; // Default
    QWORD Key = 0x752878397493cb70; // Default
    QWORD * InputBlocks = NULL;
    QWORD * OutputBlocks = NULL;
    BYTE blockAmount = 0;
    BYTE * bytePtr = NULL;
    FILE * ptrInputFile = NULL;
    FILE * ptrOutputFile = NULL;
    BYTE readbuf = 0;
    int fileSize = 0; // Byte size

    
    Key = strtoull(argv[3], NULL, 16);
    TaskMode = atoi(argv[4]);


    // File I/O:
    ptrInputFile = fopen(argv[1], "r");


    // Error Check:
    if(!ptrInputFile){
        printf("Cannot Open File!\n");
        return 0;
    }


    // Read File:
    fseek(ptrInputFile, 0, SEEK_END); // move file pointer at end of file
    fileSize = ftell(ptrInputFile); // get file pointer position at end == file size
    rewind(ptrInputFile); // move file pointer at start of file
    if(TaskMode == MODE_ENCRYPTION){
        blockAmount = (fileSize / 8) + 1; // Calc the Amount of Blocks
    }else if(TaskMode == MODE_DECRYPTION){
        blockAmount = (fileSize + ((8 - (fileSize % 8)) % 8)) / 8;
    }


    InputBlocks = (QWORD *)malloc(sizeof(QWORD) * blockAmount); // dynamic allocation
    OutputBlocks = (QWORD *)malloc(sizeof(QWORD) * blockAmount); // dynamic allocation
    memset(InputBlocks, 0, blockAmount * 8); // init as 0
    memset(OutputBlocks, 0, blockAmount * 8); // init as 0


    bytePtr = (BYTE *)(InputBlocks);
    printf("File Size: %dByte\n", fileSize);
    for(int i=0; i<fileSize; i++){
        fread((bytePtr + ((i / 8) * 8) + (7 - (i % 8))), 1, 1, ptrInputFile);
    }
    fclose(ptrInputFile);


    // padding: PKCS5
    BYTE paddingVal = 0;
    if(TaskMode == MODE_ENCRYPTION){
        // add padding:
        paddingVal = (blockAmount * 8) - fileSize;
        for(BYTE i = 0; i<paddingVal; i++){
            bytePtr[((blockAmount - 1) * 8) + i] = paddingVal;
        }
    }


    printf("Input With PKCS5 Padding:\n");
    for(int i=0; i<blockAmount; i++){
        TestPrintHex(InputBlocks[i]);
        printf("\n");
    }


    // ECB(Electroinc? Code Book) Mode:
    for(BYTE i=0; i<blockAmount; i++){
        if(TaskMode == MODE_ENCRYPTION){
            Encrypt(&(OutputBlocks[i]), &(InputBlocks[i]), &Key);
        }else if(TaskMode == MODE_DECRYPTION){
            Decrypt(&(OutputBlocks[i]), &(InputBlocks[i]), &Key);
        }
    }

    
    if(TaskMode == MODE_DECRYPTION){
        // remove padding:
        paddingVal = (OutputBlocks[blockAmount-1]) & 0xff;
        if(paddingVal == 8){
            // paddingVal == 8(byte): remove last block
            blockAmount--;
        }else{
            for(BYTE i=0; i<paddingVal; i++){
                OutputBlocks[blockAmount-1] = OutputBlocks[blockAmount-1] >> 8;
            }
            OutputBlocks[blockAmount-1] = OutputBlocks[blockAmount-1] << (paddingVal * 8);
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    printf("Output:\n");
    for(int i=0; i<blockAmount; i++){
        TestPrintHex(OutputBlocks[i]);
        printf("\n");
    }


    // Save to File:
    // File I/O:
    ptrOutputFile = fopen(argv[2], "w");
    bytePtr = (BYTE *)OutputBlocks;
    int OutputFileSize = 0;
    if(TaskMode == MODE_ENCRYPTION){
        OutputFileSize = blockAmount * 8;
    }else if(TaskMode == MODE_DECRYPTION){
        OutputFileSize = (blockAmount * 8) - (paddingVal % 8);
    }

    for(int i=0; i<OutputFileSize; i++){
        fwrite((bytePtr + ((i / 8) * 8) + (7 - (i % 8))), 1, 1, ptrInputFile);
    }
    fclose(ptrInputFile);


    // Exiting:
    free(InputBlocks);
    free(OutputBlocks);
    return 0;
}