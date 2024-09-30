#include <stdio.h>
#include <math.h>
#include <malloc.h>

typedef unsigned long long int INT64;

INT64 valMin64b = 0x8000000000000000; // 100 ... 000 (64bit)
INT64 valMax64b = 0xffffffffffffffff; // 111 ... 111 (64bit)

INT64 GetBitAmount(INT64 n)
{
	INT64 count = 0;
	while (n > 0)
	{
		n = n >> 1;
		count++;
	}
	return count;
}

INT64 FastModuloExponentiation(INT64 a, INT64 x, INT64 n)
{
	INT64 y = 1;
	INT64 bits_x = 0;
	bits_x = GetBitAmount(x);
	for (INT64 i = 0; i < bits_x; i++)
	{
		if (((x >> i) & 0b1))
		{
			y = (a * y) % n;
		}
		a = (a * a) % n;
	}
	return y;
}

char MillerRabinPrimalityTest(INT64 n, INT64 k)
{
	INT64 s = 0;
	INT64 d = 0;

	// Determine s, d:
	INT64 temp_n = n - 1;
	while (((temp_n >> 1) << 1) == temp_n)
	{
		temp_n = temp_n >> 1;
		s++;
	}
	d = temp_n;
	// printf("s: %d, d: %d\n", s, d);

	// Test k times:
	//INT64 a_arr[] = {2, 3, 5, 7, 11, 13, 17, 31, 73}; // a


	INT64 *a_arr = (INT64 *)malloc(sizeof(INT64) * (n - 1));
	for(INT64 i=1; i<n; i++){
		a_arr[i-1] = i;
	}
	unsigned int len = sizeof(a_arr) / sizeof(a_arr[0]);

	
	for (INT64 j = 0; j < k; j++)
	{
		for (INT64 i = 0; i < len; i++)
		{
			INT64 a = a_arr[i];
			for (INT64 r = 0; r < s; r++)
			{
				if ((FastModuloExponentiation(a, d, n) == 1) || (FastModuloExponentiation(a, (2 * r * d), n) == 1))
				{
					return 1; // 소수를 의미
				}
			}
		}
	}

	return 0; // 합성수를 의미
}

char isPrime(INT64 n)
{
	return MillerRabinPrimalityTest(n, 50);
}

int main(void)
{
	//for (INT64 n = valMin64b; n < valMax64b; n++)
	for (INT64 n = 1000; n < 1000000; n++)
	{
		char res = MillerRabinPrimalityTest(n, 10);
		if (res)
		{
			printf("Prime: %llu\n", n);
		}
	};
	/*
	for(INT64 n = valMin64b; n<valMax64b; n++){
		if(isPrime(n)){
			printf("Prime: %llu\n", n);
		}
	}
	*/
	return 0;
}