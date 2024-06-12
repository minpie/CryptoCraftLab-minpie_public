#include <gmp.h>
#include <stdio.h>

// 고속 모듈러 지수연산
void FastModuloExponentiation(mpz_t result, mpz_t a, mpz_t x, mpz_t n)
{
    mp_bitcnt_t bits_x, i;
    mpz_t tmp_a, y;
    mpz_inits(tmp_a, y, NULL);
    mpz_set(tmp_a, a);
    mpz_set_ui(y, 1); // y = 1
    i = 0;
    bits_x = mpz_sizeinbase(x, 2);

    while (bits_x > i)
    {
        if (mpz_tstbit(x, i))
        {
            // x의 i번째 비트가 1이면:
            mpz_mul(y, tmp_a, y); // y = tmp_a * y
            mpz_mod(y, y, n);     // y = y mod n
        }
        mpz_mul(tmp_a, tmp_a, tmp_a); // tmp_a = tmp_a * tmp_a
        mpz_mod(tmp_a, tmp_a, n);     // tmp_a = tmp_a mod n
        i = (mp_bitcnt_t)(i + 1);     // i++
    }

    mpz_set(result, y);

    mpz_clears(tmp_a, y, NULL);
}

// a, b의 GCD(최대공약수) 구하는 함수
void GetGCD(mpz_t result, mpz_t a, mpz_t b)
{
    // 유클리드 알고리즘 사용
    mpz_t r, r1, r2, q, tmp1;
    mpz_inits(r, r1, r2, q, tmp1, NULL);
    mpz_set(r1, a);
    mpz_set(r2, b);
    while (mpz_cmp_si(r2, 0) > 0)
    {
        mpz_fdiv_q(q, r1, r2); // q = r1 / r2
        mpz_mul(tmp1, q, r2);  // tmp1 = q * r2
        mpz_sub(r, r1, tmp1);  // r = r1 - tmp1 = r1 - (q * r2)
        mpz_set(r1, r2);       // r1 = r2
        mpz_set(r2, r);        // r2 = r
    }

    mpz_set(result, r1); // result = r1
    mpz_clears(r, r1, r2, q, tmp1, NULL);
}

// 모듈러 곱셈 역 구하기
void GetModularMultiplicativeInverse(mpz_t a_1, mpz_t n, mpz_t a)
{
    // 확장 유클리드 알고리즘 사용
    mpz_t q, r1, r2, r, t, t1, t2, tmp1, tmp2;
    mpz_inits(q, r1, r2, r, t, t1, t2, tmp1, tmp2, NULL);
    mpz_set(r1, n);    // r1 = n;
    mpz_set(r2, a);    // r2 = a
    mpz_set_si(t1, 0); // t1 = 0
    mpz_set_si(t2, 1); // t2 = 1

    while (mpz_cmp_si(r2, 0))
    {
        mpz_fdiv_q(q, r1, r2); // q = r1 / r2
        mpz_mul(tmp1, q, r2);  // tmp1 = q * r2
        mpz_sub(r, r1, tmp1);  // r = r1 - tmp1 = r1 - (q * r2)
        mpz_set(r1, r2);       // r1 = r2
        mpz_set(r2, r);        // r2 = r

        mpz_mul(tmp2, q, t2); // tmp2 = q * t2
        mpz_sub(t, t1, tmp2); // t = t1 - tmp2 = t1 - (q * t2)
        mpz_set(t1, t2);      // t1 = t2
        mpz_set(t2, t);       // t2 = t
    }
    mpz_set(a_1, t1);
    if (mpz_sgn(a_1) < 0)
    {
        mpz_add(a_1, n, a_1);
    }
    mpz_clears(q, r1, r2, r, t, t1, t2, tmp1, tmp2, NULL);
}

void KeyGeneration(mpz_t e, mpz_t d, mpz_t p, mpz_t q)
{
    mpz_t n, phi_n;
    mpz_t i;
    mpz_t tmp1, tmp2;
    mpz_inits(n, phi_n, NULL);
    mpz_inits(i, NULL);
    mpz_set_si(i, 2);
    mpz_inits(tmp1, tmp2, NULL);

    // get n , phi(n)
    mpz_mul(n, p, q);           // n = p * q;
    mpz_sub_ui(tmp1, p, 1);     // tmp1 = p - 1
    mpz_sub_ui(tmp2, q, 1);     // tmp2 = q - 1
    mpz_mul(phi_n, tmp1, tmp2); // phi_n = tmp1 * tmp2 = (p - 1) * (q - 1)
    printf("Done: Get n, phi_n\n");

    // get e
    mpz_set_si(tmp1, 0);
    mpz_set_si(tmp2, 0);
    while (mpz_cmp(phi_n, i))
    {
        // loop 조건: i < phi_n
        GetGCD(tmp1, phi_n, i); // tmp1 = gcd(phi_n, i)
        if (!mpz_cmp_si(tmp1, 1))
        {
            // gcd(phi_n, i) == 1
            mpz_set(e, i); // e = i
        }
        // TEST:
        if (!mpz_cmp_si(i, 35535))
        {
            break;
        }

        mpz_add_ui(i, i, 1); // i++
    }
    // mpz_set_si(e, 343); // TEST
    mpz_set_si(e, 35535); // TEST
    printf("Done: Get e\n");

    // get d
    GetModularMultiplicativeInverse(d, phi_n, e);
    printf("Done: Get d\n");

    // end:
    mpz_clears(n, phi_n, NULL);
    mpz_clears(i, NULL);
    mpz_clears(tmp1, tmp2, NULL);
}

void Encryption(mpz_t cipher, mpz_t plain, mpz_t n, mpz_t e)
{
    FastModuloExponentiation(cipher, plain, e, n);
}

void Decryption(mpz_t plain, mpz_t cipher, mpz_t n, mpz_t d)
{
    FastModuloExponentiation(plain, cipher, d, n);
}

int main(void)
{
    // start:
    mpz_t p, q, n, e, d;
    mpz_t plain, cipher, plain2;
    mpz_inits(p, q, n, e, d, NULL);
    mpz_inits(plain, cipher, plain2, NULL);

    // test:
    /*
    mpz_set_si(p, 397);
    mpz_set_si(q, 401);
    mpz_set_si(plain, 1314);
    */
    ///*
    mpz_set_str(p, "9613034531358350457419158128061542790930984559499621582258315087964794045505647063849125716018034750312098666606492420191808780667421096063354219926661209", 10);
    mpz_set_str(q, "12060191957231446918276794204450896001555925054637033936061798321731482148483764659215389453209175225273226830107120695604602513887145524969000359660045617", 10);
    mpz_set_str(plain, "1907081826081826002619041819", 10);
    //*/

    // get n, e, d
    
    /*
    
    mpz_mul(n, p, q); // n = p = q;
    KeyGeneration(e, d, p, q);

    gmp_printf("Public Key (n, e)=(%Zd, %Zd)\n", n, e);
    gmp_printf("Private Key (d)=(%Zd)\n", d);

    Encryption(cipher, plain, n, e);
    Decryption(plain2, cipher, n, d);

    gmp_printf("Encryption: P=%Zd, C=%Zd\n", plain, cipher);
    gmp_printf("Decryption: C=%Zd, P=%Zd\n", cipher, plain2);


    */
    mpz_set_str(p, "9797", 10);
    mpz_set_str(q, "131", 10);

    GetModularMultiplicativeInverse(n, p, q);
    gmp_printf("Inv=%Zd\n", n);



    // end:
    mpz_clears(p, q, n, e, d, NULL);
    mpz_clears(plain, cipher, plain2, NULL);
    return 0;
}