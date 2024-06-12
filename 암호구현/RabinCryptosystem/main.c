#include <gmp.h>
#include <stdio.h>

void GetChineseRemainderTheorem_num_2(mpz_t result, mpz_t a1, mpz_t a2, mpz_t m1, mpz_t m2)
{
    mpz_t common_m, m1_div_common_m, m2_div_common_m, m1_div_common_m_inv, m2_div_common_m_inv;
    mpz_t tmp1, tmp2;
    mpz_inits(common_m, m1_div_common_m, m2_div_common_m, m1_div_common_m_inv, m2_div_common_m_inv, NULL);
    mpz_inits(tmp1, tmp2, NULL);

    // get common_m:
    mpz_mul(common_m, m1, m2);

    // get m1_div_common_m:
    mpz_fdiv_q(m1_div_common_m, common_m, m1);
    // get m2_div_common_m:
    mpz_fdiv_q(m2_div_common_m, common_m, m2);

    // get m1_div_common_m_inv:
    mpz_invert(m1_div_common_m_inv, m1_div_common_m, m1);
    // get m2_div_common_m_inv:
    mpz_invert(m2_div_common_m_inv, m2_div_common_m, m2);

    // get result:
    mpz_mul(tmp1, a1, m1_div_common_m);
    mpz_mul(tmp1, tmp1, m1_div_common_m_inv);
    mpz_mul(tmp2, a2, m2_div_common_m);
    mpz_mul(tmp2, tmp2, m2_div_common_m_inv);
    mpz_add(result, tmp1, tmp2);
    mpz_mod(result, result, common_m);

    mpz_clears(common_m, m1_div_common_m, m2_div_common_m, m1_div_common_m_inv, m2_div_common_m_inv, NULL);
    mpz_clears(tmp1, tmp2, NULL);
}

void Encrypt(mpz_t cipher, mpz_t plain, mpz_t n)
{
    mpz_powm_ui(cipher, plain, 2, n);
}

void Decrypt(mpz_t plains[4], mpz_t cipher, mpz_t p, mpz_t q)
{
    mpz_t exp1, exp2, tmp1, tmp2, a1, a2, b1, b2;
    mpz_inits(exp1, exp2, tmp1, tmp2, a1, a2, b1, b2, NULL);
    mpz_add_ui(tmp1, p, 1);       // tmp1 = p + 1
    mpz_add_ui(tmp2, q, 1);       // tmp2 = q + 1
    mpz_fdiv_q_ui(exp1, tmp1, 4); // exp1 = tmp1 / 4 = (p+1) / 4
    mpz_fdiv_q_ui(exp2, tmp2, 4); // exp2 = tmp2 / 4 = (q+1) / 4

    // a1:
    mpz_powm(a1, cipher, exp1, p); // a1 = (cipher ** exp1) mod p = (cipher ** ((p+1) / 4)) mod p

    // a2:
    mpz_powm(a2, cipher, exp1, p); // a2 = (cipher ** exp1) mod p = (cipher ** ((p+1) / 4)) mod p
    mpz_mul_si(a2, a2, -1);        // a2 = -(a2)
    mpz_mod(a2, a2, p);

    // b1:
    mpz_powm(b1, cipher, exp2, q); // b1 = (cipher ** exp2) mod p = (cipher ** ((q+1) / 4)) mod q

    // b2:
    mpz_powm(b2, cipher, exp2, q); // b2 = (cipher ** exp2) mod p = (cipher ** ((q+1) / 4)) mod q
    mpz_mul_si(b2, b2, -1);        // b2 = -(b2)
    mpz_mod(b2, b2, q);

    // get plains:
    GetChineseRemainderTheorem_num_2(plains[0], a1, b1, p, q);
    GetChineseRemainderTheorem_num_2(plains[1], a1, b2, p, q);
    GetChineseRemainderTheorem_num_2(plains[2], a2, b1, p, q);
    GetChineseRemainderTheorem_num_2(plains[3], a2, b2, p, q);

    mpz_clears(exp1, exp2, tmp1, tmp2, a1, a2, b1, b2, NULL);
}

int main(void)
{
    mpz_t p, q, n, plain, cipher;
    mpz_t plain2[4];
    mpz_inits(p, q, n, plain, cipher, NULL);
    mpz_inits(plain2[0], plain2[1], plain2[2], plain2[3], NULL);

    // test:
    mpz_set_ui(p, 23);
    mpz_set_ui(q, 7);
    mpz_set_ui(plain, 24);

    mpz_mul(n, p, q); // n = p * q
    Encrypt(cipher, plain, n);
    Decrypt(plain2, cipher, p, q);

    printf("Encryption:\n");
    gmp_printf("P=%Zd, C=%Zd\n", plain, cipher);

    printf("Decryption:\n");
    gmp_printf("C=%Zd\n", cipher);
    gmp_printf("P1=%Zd\n", plain2[0]);
    gmp_printf("P2=%Zd\n", plain2[1]);
    gmp_printf("P3=%Zd\n", plain2[2]);
    gmp_printf("P4=%Zd\n", plain2[3]);

    mpz_clears(p, q, n, plain, cipher, NULL);
    mpz_clears(plain2[0], plain2[1], plain2[2], plain2[3], NULL);

    return 0;
}