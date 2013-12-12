/*
 *  Terrain.h
 *  
 *
 *  Created by Barry Martin on 3/31/10.
 *
 */

#ifndef TERRAIN
#define TERRAIN

#include <math.h>
#include "perlinNoise.h"

class Terrain{
	public:
	Terrain();
	
	static float getElevation(perlinNoise* p, float x, float y);
	static float getElevation(perlinNoise * p, float x, float y, float persistence, float frequency);
	static float getEl(perlinNoise * p, float x, float y);
	
	~Terrain();
};



#endif

