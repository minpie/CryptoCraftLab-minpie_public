/*
main.c
*/
#include <stdio.h>
#include <math.h>


typedef long long int INT64;

typedef struct PublicKeyType{
    INT64 e;
    INT64 n;
}PublicKey;

typedef struct PrivateKeyType{
    INT64 d;
}PrivateKey;

typedef struct KeyType{
    PublicKey public_key;
    PrivateKey private_key;
}Key;


INT64 GetBitAmount(INT64 n){
    INT64 count = 0;
    while(n > 0){
        n = n >> 1;
        count++;
    }
    return count;
}



INT64 FastModuloExponentiation(INT64 a, INT64 x, INT64 n){
    INT64 y = 1;
    INT64 bits_x = 0;
    bits_x = GetBitAmount(x);
    for(INT64 i=0; i<bits_x; i++){
        if(((x >> i) & 0b1)){
            y = (a * y) % n;
        }
        a = (a * a) % n;
    }
    return y;
}

INT64 FastExponentiation(INT64 a, INT64 x){
    INT64 y = 1;
    INT64 bits_x = 0;
    bits_x = GetBitAmount(x);
    for(INT64 i=0; i<bits_x; i++){
        if(((x >> i) & 0b1)){
            y = (a * y);
        }
        a = (a * a);
    }
    return y;
}


// a, b의 GCD(최대공약수) 구하는 함수
INT64 GetGCD(INT64 a, INT64 b){
    // 유클리드 알고리즘 사용
    INT64 r, r1, r2;
    r1 = a;
    r2 = b;
    while(r2 > 0){
        INT64 q;
        q = r1 / r2;
        r = r1 - (q * r2);
        r1 = r2;
        r2 = r;
    }
    return r1;
}

// 모듈러 곱셈 역 구하기
INT64 GetModularMultiplicativeInverse(INT64 n, INT64 a){
    INT64 a_1 = 0;
    // 확장 유클리드 알고리즘 사용
    INT64 q, r1, r2, r, t, t1, t2;
    r1 = n;
    r2 = a;
    t1 = 0;
    t2 = 1;
    while(r2 > 0){
        q = r1 / r2;
        r = r1 - (q * r2);
        r1 = r2;
        r2 = r;

        t = t1 - (q * t2);
        t1 = t2;
        t2 = t;
    }
    a_1 = t1;
    if(a_1 < 0){
        a_1 = n + a_1;
    }

    return a_1;
}

Key KeyGeneration(INT64 p, INT64 q){
    INT64 n = p * q;
    INT64 phi_n = (p - 1) * (q - 1);

    INT64 e = 0;
    for(INT64 i=2; i<phi_n; i++){
        if(GetGCD(phi_n, i) == 1){
            e = i;
        }
    }
    e = 343;

    INT64 d = 0;
    d = GetModularMultiplicativeInverse(phi_n, e);


    Key generated_key;
    generated_key.private_key.d = d;
    generated_key.public_key.e = e;
    generated_key.public_key.n = n;

    return generated_key;
}

INT64 Encryption(INT64 plain, INT64 e, INT64 n){
    INT64 cipher = FastModuloExponentiation(plain, e, n);

    return cipher;
}

INT64 Decryption(INT64 cipher, INT64 d, INT64 n){
    INT64 plain = FastModuloExponentiation(cipher, d, n);
    return plain;
}





int main(void){
    Key k = KeyGeneration(397, 401);
    INT64 plain = 1314;
    INT64 cipher = 0;
    INT64 plain2 = 0;
    printf("Public Key (n, e)=(%llu, %llu)\n", k.public_key.n, k.public_key.e);
    printf("Private Key (d)=(%llu)\n", k.private_key.d);

    cipher = Encryption(plain, k.public_key.e, k.public_key.n);
    plain2 = Decryption(cipher, k.private_key.d, k.public_key.n);
    printf("Encryption: P=%llu, C=%llu\n", plain, cipher);
    printf("Decryption: C=%llu, P=%llu\n", cipher, plain2);



    return 0;
}