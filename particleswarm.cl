#include "particleswarm_common.h"

__kernel void particleswarm(__global struct particle *particles, const float gbest, const float gx, const float gy, const float phi1, const float phi2)
{
	int i = get_global_id(0);
	__global struct particle *p = &particles[i];

	float fitness = f(p->x, p->y);
	if (fitness > p->pbest)
	{
		p->pbest = fitness;
		p->px = p->x;
		p->py = p->y;
	}

	float rndval = xorshift(&p->state)*phi1;
	p->vx += rndval*(p->px - p->x);
	rndval = xorshift(&p->state)*phi2;
	p->vx += rndval*(gx - p->x);
	p->x = clamp(p->x + p->vx, X_MIN, X_MAX);

	rndval = xorshift(&p->state)*phi1;
	p->vy += rndval*(p->py - p->y);
	rndval = xorshift(&p->state)*phi2;
	p->vy += rndval*(gy - p->y);
	p->y = clamp(p->y + p->vy, Y_MIN, Y_MAX);
}

/* vim: set ft=c : */
