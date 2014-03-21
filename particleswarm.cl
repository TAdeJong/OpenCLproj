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

	p->vx *= 0.9f;
	p->vy *= 0.9f;

	float rndval = xorshift(&p->state)*phi1*(X_MAX-X_MIN)/100.f;
	p->vx += rndval*(p->px - p->x);
	rndval = xorshift(&p->state)*phi2*(X_MAX-X_MIN)/100.f;
	p->vx += rndval*(gx - p->x);
	p->x = clamp(p->x + p->vx, X_MIN, X_MAX);

	rndval = xorshift(&p->state)*phi1*(Y_MAX-Y_MIN)/100.f;
	p->vy += rndval*(p->py - p->y);
	rndval = xorshift(&p->state)*phi2*(Y_MAX-Y_MIN)/100.f;
	p->vy += rndval*(gy - p->y);
	p->y = clamp(p->y + p->vy, Y_MIN, Y_MAX);
}

/* vim: set ft=c : */
