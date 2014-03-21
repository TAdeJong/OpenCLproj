#ifdef __OPENCL_VERSION__
#	define uint32_t uint
#endif

struct rngState
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
};

#ifdef __OPENCL_VERSION__
float xorshift(__global struct rngState *state)
{
    uint32_t t;
 
    t = state->x ^ (state->x << 11);
    state->x = state->y;
	state->y = state->z;
	state->z = state->w;
	state->w ^= (state->w >> 19) ^ t ^ (t >> 8);
	return state->w / ((float) UINT_MAX);
}

#undef uint32_t
#endif

/* vim: set ft=c : */
