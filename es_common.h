#ifdef OPENCL_KERNEL
	typedef float cl_float;
#define CL_SCOPE_GLOBAL __global
#else
#	define CL_SCOPE_GLOBAL
#endif

#define DIM 10
#define XMIN -10.f
#define XMAX 10.f

struct individual
{
	cl_float x[DIM];
	cl_float sigma;
	cl_float fitness;
};

float es_f(CL_SCOPE_GLOBAL cl_float x[DIM])
{
	
	float res = -1.f;
	for (int i = 0; i < DIM; ++i)
	{
		res += (x[i] - 1.f/(i+1))*(x[i] - 1.f/(i+1));
	}
	/*
	float res = 0;
	for (int i = 0; i < DIM - 1; ++i)
	{
		res += 100 * pow(x[i+1] - x[i]*x[i], 2) + (x[i]-1)*(x[i]-1);
	}
	*/
	return res;
}

/* vim: set ft=c : */
