__kernel void vector_mandelbrot(__global const uint xsize, __global const double scale, __global const uint maxiter __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
	double cre = ((double) i/xsize)/scale;
 	double cim = ((double) i%xsize)/scale;
    double re = cre;
	double im = cim;
	double retemp;
	int result = 0;
	
	// Do the operation
	for(uint j = 0; j < maxiter; j++) {
			temp = re;
			re = re*re-im*im + cre;
			im = 2*retemp*im + cim;
			result += (int)(re*re+im*im > 4);
	}
	C[i] = result;
}
