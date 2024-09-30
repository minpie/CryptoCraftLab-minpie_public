#include <stdio.h>
#include <gmp.h>

int main(void)
{
    mpz_t data_integer;          // 정수
    mpq_t data_quotient;         // 유리수
    mpf_t data_fp;               // 부동소수점 가수, short 크기
    mp_exp_t data_exp;           // 부동소수점 지수, long 크기
    mp_limb_t data_limb;         // 단일기계어에 맞는 다중 정밀도 숫자의 일부?, 32/64비트 크기
    mp_size_t data_limb_size;    // Counts of mp_limb_t, int/long/long long 크기
    mp_bitcnt_t data_bit_size;   // 다중 정밀도 숫자의 비트 수, 부호없는 long/부호없는 long long 크기
    gmp_randstate_t data_rstate; // 알고리즘 선택/현재 상태 데이터?

    /*
    gmp 라이브러리의 함수 종류 5가지:

    부호 있는 정수를 위한 연산:
    함수명이 mpz_ 로 시작함, mpz_t 자료형을 사용함.

    유리수를 위한 연산:
    함수명이 mpq_ 로 시작함, mpq_t 자료형을 사용함.

    부동소수점 연산:
    함수명이 mpf_ 로 시작함, mpf_t 자료형을 사용함.

    자연수를 위한 고속 로우레벨 연산:
    함수명이 mpn_ 로 시작함, mp_limb_t 배열을 사용함, 직접 호출해서 사용할 수도 있음.

    기타 함수:
    사용자 지정 할당 혹은 난수 생성 기능 등
    */

    /*
    gmp 라이브러리 변수 규칙

    - 일반적으로 gmp 함수는 출력인자가 입력 인자보다 먼저 옴.
    - 한번의 호출에서 입력과 출력 모두에 대해 동일한 변수를 사용가능함. mpz_mul(x, x, x); 같은 형태가 가능하다는 것.
    - gmp 변수에 값을 할당하기 전에, 특별한 초기화 함수를 사용해야 함.
    - gmp 변수를 다 사용하고 나면, 특별한 함수를 사용해 변수를 해제해야 함.
    - 그 이름은 정수/유리수/부동소수점 등 타입에 따라 다름.
    - gmp 변수는 한번만 초기화되거나/ clear(변수 해제?)된 이후에 초기화되어야 함.
    - 변수가 초기화되면 얼마든지 할당할 수 있음(=사용할 수 있다?)
    - 효율을 위해, 과도한 초기화/삭제는 피하는 것이 좋다.
    - 보통, 시작과 끝에서 초기화/삭제를 수행한다.
    - 동작 구조 및 효율성의 이유로, gmp 개체 자체의 사본을 만드는 것은 권장하지 않음.
    */

    /*
    gmp 라이브러리 인자 규칙
    - gmp 변수가 함수의 인자로 쓰일때는, call-by-reference로 동작한다.
    - 이러한 것에 const를 사용시, 컴파일러 오류나 경고를 유발할 수 있음.
    - mpz_t 등을 직접 return하는 것은 제대로 동작하지 않을 수 있음?
    */

    /*
    gmp 라이브러리는 thread-safe하지만, 몇가지 예외가 있다.
    - 
    */

    return 0;
}