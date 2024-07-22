#ifndef _MTRANDOM_H
#define _MTRANDOM_H

#include "random.h"

extern random_vt mtrandom_vt;

typedef struct _mtrandom_t{
	random_t rand;
	int seed;
	unsigned int mt[624];
	int index;
	float nextNextGaussian;
	char hasNextNextGaussian;
} mtrandom_t;

void mtrandom_create(mtrandom_t* r, int seed);
void mtrandom_setSeed(mtrandom_t* r, int seed);
unsigned int mtrandom_getRandInt(mtrandom_t* r);
int mtrandom_nextIntUnbounded(mtrandom_t* r);
int mtrandom_nextInt(mtrandom_t* r, int bound);
float mtrandom_nextFloat(mtrandom_t* r);

#endif
