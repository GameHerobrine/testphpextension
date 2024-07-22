#include "mtrandom.h"
#include "random.h"


random_vt mtrandom_vt = {
	mtrandom_nextFloat,
	mtrandom_nextInt
};

void mtrandom_create(mtrandom_t* r, int seed){
	r->rand.vt = &mtrandom_vt;
	mtrandom_setSeed(r, seed);
}
void mtrandom_setSeed(mtrandom_t* r, int seed){
	r->mt[0] = seed;
	r->seed = seed;
	for(r->index = 1; r->index < 624; ++r->index){
		r->mt[r->index] = 0x6c078965 * (r->mt[r->index-1] >> 30 ^ r->mt[r->index-1]) + r->index;
	}
}

static unsigned int mag[] = {0, 0x9908b0df};

unsigned int mtrandom_getRandInt(mtrandom_t* r){
	unsigned int kk, y;
	if(r->index >= 624 || r->index < 0){
		if(r->index >= 625 || r->index < 0) mtrandom_setSeed(r, 4357);

		for(kk = 0; kk < 227; ++kk){
			y = (r->mt[kk] & 0x80000000) | (r->mt[kk + 1] & 0x7fffffff);
			r->mt[kk] = r->mt[kk+397] ^ (y >> 1) ^ mag[y & 1];
		}

		for(; kk < 623; ++kk){
			y = (r->mt[kk] & 0x80000000) | (r->mt[kk + 1] & 0x7fffffff);
			r->mt[kk] = r->mt[kk-227] ^ (y >> 1) ^ mag[y & 1];
		}

		y = (r->mt[623] & 0x80000000) | (r->mt[0] & 0x7fffffff);
		r->mt[kk] = r->mt[396] ^ (y >> 1) ^ mag[y & 1];
		r->index = 0;
	}
	
	y = r->mt[r->index];
	++r->index;
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= (y >> 18);
	return y;
}
int mtrandom_nextIntUnbounded(mtrandom_t* r){
	return mtrandom_getRandInt(r) >> 1;
}
int mtrandom_nextInt(mtrandom_t* r, int bound){
	return mtrandom_getRandInt(r) % bound;
}
float mtrandom_nextFloat(mtrandom_t* r){
	return (float)mtrandom_getRandInt(r) / 0xffffffff;
}
