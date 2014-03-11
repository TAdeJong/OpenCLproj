#define OPENCL_KERNEL
#include "es_common.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f
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

__kernel void es(__global struct individual *individuals, const float tau0, const ulong seed)
{
	int id = get_global_id(0);
	ulong cur_rand = seed + id;
	mrand(&cur_rand);
	__global struct individual *p = &individuals[id];
	
	p->sigma = p->sigma * exp(tau0 * randn(&cur_rand));
	for (int i = 0; i < DIM; ++i)
	{
		p->x[i] = p->x[i] + p->sigma * randn(&cur_rand);
	}
	
	p->fitness = es_f(p->x);
}

/* vim: set ft=c : */
