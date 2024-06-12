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

// 원소의 위수 구하기:
void GetElementOrd(mpz_t ord, mpz_t a, mpz_t n)
{
    mpz_t i, i_end, e, tmp1;
    mpz_inits(i, i_end, e, tmp1, NULL);
    mpz_set_ui(e, 1);        // 항등원 e = 1
    mpz_set_ui(i, 1);        // i = 1
    mpz_sub_ui(i_end, n, 1); // i_end = n - 1

    while (mpz_cmp(i_end, i) >= 0)
    {
        FastModuloExponentiation(tmp1, a, i, n); // 지수연산
        if (!mpz_cmp_ui(tmp1, 1))
        {
            // tmp1 == 1:
            mpz_set(ord, i);
            break;
        }

        mpz_add_ui(i, i, 1); // i++
    }

    mpz_clears(i, i_end, e, tmp1, NULL);
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

void KeyGeneration(mpz_t e1, mpz_t e2, mpz_t d, mpz_t p)
{
    mpz_t i, i_end, phi_p, tmp1;
    mpz_inits(i, i_end, phi_p, tmp1, NULL);

    // Get d:
    mpz_set_ui(i, 1);        // i = 1
    mpz_sub_ui(i_end, p, 2); // i_end = p - 2

    /*
    while (mpz_cmp(i_end, i) >= 0)
    {
        mpz_set(d, i); // d = i = d in Zp*

        mpz_add_ui(i, i, 1); // i++
    }
    */

    //mpz_set_ui(d, 3); // test
    mpz_set_ui(d, 1007); // test

    // Get e1:
    mpz_sub_ui(phi_p, p, 1); // phi_p = p - 1

    mpz_set_ui(i, 1);        // i = 1
    mpz_sub_ui(i_end, p, 1); // i_end = p - 1

    /*
    while (mpz_cmp(i_end, i) >= 0)
    {
        GetElementOrd(tmp1, i, p);
        if (!mpz_cmp(tmp1, phi_p))
        {
            // tmp1 == phi_p:
            mpz_set(e1, i);
        }

        mpz_add_ui(i, i, 1); // i++
    }
    */

    //mpz_set_ui(e1, 2); // test
    mpz_set_ui(e1, 2); // test

    // Get e2:
    FastModuloExponentiation(e2, e1, d, p);

    // end:
    mpz_clears(i, i_end, phi_p, tmp1, NULL);
}

void Encrypt(mpz_t cipher1, mpz_t cipher2, mpz_t plain, mpz_t e1, mpz_t e2, mpz_t p)
{
    mpz_t r, tmp1, tmp2;
    gmp_randstate_t randstate;
    mpz_inits(r, tmp1, tmp2, NULL);
    gmp_randinit_default(randstate);

    // Get r:
    mpz_sub_ui(tmp2, p, 1);           // tmp2 = p - 1
    mpz_urandomm(r, randstate, tmp2); // r = 0 ~ (tmp2 - 1) = 0 ~ (p - 2)
    mpz_add_ui(r, r, 1);              // r = r + 1 = 1 ~ (p - 1)

    //mpz_set_ui(r, 4); // test
    mpz_set_ui(r, 545131); // test

    // Get cipher1:
    FastModuloExponentiation(cipher1, e1, r, p);

    // Get cipher2:
    FastModuloExponentiation(tmp1, e2, r, p);
    mpz_mul(cipher2, plain, tmp1);
    mpz_mod(cipher2, cipher2, p);

    mpz_clears(r, tmp1, tmp2, NULL);
}

void Decrypt(mpz_t plain, mpz_t cipher1, mpz_t cipher2, mpz_t d, mpz_t p)
{
    mpz_t tmp1, cipher1_d_1;
    mpz_inits(tmp1, cipher1_d_1, NULL);

    FastModuloExponentiation(tmp1, cipher1, d, p);
    GetModularMultiplicativeInverse(cipher1_d_1, p, tmp1);
    mpz_mul(tmp1, cipher2, cipher1_d_1);
    mpz_mod(plain, tmp1, p);

    mpz_clears(tmp1, cipher1_d_1, NULL);
}

int main(void)
{
    mpz_t plain, cipher1, cipher2, e1, e2, p, plain2, d;
    mpz_inits(plain, cipher1, cipher2, e1, e2, p, plain2, d, NULL);

    /*
    // test:
    mpz_set_ui(plain, 7);
    mpz_set_ui(p, 11);
    */


    // test:
    mpz_set_str(p, "11534899272561676244925313717014331740490094532609834959814346921905689869862264593212975473787189514436889176526473093615929993728061165964347353440008577", 10);
    mpz_set_ui(plain, 3200);

    // Encrypt & Decrypt:
    KeyGeneration(e1, e2, d, p);
    Encrypt(cipher1, cipher2, plain, e1, e2, p);
    Decrypt(plain2, cipher1, cipher2, d, p);

    // print result:
    printf("KeyGeneration:\n");
    printf("Public Key:\n");
    gmp_printf("e1 = %Zd, e2 = %Zd, p = %Zd\n", e1, e2, p);
    printf("\n\n");
    printf("Private Key:\n");
    gmp_printf("d = %Zd\n", d);

    printf("\n\n");
    printf("Encrypt:\n");
    gmp_printf("Plain=%Zd, Cipher1=%Zd, Cipher2=%Zd\n", plain, cipher1, cipher2);

    printf("\n\n");
    printf("Decrypt:\n");
    gmp_printf("Plain=%Zd, Cipher1=%Zd, Cipher2=%Zd\n", plain2, cipher1, cipher2);

    mpz_clears(plain, cipher1, cipher2, e1, e2, p, plain2, d, NULL);
    return 0;
}