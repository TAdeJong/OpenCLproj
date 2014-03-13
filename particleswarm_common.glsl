struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;
};

float f(float x, float y)
{
	x *= 10.f;
	y *= 10.f;
	return -pow(x, 2) - pow(y-1.5f, 2) + 1.f;
	//return 3.f + cos(y-0.87f) + 0.5f*pow(x+0.3f,2) - 0.25f*pow(x+0.3f,4);
}

/* vim: set ft=c : */
