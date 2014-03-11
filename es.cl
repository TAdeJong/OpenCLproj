#define OPENCL_KERNEL
#include "es_common.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f
#define BETA 0.0873f
__constant ulong RAND_MAX = (1L << 48);

ulong mrand(ulong *seed)
{
	*seed = (*seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
	return *seed;
}

float randn(ulong *seed)
{
	float U1 = (float)mrand(seed) / (float)RAND_MAX;
	float U2 = (float)mrand(seed) / (float)RAND_MAX;
	return sqrt(-2.f*log(U1))*cos(2*PI*U2);
}

__kernel void es(__global struct individual *individuals, const ulong seed)
{
	float tauprime = 1.f / sqrt(2.f * DIM);
	float tau = 1.f / sqrt(2.f * sqrt((float)DIM));
	float tau0 = 1.f / sqrt((float)DIM);

	int id = get_global_id(0);
	ulong cur_rand = seed + id;
	mrand(&cur_rand);
	__global struct individual *ind = &individuals[id];

	/*
	ind->s[0] = ind->s[0] * exp(tau0 * randn(&cur_rand));
	for (int i = 0; i < DIM; ++i)
	{
		ind->x[i] = ind->x[i] + ind->s[0] * randn(&cur_rand);
	}
	*/

	float r1 = randn(&cur_rand);
	float s_corr[DIM];
	float d1, d2;
	for (int i = 0; i < DIM; ++i)
	{
		//so = so .* exp(tauprime * randn() + tau * randn(1, n));
		ind->s[i] *= exp(tauprime * r1 + tau * randn(&cur_rand));
		s_corr[i] = ind->s[i] * randn(&cur_rand);
	}
	for (int i = 0; i < DIM_A; ++i)
	{
		//ao = ao + beta * randn(1, an);
		ind->a[i] += fmod(BETA * randn(&cur_rand), 2*PI);
	}

	int nq = DIM_A - 1;
	int n1 = DIM;
	for (int k = 0; k < DIM-1; ++k)
	{
		n1 -= 1;
		int n2 = DIM - 1;
		for (int i = 0; i < k; ++i)
		{
			d1 = s_corr[n1];
			d2 = s_corr[n2];
			s_corr[n2] = d1 * sin(ind->a[nq]) + d2 * cos(ind->a[nq]);
			s_corr[n1] = d1 * cos(ind->a[nq]) - d2 * sin(ind->a[nq]);
			n2 -= 1;
			nq -= 1;
		}
	}

	for (int i = 0; i < DIM; ++i)
	{
		ind->x[i] += s_corr[i];
	}

	ind->fitness = es_f(ind->x);
}

/* vim: set ft=c : */
