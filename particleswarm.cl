struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;
};

float f(float x, float y)
{
	return -pow(x, 2) - pow(y-1.5f, 2) + 1.f;
	//return 3.f + cos(y-0.87f) + 0.5f*pow(x+0.3f,2) - 0.25f*pow(x+0.3f,4);
}

__kernel void particleswarm(__global struct particle *particles, const float gbest, const float gx, const float gy)
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

	p->vx += 0.5*(p->px - p->x) + 0.5*(gx - p->x);
	p->vy += 0.5*(p->py - p->y) + 0.5*(gy - p->y);

	p->x = clamp(p->x + p->vx, X_MIN, X_MAX);
	p->y = clamp(p->y + p->vy, Y_MIN, Y_MAX);
}

/* vim: set ft=c : */
