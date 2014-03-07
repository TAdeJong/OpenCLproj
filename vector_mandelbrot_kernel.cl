#pragma OPENCL EXTENSION cl_khr_fp64: enable
__kernel void vector_mandelbrot(const uint xsize, const double scale, const uint maxiter, __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
	double cre = (i/xsize)*scale-1.0;
 	double cim = (i%xsize)*scale-1.0;
    double re = cre;
	double im = cim;
	double retemp;
	int result = 0;
	
	// Do the operation
	for(uint j = 0; j < maxiter; j++) {
			retemp = re;
			re = re*re-im*im + cre;
			im = 2*retemp*im + cim;
			result += (re*re+im*im < 4);
	}
	C[i] = result;
}

/* vim: set ft=c : */
