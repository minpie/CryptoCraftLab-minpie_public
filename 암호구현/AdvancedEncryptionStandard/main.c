/*
main.c
AES-128 C 구현
*/
// ##### Start Code
// #### include:
#include <stdio.h>
#include <string.h>


// #### typedef
typedef unsigned char UINT8; // 부호없는 8비트 정수
typedef unsigned short UINT16; // 부호없는 16비트 정수


// #### 함수 선언:
// ### 일반 함수:
void CopyBytes(UINT8 * dst, const UINT8 * src, const UINT16 siz);
UINT8 GetBitAmount(UINT8 n);
// ### 테스트용 출력 함수:
void TestPrint16Byte(const UINT8 * some16Bytes, const UINT8 isState);
// ### AES 관련 함수:
UINT8 Mul2(UINT8 a);
UINT8 Mul3(UINT8 a);
UINT8 MulGF256(UINT8 a0, UINT8 b0);
void SubWord(UINT8 * wrd);
void RotWord(UINT8 * wrd);
void KeyExpansion(UINT8 * roundKeys, const UINT8 * initialKey);
void AddRoundKey(UINT8 * state, const UINT8 * roundKey);
void SubBytes(UINT8 * state);
void InvSubBytes(UINT8 * state);
void ShiftRows(UINT8 * state);
void InvShiftRows(UINT8 * state);
void MixColumns(UINT8 * state);
void InvMixColumns(UINT8 * state);
void ConvInputToState(UINT8 * state, const UINT8 * input);
void ConvStateToOutput(UINT8 * output, const UINT8 * state);
void Encrypt(UINT8 * cipher, const UINT8 * plain, const UINT8 * initialKey);
void Decrypt(UINT8 * plain, const UINT8 * cipher, const UINT8 * initialKey);


// #### main():
int main(void){
    //
    // 초기값:
    UINT8 testPlain[16] = {0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    UINT8 testCipher[16] = {0, };
    UINT8 testKey[16] = {0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    UINT8 test2Plain[16] = {0, };
    UINT8 test2Cipher[16];
    UINT8 test2Key[16] = {0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    
    // 암호화:
    Encrypt(testCipher, testPlain, testKey); // 암호화
    //printf("\n\n\n");
    // 복호화:
    CopyBytes(test2Cipher, testCipher, 16);
    Decrypt(test2Plain, test2Cipher, test2Key); // 복호화

    // 같은지 확인:
    printf("(testPlain == test2Plain): %d\n", (memcmp(testPlain, test2Plain, 16) == 0));

    return 0;
}


// #### 함수 정의:
// ### 일반 함수:
void CopyBytes(UINT8 * dst, const UINT8 * src, const UINT16 siz){
    // ## src에서 siz 바이트만큼 dst에 복사
    for(UINT16 i=0; i<siz; i++){
        dst[i] = src[i];
    }
}
UINT8 GetBitAmount(UINT8 n){
    // ## n의 2진 자릿수 반환
    UINT8 count = 0;
    while(n > 0){
        n = n >> 1;
        count++;
    }
    return count;
}


// ### 테스트용 출력 함수:
void TestPrint16Byte(const UINT8 * some16Bytes, const UINT8 isState){
    // ## 16바이트를 출력
    if(isState){
        for(UINT8 i=0; i<4; i++){
            printf("%02x%02x%02x%02x", (int)some16Bytes[i], (int)some16Bytes[i+4], (int)some16Bytes[i+8], (int)some16Bytes[i+12]);
        }
    }else{
        for(UINT8 i=0; i<16; i++){
            printf("%02x", (int)some16Bytes[i]);
        }
    }
}


// ### AES 관련 함수:
UINT8 Mul2(UINT8 a)
{
    // ## Multiplacation by 2 in GF(2^8).
    UINT8 b = (a >> 7) & 0b1;
    UINT8 c = a << 1; // c(x) = x * b(x)
    if (b)
    {
        c = c ^ 0x1b; // c(x) = c(x) mod p(x)
    }
    return c;
}
UINT8 Mul3(UINT8 a)
{
    // ## Multiplacation by 3 in GF(2^8).
    UINT8 b;
    b = Mul2(a) ^ a; // b(x) = (x * b(x)) + (1 * b(x))
    return b;
}
UINT8 MulGF256(UINT8 a0, UINT8 b0){
    // ## Multiplacation in GF(2^8)
    UINT8 nTimes = GetBitAmount(a0);
    UINT8 b = 0;
    UINT8 c = 0;
    UINT8 d = 0;

    for(UINT8 i=0; i<nTimes; i++){
        c = ((a0 >> i) & 0b1) << i;
        d = 0;
        if(c){
            d = b0;
            while(c > 1){
                d = Mul2(d);
                c /= 2;
            }
        }

        b = b ^ d;
    }
    return b;
}
void SubWord(UINT8 * wrd){
    // ## SubWord 함수
    // # 0. 변수 초기화
    UINT8 temp[4]; // 임시 변수
    const UINT8 Sbox[16][16] = {
        {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
        {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
        {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
        {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
        {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
        {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
        {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
        {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
        {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
        {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
        {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
        {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
        {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
        {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
        {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
        {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
    };
    // # 1.연산:
    CopyBytes(temp, wrd, 4);
    for(UINT8 i=0; i<4; i++){
        wrd[i] = Sbox[(temp[i] >> 4)][(temp[i] & 0x0f)]; // Sbox 계산
    }
}
void RotWord(UINT8 * wrd){
    // ## RotWord 함수
    // # 0. 변수 초기화
    UINT8 temp[4];

    // # 1. 연산:
    CopyBytes(temp, wrd, 4);
    wrd[0] = temp[1];
    wrd[1] = temp[2];
    wrd[2] = temp[3];
    wrd[3] = temp[0];
}
void KeyExpansion(UINT8 * roundKeys, const UINT8 * initialKey){
    // ## KeyExpansion 함수
    // # 0. 변수 초기화
    UINT8 temp1[4];
    UINT8 temp2[4];
    const UINT8 RoundConstant[10][4] = {
        {0x01, 0, 0, 0},
        {0x02, 0, 0, 0},
        {0x04, 0, 0, 0},
        {0x08, 0, 0, 0},
        {0x10, 0, 0, 0},
        {0x20, 0, 0, 0},
        {0x40, 0, 0, 0},
        {0x80, 0, 0, 0},
        {0x1b, 0, 0, 0},
        {0x36, 0, 0, 0}
    };


    // # 1. w0~w3:
    CopyBytes(roundKeys, initialKey, 16);

    // # 2. w4~w43:
    for(UINT8 i=4; i<44; i++){
        CopyBytes(temp1, &(roundKeys[(i-1)*4]), 4); // temp = w[i-1]
        if(!(i % 4)){
            // temp1 = SubWord(RotWord(temp1)):
            RotWord(temp1);
            SubWord(temp1);

            // temp1 = temp1 XOR RoundConstant:
            temp2[0] = temp1[0] ^ RoundConstant[(i/4)-1][0];
            temp2[1] = temp1[1] ^ RoundConstant[(i/4)-1][1];
            temp2[2] = temp1[2] ^ RoundConstant[(i/4)-1][2];
            temp2[3] = temp1[3] ^ RoundConstant[(i/4)-1][3];
            CopyBytes(temp1, temp2, 4); // temp1 = temp2
        }
        // # w[i] = w[i-4] ^ temp1:
        roundKeys[(i*4)] = roundKeys[((i-4)*4)] ^ temp1[0];
        roundKeys[(i*4)+1] = roundKeys[((i-4)*4)+1] ^ temp1[1];
        roundKeys[(i*4)+2] = roundKeys[((i-4)*4)+2] ^ temp1[2];
        roundKeys[(i*4)+3] = roundKeys[((i-4)*4)+3] ^ temp1[3];
    }
}
void AddRoundKey(UINT8 * state, const UINT8 * roundKey){
    // ## AddRoundKey 함수
    // # 0. 변수 초기화
    UINT8 temp[16]; // 임시 변수

    // # 1. 연산:
    CopyBytes(temp, state, 16);
    for(UINT8 i=0; i<4; i++){
        state[i] = temp[i] ^ roundKey[(i*4)];
        state[i+4] = temp[i+4] ^ roundKey[(i*4)+1];
        state[i+8] = temp[i+8] ^ roundKey[(i*4)+2];
        state[i+12] = temp[i+12] ^ roundKey[(i*4)+3];
    }
}
void SubBytes(UINT8 * state){
    // ## SubBytes 함수
    // # 0. 변수 초기화
    UINT8 temp[16]; // 임시 변수
    const UINT8 Sbox[16][16] = {
        {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
        {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
        {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
        {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
        {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
        {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
        {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
        {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
        {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
        {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
        {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
        {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
        {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
        {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
        {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
        {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
    };
    // # 1.연산:
    CopyBytes(temp, state, 16);
    for(UINT8 i=0; i<16; i++){
        state[i] = Sbox[(temp[i] >> 4)][(temp[i] & 0x0f)]; // Sbox 계산
    }
}
void InvSubBytes(UINT8 * state){
    // ## InvSubBytes 함수
    // # 0. 변수 초기화
    UINT8 temp[16]; // 임시 변수
    const UINT8 Sbox[16][16] = {
        {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
        {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
        {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
        {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
        {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
        {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
        {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
        {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
        {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
        {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
        {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
        {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
        {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
        {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
        {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
        {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}
    };

    // # 1.연산:
    CopyBytes(temp, state, 16);
    for(UINT8 i=0; i<16; i++){
        state[i] = Sbox[(temp[i] >> 4)][(temp[i] & 0x0f)]; // Sbox 계산
    }
}
void ShiftRows(UINT8 * state){
    // ## ShiftRows 함수
    // # 0. 변수 초기화
    UINT8 temp[16]; // 임시 변수
    // # 1.연산:
    CopyBytes(temp, state, 16);

    state[0] = temp[0];
    state[1] = temp[1];
    state[2] = temp[2];
    state[3] = temp[3];

    state[4] = temp[5];
    state[5] = temp[6];
    state[6] = temp[7];
    state[7] = temp[4];

    state[8] = temp[10];
    state[9] = temp[11];
    state[10] = temp[8];
    state[11] = temp[9];

    state[12] = temp[15];
    state[13] = temp[12];
    state[14] = temp[13];
    state[15] = temp[14];

}
void InvShiftRows(UINT8 * state){
    // ## InvShiftRows 함수
    // # 0. 변수 초기화
    UINT8 temp[16]; // 임시 변수
    // # 1.연산:
    CopyBytes(temp, state, 16);

    state[0] = temp[0];
    state[1] = temp[1];
    state[2] = temp[2];
    state[3] = temp[3];

    state[4] = temp[7];
    state[5] = temp[4];
    state[6] = temp[5];
    state[7] = temp[6];

    state[8] = temp[10];
    state[9] = temp[11];
    state[10] = temp[8];
    state[11] = temp[9];

    state[12] = temp[13];
    state[13] = temp[14];
    state[14] = temp[15];
    state[15] = temp[12];
}
void MixColumns(UINT8 * state){
    // ## MixColumns 함수
    UINT8 temp[16];
    CopyBytes(temp, state, 16);

    state[0] = (MulGF256(0x2, temp[0])) ^ (MulGF256(0x3, temp[4])) ^ (MulGF256(0x1, temp[8])) ^ (MulGF256(0x1, temp[12]));
    state[4] = (MulGF256(0x1, temp[0])) ^ (MulGF256(0x2, temp[4])) ^ (MulGF256(0x3, temp[8])) ^ (MulGF256(0x1, temp[12]));
    state[8] = (MulGF256(0x1, temp[0])) ^ (MulGF256(0x1, temp[4])) ^ (MulGF256(0x2, temp[8])) ^ (MulGF256(0x3, temp[12]));
    state[12] = (MulGF256(0x3, temp[0])) ^ (MulGF256(0x1, temp[4])) ^ (MulGF256(0x1, temp[8])) ^ (MulGF256(0x2, temp[12]));

    state[1] = (MulGF256(0x2, temp[1])) ^ (MulGF256(0x3, temp[5])) ^ (MulGF256(0x1, temp[9])) ^ (MulGF256(0x1, temp[13]));
    state[5] = (MulGF256(0x1, temp[1])) ^ (MulGF256(0x2, temp[5])) ^ (MulGF256(0x3, temp[9])) ^ (MulGF256(0x1, temp[13]));
    state[9] = (MulGF256(0x1, temp[1])) ^ (MulGF256(0x1, temp[5])) ^ (MulGF256(0x2, temp[9])) ^ (MulGF256(0x3, temp[13]));
    state[13] = (MulGF256(0x3, temp[1])) ^ (MulGF256(0x1, temp[5])) ^ (MulGF256(0x1, temp[9])) ^ (MulGF256(0x2, temp[13]));

    state[2] = (MulGF256(0x2, temp[2])) ^ (MulGF256(0x3, temp[6])) ^ (MulGF256(0x1, temp[10])) ^ (MulGF256(0x1, temp[14]));
    state[6] = (MulGF256(0x1, temp[2])) ^ (MulGF256(0x2, temp[6])) ^ (MulGF256(0x3, temp[10])) ^ (MulGF256(0x1, temp[14]));
    state[10] = (MulGF256(0x1, temp[2])) ^ (MulGF256(0x1, temp[6])) ^ (MulGF256(0x2, temp[10])) ^ (MulGF256(0x3, temp[14]));
    state[14] = (MulGF256(0x3, temp[2])) ^ (MulGF256(0x1, temp[6])) ^ (MulGF256(0x1, temp[10])) ^ (MulGF256(0x2, temp[14]));

    state[3] = (MulGF256(0x2, temp[3])) ^ (MulGF256(0x3, temp[7])) ^ (MulGF256(0x1, temp[11])) ^ (MulGF256(0x1, temp[15]));
    state[7] = (MulGF256(0x1, temp[3])) ^ (MulGF256(0x2, temp[7])) ^ (MulGF256(0x3, temp[11])) ^ (MulGF256(0x1, temp[15]));
    state[11] = (MulGF256(0x1, temp[3])) ^ (MulGF256(0x1, temp[7])) ^ (MulGF256(0x2, temp[11])) ^ (MulGF256(0x3, temp[15]));
    state[15] = (MulGF256(0x3, temp[3])) ^ (MulGF256(0x1, temp[7])) ^ (MulGF256(0x1, temp[11])) ^ (MulGF256(0x2, temp[15]));
}
void InvMixColumns(UINT8 * state){
    // ## InvMixColumns 함수
    // # Inverse function of MixColumns().
    UINT8 temp[16];            // 전체 복사한 값
    CopyBytes(temp, state, 16); // 복사

    state[0] = (MulGF256(0xe, temp[0])) ^ (MulGF256(0xb, temp[4])) ^ (MulGF256(0xd, temp[8])) ^ (MulGF256(0x9, temp[12]));
    state[4] = (MulGF256(0x9, temp[0])) ^ (MulGF256(0xe, temp[4])) ^ (MulGF256(0xb, temp[8])) ^ (MulGF256(0xd, temp[12]));
    state[8] = (MulGF256(0xd, temp[0])) ^ (MulGF256(0x9, temp[4])) ^ (MulGF256(0xe, temp[8])) ^ (MulGF256(0xb, temp[12]));
    state[12] = (MulGF256(0xb, temp[0])) ^ (MulGF256(0xd, temp[4])) ^ (MulGF256(0x9, temp[8])) ^ (MulGF256(0xe, temp[12]));

    state[1] = (MulGF256(0xe, temp[1])) ^ (MulGF256(0xb, temp[5])) ^ (MulGF256(0xd, temp[9])) ^ (MulGF256(0x9, temp[13]));
    state[5] = (MulGF256(0x9, temp[1])) ^ (MulGF256(0xe, temp[5])) ^ (MulGF256(0xb, temp[9])) ^ (MulGF256(0xd, temp[13]));
    state[9] = (MulGF256(0xd, temp[1])) ^ (MulGF256(0x9, temp[5])) ^ (MulGF256(0xe, temp[9])) ^ (MulGF256(0xb, temp[13]));
    state[13] = (MulGF256(0xb, temp[1])) ^ (MulGF256(0xd, temp[5])) ^ (MulGF256(0x9, temp[9])) ^ (MulGF256(0xe, temp[13]));

    state[2] = (MulGF256(0xe, temp[2])) ^ (MulGF256(0xb, temp[6])) ^ (MulGF256(0xd, temp[10])) ^ (MulGF256(0x9, temp[14]));
    state[6] = (MulGF256(0x9, temp[2])) ^ (MulGF256(0xe, temp[6])) ^ (MulGF256(0xb, temp[10])) ^ (MulGF256(0xd, temp[14]));
    state[10] = (MulGF256(0xd, temp[2])) ^ (MulGF256(0x9, temp[6])) ^ (MulGF256(0xe, temp[10])) ^ (MulGF256(0xb, temp[14]));
    state[14] = (MulGF256(0xb, temp[2])) ^ (MulGF256(0xd, temp[6])) ^ (MulGF256(0x9, temp[10])) ^ (MulGF256(0xe, temp[14]));

    state[3] = (MulGF256(0xe, temp[3])) ^ (MulGF256(0xb, temp[7])) ^ (MulGF256(0xd, temp[11])) ^ (MulGF256(0x9, temp[15]));
    state[7] = (MulGF256(0x9, temp[3])) ^ (MulGF256(0xe, temp[7])) ^ (MulGF256(0xb, temp[11])) ^ (MulGF256(0xd, temp[15]));
    state[11] = (MulGF256(0xd, temp[3])) ^ (MulGF256(0x9, temp[7])) ^ (MulGF256(0xe, temp[11])) ^ (MulGF256(0xb, temp[15]));
    state[15] = (MulGF256(0xb, temp[3])) ^ (MulGF256(0xd, temp[7])) ^ (MulGF256(0x9, temp[11])) ^ (MulGF256(0xe, temp[15]));
}
void ConvInputToState(UINT8 * state, const UINT8 * input){
    // ## Input to State 변환 함수
    state[0] = input[0];
    state[1] = input[4];
    state[2] = input[8];
    state[3] = input[12];

    state[4] = input[1];
    state[5] = input[5];
    state[6] = input[9];
    state[7] = input[13];

    state[8] = input[2];
    state[9] = input[6];
    state[10] = input[10];
    state[11] = input[14];

    state[12] = input[3];
    state[13] = input[7];
    state[14] = input[11];
    state[15] = input[15];
}
void ConvStateToOutput(UINT8 * output, const UINT8 * state){
    // ## State to Output 변환 함수
    output[0] = state[0];
    output[4] = state[1];
    output[8] = state[2];
    output[12] = state[3];

    output[1] = state[4];
    output[5] = state[5];
    output[9] = state[6];
    output[13] = state[7];

    output[2] = state[8];
    output[6] = state[9];
    output[10] = state[10];
    output[14] = state[11];

    output[3] = state[12];
    output[7] = state[13];
    output[11] = state[14];
    output[15] = state[15];
}
void Encrypt(UINT8 * cipher, const UINT8 * plain, const UINT8 * initialKey){
    // ## 단일 블록 암호화 함수
    // # 0. 변수 초기화
    UINT8 state[16]; // 내부 state
    UINT8 roundKey[176]; // roundkey
    // printf("round[00].input : "); TestPrint16Byte(plain, 0); printf("\n"); // test print


    // # 1. Key Expansion
    KeyExpansion(roundKey, initialKey); // key expansion
    // printf("round[00].k_sch : ");TestPrint16Byte(&(roundKey[0]), 0); printf("\n");// test print


    // # 2. Input To State
    ConvInputToState(state, plain); // plain to state


    // # 3. Rounds
    AddRoundKey(state, &(roundKey[0]));
    // printf("round[01].start : "); TestPrint16Byte(state, 1); printf("\n"); // test print
    for(UINT8 rnd=1; rnd<10; rnd++){
        // SubBytes
        SubBytes(state);
        // printf("round[%02d].s_box : ", rnd); TestPrint16Byte(state, 1); printf("\n"); // test print

        // ShiftRows
        ShiftRows(state);
        // printf("round[%02d].s_row : ", rnd); TestPrint16Byte(state, 1); printf("\n"); // test print

        // MixColumns
        MixColumns(state);
        // printf("round[%02d].m_col : ", rnd); TestPrint16Byte(state, 1); printf("\n"); // test print

        // AddRoundKey
        AddRoundKey(state, &(roundKey[(rnd*16)]));
        // printf("round[%02d].k_sch : ", rnd); TestPrint16Byte(&(roundKey[rnd*16]), 0); printf("\n"); // test print
        // printf("round[%02d].start : ", rnd+1); TestPrint16Byte(state, 1); printf("\n"); // test print
    }
    // SubBytes
    SubBytes(state);
    // printf("round[10].s_box : "); TestPrint16Byte(state, 1); printf("\n"); // test print

    // ShiftRows
    ShiftRows(state);
    // printf("round[10].s_row : "); TestPrint16Byte(state, 1); printf("\n"); // test print

    // AddRoundKey
    AddRoundKey(state, &(roundKey[160]));
    // printf("round[10].k_sch : "); TestPrint16Byte(&(roundKey[160]), 0); printf("\n"); // test print


    // # 4. State To Output
    ConvStateToOutput(cipher, state); // state to cipher
    // printf("round[10].output: "); TestPrint16Byte(cipher, 0); printf("\n"); // test print
}
void Decrypt(UINT8 * plain, const UINT8 * cipher, const UINT8 * initialKey){
    // ## 단일 블록 복호화 함수
    // # 0. 변수 초기화
    UINT8 state[16]; // 내부 state
    UINT8 roundKey[176]; // roundkey
    // printf("round[00].input : "); TestPrint16Byte(cipher, 0); printf("\n"); // test print


    // # 1. Key Expansion
    KeyExpansion(roundKey, initialKey); // key expansion
    // printf("round[00].k_sch : ");TestPrint16Byte(&(roundKey[160]), 0); printf("\n");// test print


    // # 2. Input To State
    ConvInputToState(state, cipher); // plain to state


    // # 3. Rounds
    AddRoundKey(state, &(roundKey[160]));
    // printf("round[01].start : "); TestPrint16Byte(state, 1); printf("\n"); // test print
    for(UINT8 rnd=9; rnd>0; rnd--){
        // InvShiftRows
        InvShiftRows(state);
        // printf("round[%02d].s_row : ", (10-rnd)); TestPrint16Byte(state, 1); printf("\n"); // test print

        // InvSubBytes
        InvSubBytes(state);
        // printf("round[%02d].s_box : ", (10-rnd)); TestPrint16Byte(state, 1); printf("\n"); // test print

        // AddRoundKey
        AddRoundKey(state, &(roundKey[(rnd*16)]));
        // printf("round[%02d].k_sch : ", (10-rnd));TestPrint16Byte(&(roundKey[(rnd*16)]), 0); printf("\n");// test print
        // printf("round[%02d].k_add : ", (11-rnd)); TestPrint16Byte(state, 1); printf("\n"); // test print

        // InvMixColumns
        InvMixColumns(state);
        // printf("round[%02d].start : ", (11-rnd)); TestPrint16Byte(state, 1); printf("\n"); // test print
    }
    // InvShiftRows
    InvShiftRows(state);
    // printf("round[10].s_row : "); TestPrint16Byte(state, 1); printf("\n"); // test print

    // InvSubBytes
    InvSubBytes(state);
    // printf("round[10].s_box : "); TestPrint16Byte(state, 1); printf("\n"); // test print

    // AddRoundKey
    AddRoundKey(state, &(roundKey[(0)]));
    // printf("round[10].k_sch : ");TestPrint16Byte(&(roundKey[0]), 0); printf("\n");// test print


    // # 4. State To Output
    ConvStateToOutput(plain, state); // state to cipher
    // printf("round[10].output: "); TestPrint16Byte(plain, 0); printf("\n"); // test print
}
// #### End Code