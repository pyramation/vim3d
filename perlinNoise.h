/*
 *  perlinNoise.h
 *  
 *
 *  Created by Barry Martin on 3/30/10.
 *
 */

#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <math.h>

class perlinNoise{

	public:
	perlinNoise();
	
	
	float perlinNoise2D(float x, float y);
	float noise2D(float x, float y);
	float noise2D2(float x, float y);
	float noise2D3(float x, float y);	

	float smoothNoise2D(float x, float y);
	float interpolate(float a, float b, float x);
	float interpolatedNoise2D(float x, float y);
	
	float perlinNoise3D(float x, float y, float z);
	//void Render;
	

	//float numberOfOctaves;
	int numberOfOctaves;
	float persistence;
	float amplitude;
	float frequency;
	
	bool freqOctDep;	
	float ampOctDep;
	bool useSum;

	float curr_seed;

	
	~perlinNoise();
	
};
	
#endif

