#include <gmp.h>
#include <stdio.h>

void KeyGeneration(mpz_t e, mpz_t d, mpz_t p, mpz_t q)
{
    mpz_t n, phi_n;
    mpz_t i;
    mpz_t tmp1, tmp2;
    mpz_inits(n, phi_n, NULL);
    mpz_inits(i, NULL);
    mpz_inits(tmp1, tmp2, NULL);

    // get n , phi(n)
    mpz_mul(n, p, q);           // n = p = q;
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
        mpz_gcd(tmp1, phi_n, i); // tmp1 = gcd(phi_n, i)
        if (!mpz_cmp_si(tmp1, 1))
        {
            // gcd(phi_n, i) == 1
            mpz_set(e, i); // e = i
        }
        // TEST:
        if(!mpz_cmp_si(i, 35535)){
            break;
        }

        mpz_add_ui(i, i, 1); // i++
    }
    // mpz_set_si(e, 343); // TEST
    mpz_set_si(e, 35535); // TEST
    printf("Done: Get e\n");

    // get d
    mpz_invert(d, e, phi_n);
    printf("Done: Get d\n");

    // end:
    mpz_clears(n, phi_n, NULL);
    mpz_clears(i, NULL);
    mpz_clears(tmp1, tmp2, NULL);
}

void Encryption(mpz_t cipher, mpz_t plain, mpz_t n, mpz_t e)
{
    mpz_powm(cipher, plain, e, n);
}

void Decryption(mpz_t plain, mpz_t cipher, mpz_t n, mpz_t d)
{
    mpz_powm(plain, cipher, d, n);
}

int main(void)
{
    // start:
    mpz_t p, q, n, e, d;
    mpz_t plain, cipher, plain2;
    mpz_inits(p, q, n, e, d, NULL);
    mpz_inits(plain, cipher, plain2, NULL);

    // test:
    // mpz_set_si(p, 397);
    // mpz_set_si(q, 401);
    // mpz_set_si(plain, 1314);
    mpz_set_str(p, "9613034531358350457419158128061542790930984559499621582258315087964794045505647063849125716018034750312098666606492420191808780667421096063354219926661209", 10);
    mpz_set_str(q, "12060191957231446918276794204450896001555925054637033936061798321731482148483764659215389453209175225273226830107120695604602513887145524969000359660045617", 10);
    mpz_set_str(plain, "1907081826081826002619041819", 10);

    // get n, e, d
    mpz_mul(n, p, q); // n = p = q;
    KeyGeneration(e, d, p, q);

    gmp_printf("Public Key (n, e)=(%Zd, %Zd)\n", n, e);
    gmp_printf("Private Key (d)=(%Zd)\n", d);

    Encryption(cipher, plain, n, e);
    Decryption(plain2, cipher, n, d);

    gmp_printf("Encryption: P=%Zd, C=%Zd\n", plain, cipher);
    gmp_printf("Decryption: C=%Zd, P=%Zd\n", cipher, plain2);

    // end:
    mpz_clears(p, q, n, e, d, NULL);
    mpz_clears(plain, cipher, plain2, NULL);
    return 0;
}