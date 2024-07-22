#ifndef _NOISE_H
#define _NOISE_H

#define FADE(x) (x*x*x*(x*(x*6 - 15) + 10))
#define LERP(x, y, z) (y + x * (z - y))

struct _random_t;

typedef enum _noisetype_t{
	n_PERLIN,
	n_SIMPLEX
} noisetype_t;

typedef struct _noise_t{
	noisetype_t type;
	int octaves;
	float offsetX, offsetY, offsetZ;
	unsigned char perm[512];
} noise_t;


typedef noise_t perlin_t;
void perlin_create(perlin_t*, struct _random_t*, int octaves);
float perlin_getNoise3D(perlin_t*, float x, float y, float z);
static inline float perlin_getNoise2D(perlin_t*, float x, float y);
extern int grad3[12][3];


typedef noise_t simplex_t;




float noise_noise2D(noise_t*, float x, float z, float frequency, float amplitude);
float noise_noise3D(noise_t*, float x, float y, float z, float frequency, float amplitude);


#endif
