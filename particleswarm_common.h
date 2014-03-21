#if defined(__OPENCL_VERSION__) || defined(__linux__) || defined(__APPLE__) || defined(WIN32)
#include "xorshift.h"

struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;

	struct rngState state;
};

#endif

#if defined(__VERSION__) || defined(__OPENCL_VERSION__)
float f(float x, float y)
{
	//x *= 10.f;
	//y *= 10.f;
	return (cos(4.f*3.f*(x+y)) - cos(11.f*(x-y)+0.5f))/2.f;
	//return 3.f + cos(y-0.87f) + 0.5f*pow(x+0.3f,2) - 0.25f*pow(x+0.3f,4);
}
#endif

/* vim: set ft=c : */
