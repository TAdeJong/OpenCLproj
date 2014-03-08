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

float uniformplane(float x, float y)
{
	return (sin(4*M_PI*x) * cos(8*M_PI*y))/2.f+.5f;
}

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

	float rndval = uniformplane(p->x, p->y)*phi1;
	p->vx += rndval*(p->px - p->x);
	rndval = uniformplane(p->x, rndval)*phi2;
	p->vx += rndval*(gx - p->x);
	p->x = clamp(p->x + p->vx, X_MIN, X_MAX);

	rndval = uniformplane(p->x, rndval)*phi1;
	p->vy += rndval*(p->py - p->y);
	rndval = uniformplane(p->x, rndval)*phi2;
	p->vy += rndval*(gy - p->y);
	p->y = clamp(p->y + p->vy, Y_MIN, Y_MAX);
}

/* vim: set ft=c : */
