struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;
};

float f(float x, float y)
{
	return (pow(x, 2)-pow(x, 4)) * cos(y);
}

__kernel void particleswarm(__global struct particle *particles, const int gbest, const int gx, const int gy)
{
	int i = get_global_id(0);
	__global struct particle *p = &particles[i];

	float fitness = f(p->x, p->y);
	if (fitness > p->pbest)
	{
		p->px = p->x;
		p->py = p->y;
	}

	p->vx += 0.5*(p->px - p->x) + 0.5*(gx - p->x);
	p->vy += 0.5*(p->py - p->y) + 0.5*(gy - p->y);

	p->x = clamp(p->x + p->vx, -1.f, 1.f);
	p->y = clamp(p->y + p->vy, -1.f, 1.f);
}

/* vim: set ft=c : */
