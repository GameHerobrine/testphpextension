#include "noise.h"
#include "random.h"
#include "mtrandom.h"

static inline int floor(float f){
	int i = (int) f;
	if((float)i > f) --i;
	return i;
}

int grad3[12][3] = {
	{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
	{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
	{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
};

static inline float grad(int hash, float x, float y, float z){
	float u, v;
	hash &= 15;
	u = hash < 8 ? x : y;
	v = hash < 4 ? y : ((hash == 12 || hash == 14) ? x : z);
	return ((hash & 1) == 0 ? u : -u) + ((hash & 2) == 0 ? v : -v);
}


void perlin_create(perlin_t* noise, random_t* random, int octaves){
	noise->type = n_PERLIN;
	noise->octaves = octaves;
	noise->offsetX = random->vt->nextFloat(random) * 256;
	noise->offsetY = random->vt->nextFloat(random) * 256;
	noise->offsetZ = random->vt->nextFloat(random) * 256;
	
	for(int i = 0; i < 512; ++i) noise->perm[i] = 0;
	for(int i = 0; i < 256; ++i) noise->perm[i] = random->vt->nextInt(random, 256) & 0xff;
	for(int i = 0; i < 256; ++i) {
		int x = random->vt->nextInt(random, 256 - i) + i;
		int old = noise->perm[i];
		
		noise->perm[i] = noise->perm[x];
		noise->perm[x] = old & 0xff;
		noise->perm[i + 256] = noise->perm[i];
	}
}

float perlin_getNoise3D(perlin_t* n, float x, float y, float z){
	x += n->offsetX;
	y += n->offsetY;
	z += n->offsetZ;

	int floorX = floor(x);
	int floorY = floor(y);
	int floorZ = floor(z);
	
	int X = floorX & 0xff;
	int Y = floorY & 0xff;
	int Z = floorZ & 0xff;
	
	x -= floorX;
	y -= floorY;
	z -= floorZ;

	float fX = FADE(x);
	float fY = FADE(y);
	float fZ = FADE(z);
	
	int A = n->perm[X] + Y;
	int AA = n->perm[A] + Z;
	int AB = n->perm[A + 1] + Z;

	int B = n->perm[X + 1] + Y;
	int BA = n->perm[B] + Z;
	int BB = n->perm[B + 1] + Z;
	
	return LERP(fZ,
		LERP(fY, 
			LERP(fX, grad(n->perm[AA], x, y, z), 	grad(n->perm[BA], x-1, y, z)),
			LERP(fX, grad(n->perm[AB], x, y-1, z),	grad(n->perm[BB], x-1, y-1, z))
		),
		LERP(fY, 
			LERP(fX, grad(n->perm[AA + 1], x, y, z-1), grad(n->perm[BA + 1], x-1, y, z-1)),
			LERP(fX, grad(n->perm[AB + 1], x, y-1, z-1), grad(n->perm[BB + 1], x-1, y-1, z-1))
		)
	);
}

static inline float perlin_getNoise2D(perlin_t* n, float x, float y){
	return perlin_getNoise3D(n, x, y, 0);
}

float noise_noise2D(noise_t* n, float x, float z, float frequency, float amplitude){
	float result, amp, freq, max;
	int i;

	result = 0;
	amp = 1;
	freq = 1;
	max = 0;

	for(i = 0; i < n->octaves; ++i){
		switch(n->type){
			case n_PERLIN:
				result += perlin_getNoise2D(n, x * freq, z * freq) * amp;
				break;
			case n_SIMPLEX:
			default:
				printf("Not implemented: %d\n", n->type);
				goto end;
		}
		max += amp;
		freq *= frequency;
		amp *= amplitude;
	}
	
	//always normalize
	result /= max;

	end:
	return result;
}
float noise_noise3D(noise_t* n, float x, float y, float z, float frequency, float amplitude){
	float result, amp, freq, max;
	int i;

	result = 0;
	amp = 1;
	freq = 1;
	max = 0;

	for(i = 0; i < n->octaves; ++i){
		switch(n->type){
			case n_PERLIN:
				result += perlin_getNoise3D(n, x * freq, y*freq, z * freq) * amp;
				break;
			case n_SIMPLEX:
			default:
				printf("Not implemented 3D: %d\n", n->type);
				goto end;
		}
		max += amp;
		freq *= frequency;
		amp *= amplitude;
	}
	
	//always normalize
	result /= max;

	end:
	return result;
}

int main(){
	mtrandom_t rand;
	mtrandom_create(&rand, 0);
	perlin_t noise;
	perlin_create(&noise, &rand, 4);
	printf("%f\n", noise_noise3D(&noise, 0.2, 0.3, 0.4, 0.5, 0.5));
	printf("%f\n", noise_noise2D(&noise, 0.3, 0.4, 0.5, 0.5));
}

