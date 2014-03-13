#ifdef __OPENCL_VERSION__
#	define uint32_t uint;
#endif

struct rngState
{
    uint32_t x = 123456789;
    uint32_t y = 362436069;
    uint32_t z = 521288629;
    uint32_t w = 88675123;
};

#ifdef __OPENCL_VERSION__
float xorshift(__global struct rngState *state)
{
    uint32_t t;
 
    t = state->x ^ (state->x << 11);
    state->x = state->y;
	state->y = state->z;
	state->z = state->w;
	state->w ^= (state->w >> 19) ^ t ^ (state->t >> 8);
	return state->w / float(UINT_MAX);
}

#undef uint32_t
#endif

/* vim: set ft=c : */
