#ifndef _RANDOM_H
#define _RANDOM_H
typedef struct _random_vt{
	float (*nextFloat)(void*);
	int (*nextInt)(void*, int);
} random_vt;

typedef struct _random_t{
	random_vt* vt;
} random_t;


#endif


